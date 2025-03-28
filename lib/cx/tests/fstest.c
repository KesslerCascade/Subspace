#include <stdio.h>
#include <cx/fs.h>
#include <cx/string.h>

#define TEST_FILE fstest
#define TEST_FUNCS fstest_funcs
#include "common.h"

static int test_fs_pathmatch()
{
    int ret = 0;
    string tpath1 = 0;
    string tpath2 = 0;
    string tpath3 = 0;
    string tpath4 = 0;

    strCopy(&tpath1, _S"/abs/path/to/dir1/file7.txt");
    strCopy(&tpath2, _S"/abs/other/some/dir2/file99.bin");
    strCopy(&tpath3, _S"rel/path/to/dir1/file7.txt");
    strCopy(&tpath4, _S"rel/other/some/dir2/file99.bin");

    // basic path matching
    if (pathMatch(tpath1, _S"/abs", 0))
        ret = 1;
    if (!pathMatch(tpath1, _S"/abs", PATH_LeadingDir))
        ret = 1;

    if (pathMatch(tpath3, _S"/rel", 0))
        ret = 1;
    if (pathMatch(tpath3, _S"/rel", PATH_LeadingDir))
        ret = 1;

    if (pathMatch(tpath1, _S"abs", 0))
        ret = 1;
    if (pathMatch(tpath1, _S"abs", PATH_LeadingDir))
        ret = 1;

    if (pathMatch(tpath3, _S"rel", 0))
        ret = 1;
    if (!pathMatch(tpath3, _S"rel", PATH_LeadingDir))
        ret = 1;

    // longer paths
    if (pathMatch(tpath1, _S"/abs/path/to", 0))
        ret = 1;
    if (!pathMatch(tpath1, _S"/abs/path/to", PATH_LeadingDir))
        ret = 1;
    if (pathMatch(tpath1, _S"/abs/path/to/dir1", 0))
        ret = 1;
    if (!pathMatch(tpath1, _S"/abs/path/to/dir1", PATH_LeadingDir))
        ret = 1;
    if (!pathMatch(tpath1, _S"/abs/path/to/dir1/file*", 0))
        ret = 1;
    if (!pathMatch(tpath1, _S"/abs/path/to/dir1/file*", PATH_LeadingDir))
        ret = 1;

    // wildcards
    if (pathMatch(tpath1, _S"/abs/path/to/di*/file8.txt", 0))
        ret = 1;
    if (!pathMatch(tpath1, _S"/abs/path/to/di*/file7.txt", 0))
        ret = 1;
    if (pathMatch(tpath1, _S"/abs/path/p?/dir1/file7.txt", 0))
        ret = 1;
    if (!pathMatch(tpath1, _S"/abs/path/t?/dir1/file7.txt", 0))
        ret = 1;
    if (pathMatch(tpath1, _S"/abs/p?h/to/dir1/file7.txt", 0))
        ret = 1;
    if (!pathMatch(tpath1, _S"/abs/p*h/to/dir1/file7.txt", 0))
        ret = 1;
    if (pathMatch(tpath1, _S"/???\?/p*h/to/dir1/file7.txt", 0))
        ret = 1;
    if (!pathMatch(tpath1, _S"/??\?/p*h/to/dir1/file7.txt", 0))
        ret = 1;

    // ignoring paths
    if (pathMatch(tpath1, _S"/abs/*/file7.txt", 0))
        ret = 1;
    if (!pathMatch(tpath1, _S"/abs/*/file7.txt", PATH_IgnorePath))
        ret = 1;
    if (pathMatch(tpath2, _S"*ile99.bin", 0))
        ret = 1;
    if (!pathMatch(tpath2, _S"*ile99.bin", PATH_IgnorePath))
        ret = 1;
    if (pathMatch(tpath3, _S"*/file7.txt", 0))
        ret = 1;
    if (!pathMatch(tpath3, _S"*/file7.txt", PATH_IgnorePath))
        ret = 1;
    if (pathMatch(tpath4, _S"rel/*", 0))
        ret = 1;
    if (!pathMatch(tpath4, _S"rel/*", PATH_IgnorePath))
        ret = 1;

    // case insensitive
    if (pathMatch(tpath2, _S"/abs/other/some/dir2/File99.bin", 0))
        ret = 1;
    if (!pathMatch(tpath2, _S"/abs/other/some/dir2/File99.bin", PATH_CaseInsensitive))
        ret = 1;
    if (pathMatch(tpath4, _S"REL/other/some/dir2/file99.bin", 0))
        ret = 1;
    if (!pathMatch(tpath4, _S"REL/other/some/dir2/file99.bin", PATH_CaseInsensitive))
        ret = 1;

    // smart mode
    if (pathMatch(tpath1, _S"/abs/path", 0))
        ret = 1;
    if (!pathMatch(tpath1, _S"/abs/path", PATH_Smart))
        ret = 1;
    if (pathMatch(tpath2, _S"/abs/other/s??\?/dir2", 0))
        ret = 1;
    if (!pathMatch(tpath2, _S"/abs/other/s??\?/dir2", PATH_Smart))
        ret = 1;
    if (pathMatch(tpath3, _S"/rel/path", 0))
        ret = 1;
    if (!pathMatch(tpath3, _S"/rel/path", PATH_Smart))
        ret = 1;
    if (pathMatch(tpath4, _S"/*/*/*/*/file99.bin", 0))
        ret = 1;
    if (!pathMatch(tpath4, _S"/*/*/*/*/file99.bin", PATH_Smart))
        ret = 1;

    if (pathMatch(tpath1, _S"file7.txt", 0))
        ret = 1;
    if (!pathMatch(tpath1, _S"file7.txt", PATH_Smart))
        ret = 1;
    if (pathMatch(tpath2, _S"*.bin", 0))
        ret = 1;
    if (!pathMatch(tpath2, _S"*.bin", PATH_Smart))
        ret = 1;
    if (pathMatch(tpath3, _S"????7.??t", 0))
        ret = 1;
    if (!pathMatch(tpath3, _S"????7.??t", PATH_Smart))
        ret = 1;
    if (pathMatch(tpath4, _S"*99*", 0))
        ret = 1;
    if (pathMatch(tpath4, _S"*2*", PATH_Smart))
        ret = 1;
    if (!pathMatch(tpath4, _S"*99*", PATH_Smart))
        ret = 1;

    strDestroy(&tpath1);
    strDestroy(&tpath2);
    strDestroy(&tpath3);
    strDestroy(&tpath4);
    return ret;
}

testfunc fstest_funcs[] = {
    { "pathmatch", test_fs_pathmatch },
    { 0, 0 }
};
