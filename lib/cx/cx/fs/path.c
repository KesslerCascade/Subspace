#include "fs_private.h"
#include "cx/container/sarray.h"

// platform-agnostic path manipulation

string fsPathSepStr = _S"/";
string fsNSSepStr = _S":";

// Get parent directory
_Use_decl_annotations_
bool pathParent(string *out, strref path)
{
    int32 len = strLen(path);
    int32 sep = strFindR(path, len - 1, fsPathSepStr);
    if (sep == -1)
        return false;

    // handle root of namespace
    if (sep == 0 || strGetChar(path, sep - 1) == ':')
        sep++;

    return strSubStr(out, path, 0, sep);
}

_Use_decl_annotations_
bool pathFilename(string *out, strref path)
{
    int sep = strFindR(path, strEnd, fsPathSepStr);
    if (sep != -1)
        return strSubStr(out, path, sep + 1, strEnd);

    strDup(out, path);
    return false;
}

_Use_decl_annotations_
bool _pathJoin(string *out, int n, strref* elements)
{
    string npath = 0;
    bool donefirst = false;
    bool lastroot = false;

    for (int i = 0; i < n; i++) {
        if (!strEmpty(elements[i])) {
            // absolute path in the middle of a join makes no sense
            if (i > 0 && pathIsAbsolute(elements[i])) {
                strDestroy(&npath);
                return false;
            }
            if (donefirst) {
                if (!lastroot) {
                    strNConcat(&npath, npath, fsPathSepStr, elements[i]);
                } else {
                    // already have a path separator
                    strAppend(&npath, elements[i]);
                    lastroot = false;
                }
            } else {
                strDup(&npath, elements[i]);
                if (strFindR(npath, strEnd, fsPathSepStr) == strLen(npath) - strLen(fsPathSepStr))
                    lastroot = true;        // this is a root that ends with the path separator
                donefirst = true;
            }
        }
    }

    strDestroy(out);
    *out = npath;
    return true;
}

_Use_decl_annotations_
void pathAddExt(string *out, strref path, strref ext)
{
    strNConcat(out, path, _S".", ext);
}

_Use_decl_annotations_
bool pathRemoveExt(string *out, strref path)
{
    if (strEmpty(path))
        return false;

    int32 dot = strFindR(path, strEnd, _S".");
    int32 sep = strFindR(path, strEnd, fsPathSepStr);
    if (dot < clamplow(sep, 1))
        return false;

    strSubStr(out, path, 0, dot);
    return true;
}

_Use_decl_annotations_
bool pathGetExt(string *out, strref path)
{
    if (!path)
        return false;

    int32 dot = strFindR(path, strEnd, _S".");
    int32 sep = strFindR(path, strEnd, fsPathSepStr);
    if (dot < clamplow(sep, 1))
        return false;

    strSubStr(out, path, dot + 1, strEnd);
    return true;
}

_Use_decl_annotations_
void pathSetExt(string *out, strref path, strref ext)
{
    pathRemoveExt(out, path);
    pathAddExt(out, *out, ext);
}

_Use_decl_annotations_
bool pathIsAbsolute(strref path)
{
    // namespaced paths are always absolute
    if (strFind(path, 0, fsNSSepStr) != -1)
        return true;
    if (strGetChar(path, 0) == '/')
        return true;
    return false;
}

_Use_decl_annotations_
bool pathSplitNS(string *nspart, string *pathpart, strref path)
{
    int32 idx = strFind(path, 0, fsNSSepStr);

    string rns = 0, rpath = 0;
    if (!nspart || !pathpart)
        return false;

    if (idx != -1) {
        strSubStr(&rns, path, 0, idx);
        strSubStr(&rpath, path, idx + 1, strEnd);
    } else {
        strDup(&rpath, path);
    }

    strDestroy(nspart);
    *nspart = rns;
    strDestroy(pathpart);
    *pathpart = rpath;
    return true;
}

static bool pathNormalized(_In_opt_ strref path)
{
    striter pi;
    striBorrow(&pi, path);
    char backbuf[5] = { 0 };
    bool nsdone = false, nscheck = false;

    while (pi.len > 0) {
        for (uint32 i = 0; i < pi.len; i++) {
            char ch = pi.bytes[i];

            if (ch == '\\')
                return false;       // backslash needs converting

            if (nscheck) {
                if (ch != '/')
                    return false;   // degenerate namespaced relative path
                nscheck = false;
            }

            if (!nsdone && ch == ':') {
                nsdone = true;
                nscheck = true;
            }

            if (ch == '/') {
                // check for parent and current directory references
                if (backbuf[4] == '.' && backbuf[3] == '.' && backbuf[2] == '/')
                    return false;
                if (backbuf[4] == '.' && backbuf[3] == '/')
                    return false;

                // and double slash
                if (backbuf[4] == '/')
                    return false;
            }

            // cheesy but fast 4-byte copy
            *(uint32*)backbuf = *(uint32*)(backbuf + 1);
            backbuf[4] = ch;
        }
        striNext(&pi);
    }

    // check for directory refs at the end
    if (backbuf[4] == '.' && backbuf[3] == '.' && backbuf[2] == '/')
        return false;
    if (backbuf[4] == '.' && backbuf[3] == '/')
        return false;

    // all good!
    return true;
}

_Use_decl_annotations_
bool pathDecompose(string *ns, sa_string *components, strref pathin)
{
    string rpath = 0;

    pathSplitNS(ns, &rpath, pathin);
    // if there are any backslashes, turn them to forward slashes
    int32 idx = 0;
    while ((idx = strFind(rpath, idx, _S"\\")) != -1)
        strSetChar(&rpath, idx, '/');

    bool absolute = pathIsAbsolute(rpath);
    strSplit(components, rpath, fsPathSepStr, true);

    for (int32 i = 0, csz = saSize(*components); i < csz; i++) {
        if (i > 0 && strEmpty(components->a[i])) {
            // remove empty components, but not from position 0 (absolute path)
            saRemove(components, i--);
            csz--;
        } else if (i > 0 && strEq(components->a[i], _S"..")) {
            // eat previous component
            // only allowed work in position 1 or later to avoid breaking relative paths
            saRemove(components, i--);
            csz--;
            if (!absolute || i > 0) {
                // and absolute paths cannot remove the first empty component
                saRemove(components, i--);
                csz--;
            }
        } else if (strEq(components->a[i], _S".")) {
            // does nothing, just delete this component
            saRemove(components, i--);
            csz--;
        }
    }

    // handle degenerate case of namespace: or namespace:path with no /
    if (absolute && (saSize(*components) == 0 || !strEmpty(components->a[0]))) {
        saInsert(components, 0, string, _S);
    }

    strDestroy(&rpath);
    return absolute;
}

_Use_decl_annotations_
bool pathCompose(string *out, strref ns, sa_string components)
{
    string rpath = 0;

    strJoin(&rpath, components, fsPathSepStr);
    if (saSize(components) == 1 && strEmpty(components.a[0]))
        strAppend(&rpath, fsPathSepStr);                // this was absolute with only a root

    if (!strEmpty(ns))
        strNConcat(out, ns, fsNSSepStr, rpath);
    else
        strDup(out, rpath);

    strDestroy(&rpath);
    return true;
}

_Use_decl_annotations_
void pathNormalize(string *path)
{
    string nspace = 0;

    if (!pathNormalized(*path)) {
        sa_string components;
        saInit(&components, string, 8, SA_Grow(Aggressive));
        pathDecompose(&nspace, &components, *path);
        pathCompose(path, nspace, components);
        saDestroy(&components);
        strDestroy(&nspace);
    }
}
