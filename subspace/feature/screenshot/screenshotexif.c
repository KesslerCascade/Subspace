#include <cx/format.h>
#include <libexif/exif-data.h>
#include "control/controlserver.h"
#include "feature/screenshot/screenshot.h"
#include "subspace.h"

/* Get an existing tag, or create one if it doesn't exist */
static ExifEntry* init_tag(ExifData* exif, ExifIfd ifd, ExifTag tag)
{
    ExifEntry* entry;
    /* Return an existing tag if one exists */
    if (!((entry = exif_content_get_entry(exif->ifd[ifd], tag)))) {
        /* Allocate a new entry */
        entry = exif_entry_new();
        assert(entry != NULL); /* catch an out of memory condition */
        entry->tag = tag;      /* tag must be set before calling
                                  exif_content_add_entry */

        /* Attach the ExifEntry to an IFD */
        exif_content_add_entry(exif->ifd[ifd], entry);

        /* Allocate memory for the entry and fill with default data */
        exif_entry_initialize(entry, tag);

        /* Ownership of the ExifEntry has now been passed to the IFD.
         * One must be very careful in accessing a structure after
         * unref'ing it; in this case, we know "entry" won't be freed
         * because the reference count was bumped when it was added to
         * the IFD.
         */
        exif_entry_unref(entry);
    }
    return entry;
}

/* Create a brand-new tag with a data field of the given length, in the
 * given IFD. This is needed when exif_entry_initialize() isn't able to create
 * this type of tag itself, or the default data length it creates isn't the
 * correct length.
 */
static ExifEntry* create_tag(ExifData* exif, ExifIfd ifd, ExifTag tag, size_t len)
{
    void* buf;
    ExifEntry* entry;

    /* Create a memory allocator to manage this ExifEntry */
    ExifMem* mem = exif_mem_new_default();
    assert(mem != NULL); /* catch an out of memory condition */

    /* Create a new ExifEntry using our allocator */
    entry = exif_entry_new_mem(mem);
    assert(entry != NULL);

    /* Allocate memory to use for holding the tag data */
    buf = exif_mem_alloc(mem, len);
    assert(buf != NULL);

    /* Fill in the entry */
    entry->data       = buf;
    entry->size       = len;
    entry->tag        = tag;
    entry->components = len;
    entry->format     = EXIF_FORMAT_UNDEFINED;

    /* Attach the ExifEntry to an IFD */
    exif_content_add_entry(exif->ifd[ifd], entry);

    /* The ExifMem and ExifEntry are now owned elsewhere */
    exif_mem_unref(mem);
    exif_entry_unref(entry);

    return entry;
}

static void exif_set_string(ExifEntry* ent, strref str)
{
    xaFree(ent->data);
    size_t bufsz    = strLen(str) + 1;
    ent->components = bufsz;
    ent->format     = EXIF_FORMAT_ASCII;
    ent->size       = bufsz;
    ent->data       = xaAlloc(bufsz);
    strCopyOut(str, 0, ent->data, bufsz);
}

bool Screenshot_populateEXIF(_In_ Screenshot* self, uint8** buf, uint32* bufsz, RunInfo* run,
                             int64 timestamp, bool automatic, hashtable fields)
{
    ExifData* exif   = exif_data_new();
    ExifEntry* entry = NULL;
    string temp      = 0;

    exif_data_set_option(exif, EXIF_DATA_OPTION_FOLLOW_SPECIFICATION);
    exif_data_set_data_type(exif, EXIF_DATA_TYPE_COMPRESSED);
    exif_data_set_byte_order(exif, EXIF_BYTE_ORDER_INTEL);

    exif_data_fix(exif);

    entry = init_tag(exif, EXIF_IFD_EXIF, EXIF_TAG_SOFTWARE);
    exif_set_string(entry, cfieldString(fields, _S"meta_software"));

    int w = cfieldValD(int32, fields, _S"w", 0);
    int h = cfieldValD(int32, fields, _S"h", 0);

    entry = init_tag(exif, EXIF_IFD_EXIF, EXIF_TAG_PIXEL_X_DIMENSION);
    exif_set_long(entry->data, EXIF_BYTE_ORDER_INTEL, w);
    entry = init_tag(exif, EXIF_IFD_EXIF, EXIF_TAG_PIXEL_Y_DIMENSION);
    exif_set_long(entry->data, EXIF_BYTE_ORDER_INTEL, h);
    entry = init_tag(exif, EXIF_IFD_EXIF, EXIF_TAG_COLOR_SPACE);
    exif_set_short(entry->data, EXIF_BYTE_ORDER_INTEL, 1);   // sRGB

    entry = init_tag(exif, EXIF_IFD_EXIF, EXIF_TAG_DATE_TIME_ORIGINAL);
    TimeParts tp;
    timeDecompose(&tp, toLocalTime(timestamp));
    strFormat(&temp,
              _S"${0int(4)}:${0int(2)}:${0int(2)} ${0int(2)}:${0int(2)}:${0int(2)}",
              stvar(int32, tp.year),
              stvar(int32, tp.month),
              stvar(int32, tp.day),
              stvar(int32, tp.hour),
              stvar(int32, tp.minute),
              stvar(int32, tp.second));
    exif_set_string(entry, temp);

    // EXIF has really poor support for time zones, this is the best we can do and most software
    // ignores it anyway
    int curday     = tp.day;
    tp.hour        = 0;
    tp.minute      = 0;
    tp.second      = 0;
    tp.usec        = 0;
    int64 testtime = timeCompose(&tp);
    testtime       = toLocalTime(testtime);
    timeDecompose(&tp, testtime);

    entry = init_tag(exif, EXIF_IFD_EXIF, EXIF_TAG_OFFSET_TIME_ORIGINAL);
    if (tp.day != curday) {
        tp.hour = 24 - tp.hour;
        if (tp.minute > 0) {
            tp.hour--;
            tp.minute = 60 - tp.minute;
        }
        strFormat(&temp, _S"-${0int(2)}:${0int(2)}", stvar(int32, tp.hour), stvar(int32, tp.minute));
    } else {
        strFormat(&temp, _S"+${0int(2)}:${0int(2)}", stvar(int32, tp.hour), stvar(int32, tp.minute));
    }
    exif_set_string(entry, temp);

    string exifcomment = 0;
    strref rf          = cfieldString(fields, _S"meta_shiptype");
    if (rf) {
        strFormat(&temp, _S"Ship: ${string}\r\n", stvar(strref, rf));
        strAppend(&exifcomment, temp);
    }

    rf = cfieldString(fields, _S"name");
    if (rf) {
        strFormat(&temp, _S"Name: ${string}\r\n", stvar(strref, rf));
        strAppend(&exifcomment, temp);
    }

    rf = cfieldString(fields, _S"meta_difficulty");
    if (rf) {
        strFormat(&temp, _S"Difficulty: ${string}\r\n", stvar(strref, rf));
        strAppend(&exifcomment, temp);
    }

    rf = cfieldString(fields, _S"meta_sector");
    if (rf) {
        strFormat(&temp, _S"Sector: ${string}\r\n", stvar(strref, rf));
        strAppend(&exifcomment, temp);
    }

    rf = cfieldString(fields, _S"meta_sectorname");
    if (rf) {
        strFormat(&temp, _S"Sector Name: ${string}\r\n", stvar(strref, rf));
        strAppend(&exifcomment, temp);
    }

    rf = cfieldString(fields, _S"meta_beacon");
    if (rf) {
        strFormat(&temp, _S"Beacon: ${string}\r\n", stvar(strref, rf));
        strAppend(&exifcomment, temp);
    }

    uint32 seed = 0;
    if (cfieldVal(uint32, fields, _S"seed", &seed)) {
        strFormat(&temp, _S"Seed: ${uint}\r\n", stvar(uint32, seed));
        strAppend(&exifcomment, temp);
    }

    entry = create_tag(exif,
                       EXIF_IFD_EXIF,
                       EXIF_TAG_USER_COMMENT,
                       8 + (strLen(exifcomment) + 1) * 2);
    /* Write the special header needed for a comment tag */
    memcpy(entry->data, "UNICODE\0", 8);
    /* Write the actual comment text, without the trailing NUL character */
    uint16* u16buf = strToUTF16A(exifcomment);
    memcpy(entry->data + 8, u16buf, (strLen(exifcomment) + 1) * 2);
    xaFree(u16buf);
    strDestroy(&exifcomment);

    exif_data_save_data(exif, buf, bufsz);

    exif_data_unref(exif);
    strDestroy(&temp);
    return true;
}
