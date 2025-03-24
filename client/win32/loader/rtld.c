#include <windows.h>
#include "minicrt.h"

#include "rtld.h"

#ifdef _DEBUG
// For debugging purposes, instead of the native loader, use LoadLibrary to load the exe. This
// registers it as a modeule and makes it easier for debuggers to locate symbol information, etc.
// #define LOAD_EXE_AS_DLL
#endif

// This whole module is basically a userspace-only reimplementaton of the Windows
// process loader and runtime linker. The only thing it doesn't handle is WOW64,
// but since this program is compiled as a 32-bit executable, that is already set
// up for us.

typedef struct EXE_HEADERS {
    IMAGE_DOS_HEADER dos;
    IMAGE_NT_HEADERS nt;
} EXE_HEADERS;

#define FILEHDR_SIZE (sizeof(IMAGE_FILE_HEADER) + sizeof(DWORD))

// Save some room in address space for resources (they MUST be above our base
// address so they can be addressed by RVA).
static char _resources[256 * 1024];

// Mucking with TLS at runtime is madness; just reserve a page for initializers
// and copy them from the exe.
__declspec(thread) char _tls[4096];

// ----- TLS stuff that is normally handled by msvcrt
ULONG _tls_index = 0;
#pragma data_seg(".tls")
char _tls_start = 0;
#pragma data_seg(".tls$ZZZ")
char _tls_end = 0;

#pragma data_seg(".rdata$T")
__declspec(allocate(".rdata$T")) extern const IMAGE_TLS_DIRECTORY _tls_used = {
    (ULONG)(ULONG_PTR)&_tls_start,   // start of tls data
    (ULONG)(ULONG_PTR)&_tls_end,     // end of tls data
    (ULONG)(ULONG_PTR)&_tls_index,   // address of tls_index
    (ULONG)0,                        // pointer to call back array
    (ULONG)0,                        // size of tls zero fill
    (ULONG)0                         // characteristics
};
#pragma data_seg()
// ----- end TLS stuff

#define ReadOrFail(f, d, dr)                                         \
    if (!ReadFile(f, &(d), sizeof(d), &dr, NULL) || dr != sizeof(d)) \
        return 0;

#define imageptr ptr(imagebase)

static int getHeader(HANDLE hFile, EXE_HEADERS* out)
{
    DWORD didread;

    ReadOrFail(hFile, out->dos, didread);
    if (out->dos.e_magic != IMAGE_DOS_SIGNATURE)
        return 0;

    if (SetFilePointer(hFile, out->dos.e_lfanew, 0, FILE_BEGIN) == INVALID_SET_FILE_POINTER)
        return 0;

    ReadOrFail(hFile, out->nt.Signature, didread);
    if (out->nt.Signature != IMAGE_NT_SIGNATURE)
        return 0;

    ReadOrFail(hFile, out->nt.FileHeader, didread);
    if (out->nt.FileHeader.SizeOfOptionalHeader < sizeof(out->nt.OptionalHeader))
        return 0;

    ReadOrFail(hFile, out->nt.OptionalHeader, didread);

    return out->nt.OptionalHeader.SizeOfImage;
}

static int loadHeaders(HANDLE hFile, EXE_HEADERS* hdrcopy, addr_t imagebase)
{
    DWORD didread, hdrsize;

    if (SetFilePointer(hFile, 0, 0, FILE_BEGIN) == INVALID_SET_FILE_POINTER)
        return 0;

    hdrsize = hdrcopy->dos.e_lfanew + FILEHDR_SIZE + hdrcopy->nt.FileHeader.SizeOfOptionalHeader +
        hdrcopy->nt.FileHeader.NumberOfSections * sizeof(IMAGE_SECTION_HEADER);
    if (!VirtualAlloc(imageptr, hdrsize, MEM_COMMIT, PAGE_READWRITE))
        return 0;

    if (!ReadFile(hFile, imageptr, hdrsize, &didread, NULL) || didread != hdrsize) {
        VirtualFree(imageptr, hdrsize, MEM_DECOMMIT);
        return 0;
    }

    // Headers are traditionally located in a read-only section
    VirtualProtect(imageptr, hdrsize, PAGE_READONLY, &didread);

    return 1;
}

static int loadSection(addr_t imagebase, IMAGE_SECTION_HEADER* sh, void* userdata)
{
    HANDLE hFile   = (HANDLE)userdata;
    DWORD filesize = min(sh->Misc.VirtualSize, sh->SizeOfRawData);
    void* sptr     = prva(imagebase, sh->VirtualAddress);
    DWORD didread;

    if (SetFilePointer(hFile, sh->PointerToRawData, 0, FILE_BEGIN) == INVALID_SET_FILE_POINTER)
        return 0;

    if (!VirtualAlloc(sptr, sh->Misc.VirtualSize, MEM_COMMIT, PAGE_READWRITE))
        return 0;

    if (!ReadFile(hFile, sptr, filesize, &didread, NULL) || didread != filesize) {
        VirtualFree(sptr, sh->Misc.VirtualSize, MEM_DECOMMIT);
        return 0;
    }

    return 1;
}

static int unprotectSection(addr_t imagebase, IMAGE_SECTION_HEADER* sh, void* userdata)
{
    void* sptr = prva(imagebase, sh->VirtualAddress);
    DWORD old;
    VirtualProtect(sptr, sh->Misc.VirtualSize, PAGE_EXECUTE_READWRITE, &old);

    return 1;
}

static int protectSection(addr_t imagebase, IMAGE_SECTION_HEADER* sh, void* userdata)
{
    void* sptr = prva(imagebase, sh->VirtualAddress);
    DWORD protect, old;

    // Set up the section's memory access
    if ((sh->Characteristics & IMAGE_SCN_MEM_WRITE) != 0 &&
        (sh->Characteristics & IMAGE_SCN_MEM_EXECUTE) != 0)
        protect = PAGE_EXECUTE_READWRITE;
    else if ((sh->Characteristics & IMAGE_SCN_MEM_READ) != 0 &&
             (sh->Characteristics & IMAGE_SCN_MEM_EXECUTE) != 0)
        protect = PAGE_EXECUTE_READ;
    else if ((sh->Characteristics & IMAGE_SCN_MEM_WRITE) != 0)
        protect = PAGE_READWRITE;
    else if ((sh->Characteristics & IMAGE_SCN_MEM_READ) != 0)
        protect = PAGE_READONLY;
    else
        protect = PAGE_NOACCESS;
    VirtualProtect(sptr, sh->Misc.VirtualSize, protect, &old);

    return 1;
}

int enumSections(addr_t imagebase, SectionCallback scb, void* userdata)
{
    IMAGE_NT_HEADERS* nthdr = nthdr(imagebase);
    int i;

    for (i = 0; i < nthdr->FileHeader.NumberOfSections; ++i) {
        if (!scb(imagebase,
                 (IMAGE_SECTION_HEADER*)rva(nthdr,
                                            FILEHDR_SIZE + nthdr->FileHeader.SizeOfOptionalHeader +
                                                i * sizeof(IMAGE_SECTION_HEADER)),
                 userdata))
            return 0;
    }

    return 1;
}

IMAGE_SECTION_HEADER* getSection(addr_t imagebase, const char* name)
{
    IMAGE_NT_HEADERS* nthdr = nthdr(imagebase);
    int i;

    for (i = 0; i < nthdr->FileHeader.NumberOfSections; ++i) {
        IMAGE_SECTION_HEADER* sh = (IMAGE_SECTION_HEADER*)
            rva(nthdr,
                FILEHDR_SIZE + nthdr->FileHeader.SizeOfOptionalHeader +
                    i * sizeof(IMAGE_SECTION_HEADER));
        if (!strcmp(sh->Name, name))
            return sh;
    }

    return NULL;
}

static int relocateImage(addr_t imagebase, addr_t origbase, IMAGE_DATA_DIRECTORY* data)
{
    IMAGE_BASE_RELOCATION* reloc = datadirptr(imagebase, data, IMAGE_BASE_RELOCATION);
    IMAGE_BASE_RELOCATION* end   = datadirend(imagebase, data, IMAGE_BASE_RELOCATION);
    DWORD i;

    while (reloc < end && reloc->SizeOfBlock) {
        for (i = sizeof(IMAGE_BASE_RELOCATION); i < reloc->SizeOfBlock; i += sizeof(WORD)) {
            WORD offset = *(WORD*)((char*)reloc + i);
            WORD rtype  = offset >> 12;
            offset &= (1 << 12) - 1;
            if (rtype == IMAGE_REL_BASED_ABSOLUTE)
                continue;
            else if (rtype == IMAGE_REL_BASED_HIGHLOW) {
                DWORD* addr = dwprva(imagebase, reloc->VirtualAddress + offset);
                *addr       = *addr - origbase + imagebase;
            } else
                return 0;   // Unhandled relocation type
        }
        reloc = (IMAGE_BASE_RELOCATION*)((char*)reloc + reloc->SizeOfBlock);
    }
    return 1;
}

static DWORD procByOrdinal(HMODULE hModule, int ovrmodule, DWORD ordinal)
{
    DWORD ovraddr = overrideProcAddressOrdinal(ovrmodule, ordinal);
    if (ovraddr || overrideAll(ovrmodule))
        return ovraddr;
    return (DWORD)GetProcAddress(hModule, (char*)ordinal);
}

static DWORD procByName(HMODULE hModule, int ovrmodule, const char* name)
{
    DWORD ovraddr = overrideProcAddress(ovrmodule, name);
    if (ovraddr || overrideAll(ovrmodule))
        return ovraddr;
    return (DWORD)GetProcAddress(hModule, name);
}

static int importDeps(addr_t imagebase, IMAGE_DATA_DIRECTORY* data)
{
    IMAGE_IMPORT_DESCRIPTOR* imp = datadirptr(imagebase, data, IMAGE_IMPORT_DESCRIPTOR);
    IMAGE_IMPORT_DESCRIPTOR* end = datadirend(imagebase, data, IMAGE_IMPORT_DESCRIPTOR);

    // SetDllDirectory(overridedir);

    while (imp < end && imp->Name) {
        HMODULE hModule = NULL;
        int ovrmodule;
        DWORD* lookup    = dwprva(imagebase,
                               imp->OriginalFirstThunk ? imp->OriginalFirstThunk : imp->FirstThunk);
        DWORD* addr      = dwprva(imagebase, imp->FirstThunk);
        const char* name = chprva(imagebase, imp->Name);

        ovrmodule = overrideModule(name);
        if (!overrideAll(ovrmodule)) {
            /*
                        LogDInfo(L"Loading library %S", name);
                        if (!stricmp(name, "opengl32.dll")) {
                            if (swrender && !stricmp(name, "opengl32.dll")) {
                                SetDllDirectory(spathToNT(spathJoin(ovrdir, "Bin/Mesa")));
                                // dir swap needed so that mesa dll can find dxtn.dll
                                hModule = LoadLibraryA(name);
                                SetDllDirectory(spathToNT(gamedir));
                            } else {
                                hModule = LoadLibraryA(name);
                            }
                            glBindSym(hModule);
                        } else { */
            hModule = LoadLibraryA(name);
            /* } */
            if (!hModule)
                continue;
        }

        while (*lookup) {
            DWORD iname = *lookup;
            if (!iname)
                break;

            if (IMAGE_SNAP_BY_ORDINAL(iname))
                *addr = procByOrdinal(hModule, ovrmodule, iname & ~IMAGE_ORDINAL_FLAG);
            else
                *addr = procByName(hModule, ovrmodule, chprva(imagebase, iname + sizeof(WORD)));
            if (!*addr)
                return 0;

            ++lookup;
            ++addr;
        }

        ++imp;
    }
    return 1;
}

static void fixResourceDir(IMAGE_RESOURCE_DIRECTORY* dir, IMAGE_DATA_DIRECTORY* data, addr_t mybase)
{
    DWORD offset;
    int i;

    for (i = 0; i < dir->NumberOfNamedEntries + dir->NumberOfIdEntries; i++) {
        IMAGE_RESOURCE_DIRECTORY_ENTRY*
            dirent = (IMAGE_RESOURCE_DIRECTORY_ENTRY*)((char*)dir +
                                                       sizeof(IMAGE_RESOURCE_DIRECTORY) +
                                                       i * sizeof(IMAGE_RESOURCE_DIRECTORY_ENTRY));
        if ((dirent->OffsetToData & IMAGE_RESOURCE_DATA_IS_DIRECTORY) != 0) {
            IMAGE_RESOURCE_DIRECTORY*
                subdir = (IMAGE_RESOURCE_DIRECTORY*)(_resources +
                                                     (dirent->OffsetToData &
                                                      ~IMAGE_RESOURCE_DATA_IS_DIRECTORY));
            fixResourceDir(subdir, data, mybase);
        } else {
            IMAGE_RESOURCE_DATA_ENTRY* leaf = (IMAGE_RESOURCE_DATA_ENTRY*)(_resources +
                                                                           dirent->OffsetToData);

            // Need to calculate the difference between the RVA of the resource entry and the
            // RVA of the original resource directory, then add it to the new base. Easy enough!
            offset             = leaf->OffsetToData - data->VirtualAddress;
            leaf->OffsetToData = addr(_resources) - mybase + offset;
        }
    }
}

static int copyResources(addr_t imagebase, IMAGE_DATA_DIRECTORY* data)
{
    addr_t mybase                = addr(GetModuleHandle(NULL));
    IMAGE_DATA_DIRECTORY* mydata = &datadir(mybase, IMAGE_DIRECTORY_ENTRY_RESOURCE);
    DWORD old;

    if (data->Size == 0)
        return 1;

    // Make sure we can modify our own PE header
    VirtualProtect(ptr(mybase), 4096, PAGE_READWRITE, &old);

    if (data->Size > sizeof(_resources))
        return 0;

    // Copy directory
    memcpy(_resources, prva(imagebase, data->VirtualAddress), data->Size);

    // Traverse resource hierarchy and update pointers
    fixResourceDir((IMAGE_RESOURCE_DIRECTORY*)_resources, data, mybase);

    // Update our PE header to point to them
    mydata->VirtualAddress = addr(_resources) - mybase;
    mydata->Size           = data->Size;

    // Change permissions back
    VirtualProtect(ptr(mybase), 4096, old, &old);

    return 1;
}

static int copyTLS(addr_t imagebase, IMAGE_DATA_DIRECTORY* data)
{
    addr_t mybase                = addr(GetModuleHandle(NULL));
    IMAGE_DATA_DIRECTORY* mydata = &datadir(mybase, IMAGE_DIRECTORY_ENTRY_TLS);
    IMAGE_TLS_DIRECTORY* mytls   = datadirptr(mybase, mydata, IMAGE_TLS_DIRECTORY);
    IMAGE_TLS_DIRECTORY* tlsinfo = datadirptr(imagebase, data, IMAGE_TLS_DIRECTORY);
    DWORD old;

    // Changing TLS parameters is highly sketchy since so much of it is done by
    // the process loader and hidden behind deeply undocumented internal interfaces.
    // Instead of trying to alter the block, just set aside a bunch of static TLS
    // memory ourselves and copy the initializer template into it.

    // TODO: Add sanity check that TLS data isn't too big to fit!

    VirtualProtect(ptr(mytls->StartAddressOfRawData), sizeof(_tls), PAGE_READWRITE, &old);
    memcpy(ptr(mytls->StartAddressOfRawData),
           ptr(tlsinfo->StartAddressOfRawData),
           tlsinfo->EndAddressOfRawData - tlsinfo->StartAddressOfRawData);
    VirtualProtect(ptr(mytls->StartAddressOfRawData), sizeof(_tls), old, &old);

    // prime main thread TLS
    memcpy(_tls,
           ptr(tlsinfo->StartAddressOfRawData),
           tlsinfo->EndAddressOfRawData - tlsinfo->StartAddressOfRawData);

    // Hack to make linker not discard the symbol as unused
    (void)_tls_used;

    return 1;
}

addr_t getSymbol(addr_t imagebase, const char* name)
{
    IMAGE_DATA_DIRECTORY* data   = &datadir(imagebase, IMAGE_DIRECTORY_ENTRY_EXPORT);
    IMAGE_EXPORT_DIRECTORY* etbl = datadirptr(imagebase, data, IMAGE_EXPORT_DIRECTORY);
    addr_t* nametbl              = prva(imagebase, etbl->AddressOfNames);
    WORD* ordinaltbl             = prva(imagebase, etbl->AddressOfNameOrdinals);
    addr_t* addrtbl              = prva(imagebase, etbl->AddressOfFunctions);
    for (int i = 0; i < etbl->NumberOfNames; i++) {
        if (!strcmp(name, (const char*)prva(imagebase, nametbl[i]))) {
            DWORD ordinal = ordinaltbl[i];
            if (ordinal < etbl->NumberOfFunctions)
                return rva(imagebase, addrtbl[ordinal]);
            return 0;
        }
    }

    return 0;
}

#ifdef LOAD_EXE_AS_DLL

// LoadLibrary-based loader for debugging

addr_t loadExe(const char* filename)
{
    EXE_HEADERS hdr;
    addr_t imagebase = 0;
    IMAGE_NT_HEADERS* nthdr;
    int needreloc = 0;

    imagebase = (addr_t)LoadLibraryExA(filename, NULL, DONT_RESOLVE_DLL_REFERENCES);
    if (!imagebase)
        goto done;

    // Save a pointer to the NT headers for convenience
    nthdr = nthdr(imagebase);

    // LoadLibrary will protect the sections; need to unprotect them to fix up dependency references
    if (!enumSections(imagebase, unprotectSection, NULL))
        goto fail;

    importDeps(imagebase, &nthdr->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT]);

    // Restore memory protection on all sections
    if (!enumSections(imagebase, protectSection, NULL))
        goto fail;

done:
    /*
    if (imagebase)
        LogDInfo(L"Successfully loaded module %S at %08x", filename, imagebase);
    else
        LogError(L"Failed to load module %S", filename); */
    return imagebase;
fail:
    if (imagebase)
        FreeLibrary((HMODULE)imagebase);
    imagebase = 0;
    goto done;
}

#else

// native loader
addr_t loadExe(const char* filename)
{
    HANDLE hFile;
    LARGE_INTEGER sz;
    EXE_HEADERS hdr;
    addr_t imagebase = 0;
    IMAGE_NT_HEADERS* nthdr;
    int needreloc = 0;

    hFile = CreateFileA(filename,
                        GENERIC_READ,
                        FILE_SHARE_READ,
                        NULL,
                        OPEN_EXISTING,
                        FILE_ATTRIBUTE_NORMAL,
                        0);
    if (!GetFileSizeEx(hFile, &sz))
        goto fail;

    // sanity check for files that are too big to possibly fit in 32-bit address space
    if (sz.HighPart > 0 || sz.LowPart > 0x7fffffff)
        goto fail;

    if (!getHeader(hFile, &hdr))
        goto fail;

    // reserve virtual address space, first try at preferred base address
    imagebase = addr(VirtualAlloc(ptr(hdr.nt.OptionalHeader.ImageBase),
                                  hdr.nt.OptionalHeader.SizeOfImage,
                                  MEM_RESERVE,
                                  PAGE_READWRITE));
    if (!imagebase && GetLastError() == ERROR_INVALID_ADDRESS) {
        // something was already there, let OS pick but we need to rebase the image
        imagebase = addr(VirtualAlloc(NULL,
                                      hdr.nt.OptionalHeader.SizeOfImage,
                                      MEM_RESERVE,
                                      PAGE_READWRITE));
        needreloc = 1;
    }
    if (!imagebase)
        goto fail;

    if (!loadHeaders(hFile, &hdr, imagebase))
        goto fail;

    // Save a pointer to the NT headers for convenience
    nthdr = nthdr(imagebase);

    if (!enumSections(imagebase, loadSection, hFile))
        goto fail;

    if (needreloc &&
        nthdr->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_BASERELOC].VirtualAddress) {
        if (!relocateImage(imagebase,
                           nthdr->OptionalHeader.ImageBase,
                           &nthdr->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_BASERELOC]))
            goto fail;
    }

    importDeps(imagebase, &nthdr->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT]);

    // Fix memory protection on all sections
    if (!enumSections(imagebase, protectSection, NULL))
        goto fail;

done:
    CloseHandle(hFile);
    /*
    if (imagebase)
        LogDInfo(L"Successfully loaded module %S at %08x", filename, imagebase);
    else
        LogError(L"Failed to load module %S", filename); */
    return imagebase;

fail:
    if (imagebase)
        VirtualFree(imageptr, 0, MEM_RELEASE);
    goto done;
}

#endif

int importResources(addr_t base)
{
    return copyResources(base, &datadir(base, IMAGE_DIRECTORY_ENTRY_RESOURCE));
}

int importTLSInitializers(addr_t base)
{
    return copyTLS(base, &datadir(base, IMAGE_DIRECTORY_ENTRY_TLS));
}
