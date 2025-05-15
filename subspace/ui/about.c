#include <cx/format.h>
#include <iup.h>
#include "version.h"
#include "about.h"
#include "subspaceui.h"

static int ok_action(Ihandle *ih)
{
    Ihandle* aboutwin = IupGetAttributeHandle(ih, "ABOUTWIN");
    if (aboutwin)
        IupHide(aboutwin);

    return IUP_DEFAULT;
}

void showAbout(Subspace *ss)
{
    Ihandle* sslbl = IupLabel(langGetC(ss, _S"subspace_title"));
    IupSetAttribute(sslbl, "FONT", "Helvetica, Bold 11");
    string verstr  = 0;
    strNConcat(&verstr, langGet(ss, _S"version"), _S" ", (strref)subspace_version_str);
    Ihandle *verlbl = IupLabel(strC(verstr));

    Ihandle* spc1 = IupSpace();
    IupSetAttribute(spc1, "SIZE", "1x12");

    Ihandle *copyrightlbl = IupLabel(langGetC(ss, _S"copyright"));
    Ihandle *contactlbl = IupLabel(langGetC(ss, _S"contact"));

    Ihandle* spc2 = IupSpace();
    IupSetAttribute(spc2, "SIZE", "1x12");

    Ihandle *licenselbl = IupLabel(langGetC(ss, _S"license"));
    IupSetAttribute(licenselbl, "SIZE", "250x40");
    IupSetAttribute(licenselbl, "ALIGNMENT", "ACENTER:ATOP");
    IupSetAttribute(licenselbl, "WORDWRAP", "YES");

    Ihandle* spc3 = IupSpace();
    IupSetAttribute(spc3, "SIZE", "1x12");

    Ihandle* okbutton = IupButton(langGetC(ss, _S"ok"), NULL);
    IupSetAttribute(okbutton, "SIZE", "60x20");
    IupSetCallback(okbutton, "ACTION", (Icallback)ok_action);

    Ihandle* vbox = IupVbox(sslbl, verlbl, spc1, copyrightlbl, contactlbl, spc2, licenselbl, spc3, okbutton, NULL);
    IupSetAttribute(vbox, "ALIGNMENT", "ACENTER");
    IupSetAttribute(vbox, "CMARGIN", "18x18");
    IupSetAttribute(vbox, "CGAP", "3");

    Ihandle* dlg = IupDialog(vbox);
    IupSetAttributeHandle(okbutton, "ABOUTWIN", dlg);
    IupSetAttribute(dlg, "PARENTDIALOG", "SUBSPACE_MAINWIN");
    IupSetAttribute(dlg, "TITLE", langGetC(ss, _S"subspace_about"));
    IupSetAttribute(dlg, "RESIZE", "NO");
    IupPopup(dlg, IUP_CENTERPARENT, IUP_CENTERPARENT);
}