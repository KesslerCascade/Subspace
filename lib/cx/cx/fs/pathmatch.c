#include "fs_private.h"
#include "cx/string.h"
#include "cx/debug/assert.h"

_Use_decl_annotations_
bool pathMatch(strref path, strref pattern, uint32 flags)
{
    striter bt_pathi = { 0 }, bt_pati = { 0 };
    striter pathi, pati;
    uint8 pathc, patc, c;

    striBorrow(&pathi, path);
    striBorrow(&pati, pattern);

    if (flags & PATH_Smart) {
        if (striPeekChar(&pati, &patc) && patc == '/') {
            // leading /, pattern is a full path
            striSeek(&pati, 1, STRI_BYTE, STRI_SET);
            flags |= PATH_LeadingDir;

            // skip any leading / in pathname too
            if (striPeekChar(&pathi, &pathc) && pathc == '/')
                striSeek(&pathi, 1, STRI_BYTE, STRI_SET);
        } else {
            // not a full path, do a filename match only
            int32 lslash = strFindR(path, strEnd, fsPathSepStr);
            if (lslash != -1)
                striSeek(&pathi, lslash + 1, STRI_BYTE, STRI_SET);
        }
    }

    for (;;) {
        if (!striChar(&pati, &patc))
            patc = 0;
        if (!striPeekChar(&pathi, &pathc))
            pathc = 0;

        switch (patc) {
        case 0:
            if ((flags & PATH_LeadingDir) && pathc == '/')
                return true;
            if (pathc == 0)
                return true;
            goto backtrack;
        case '?':
            if (pathc == 0)
                return false;
            if (pathc == '/' && !(flags & PATH_IgnorePath))
                goto backtrack;
            striAdvance(&pathi, 1);
            break;
        case '*':
            // collapse multiple asterisks
            if (!striPeekChar(&pati, &c))
                c = 0;
            while (c == '*')
                if (!striChar(&pati, &c))
                    c = 0;

            // optimize for pattern with * at end or before /
            if (c == 0) {
                if (!(flags & PATH_IgnorePath))
                    return ((flags & PATH_LeadingDir) || strFind(path, pathi.off + pathi.cursor, fsPathSepStr) == -1) ? true : false;
                else
                    return true;
            } else if (c == '/' && !(flags & PATH_IgnorePath)) {
                int32 off = strFind(path, pathi.off + pathi.cursor, fsPathSepStr);
                if (off == -1)
                    return false;
                striSeek(&pathi, off, STRI_BYTE, STRI_SET);
                break;
            }

            // Try the shortest match possible (0 characters) first
            bt_pati = pati;
            bt_pathi = pathi;
            break;
        case '\\':
            if (!striChar(&pati, &patc))
                return false;

            // intentional fallthrough
        default:
            striAdvance(&pathi, 1);
            if (pathc == patc)
                ;
            else if ((flags & PATH_CaseInsensitive) && tolower(pathc) == tolower(patc))
                ;
            else {
        backtrack:
                // in the event of a mismatch, go back to the last '*'
                // and match one more character
                if (bt_pati.len == 0)
                    return false;
                if (!striPeekChar(&bt_pathi, &pathc))
                    return false;
                if (pathc == '/' && !(flags & PATH_IgnorePath))
                    return false;
                striAdvance(&bt_pathi, 1);
                pati = bt_pati;
                pathi = bt_pathi;
            }
            break;
        }
    }

    devFatalError("Reached unreachable code");
    return 0;
}
