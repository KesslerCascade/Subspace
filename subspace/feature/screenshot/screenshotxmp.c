#include <cx/container.h>
#include <cx/format.h>
#include "control/controlserver.h"
#include "feature/screenshot/screenshot.h"

bool Screenshot_populateXMP(_In_ Screenshot* self, string* out, RunInfo* run, int64 timestamp,
                            bool automatic, hashtable fields)
{
    string xmp = 0, temp = 0;

    strDup(
        &xmp,
        _S
        "<rdf:RDF xmlns:rdf=\"http://www.w3.org/1999/02/22-rdf-syntax-ns#\" xmlns:xmp=\"http://ns.adobe.com/xap/1.0/\" xmlns:ftl=\"http://subsetgames.com/FasterThanLight/screenshot/1.0\">\n");

    strAppend(&xmp, _S"<rdf:Description rdf:about=\"\">\n");
    if (htHasKey(fields, strref, _S"meta_software")) {
        strFormat(&temp,
                  _S"<xmp:CreatorTool>${string}</xmp:CreatorTool>",
                  stvar(strref, cfieldString(fields, _S"meta_software")));
        strAppend(&xmp, temp);
    }

    TimeParts tp;
    timeDecompose(&tp, timestamp);
    strFormat(
        &temp,
        _S
        "<xmp:CreateDate>${0int(4)}-${0int(2)}-${0int(2)}T${0int(2)}:${0int(2)}:${0int(2)}Z</xmp:CreateDate>\n",
        stvar(int32, tp.year),
        stvar(int32, tp.month),
        stvar(int32, tp.day),
        stvar(int32, tp.hour),
        stvar(int32, tp.minute),
        stvar(int32, tp.second));
    strAppend(&xmp, temp);
    strAppend(&xmp, _S"</rdf:Description>\n");

    // FTL namespace
    strAppend(&xmp, _S"<rdf:Description rdf:about=\"\">\n");
    strref rf = cfieldString(fields, _S"meta_shiptype");
    if (rf) {
        strFormat(&temp, _S"<ftl:Ship><![CDATA[${string}]]></ftl:Ship>", stvar(strref, rf));
        strAppend(&xmp, temp);
    }

    rf = cfieldString(fields, _S"name");
    if (rf) {
        strFormat(&temp, _S"<ftl:Name><![CDATA[${string}]]></ftl:Name>", stvar(strref, rf));
        strAppend(&xmp, temp);
    }

        rf = cfieldString(fields, _S"meta_difficulty");
    if (rf) {
        strFormat(&temp, _S"<ftl:Difficulty>${string}</ftl:Difficulty>", stvar(strref, rf));
        strAppend(&xmp, temp);
    }


    rf = cfieldString(fields, _S"meta_sector");
    if (rf) {
        strFormat(&temp, _S"<ftl:Sector>${string}</ftl:Sector>", stvar(strref, rf));
        strAppend(&xmp, temp);
    }

    rf = cfieldString(fields, _S"meta_sectorname");
    if (rf) {
        strFormat(&temp,
                  _S"<ftl:SectorName><![CDATA[${string}]]></ftl:SectorName>",
                  stvar(strref, rf));
        strAppend(&xmp, temp);
    }

    rf = cfieldString(fields, _S"meta_beacon");
    if (rf) {
        strFormat(&temp, _S"<ftl:Beacon>${string}</ftl:Beacon>", stvar(strref, rf));
        strAppend(&xmp, temp);
    }

    uint32 seed = 0;
    if (cfieldVal(uint32, fields, _S"seed", &seed)) {
        strFormat(&temp,
                  _S"<ftl:Seed>${uint}</ftl:Seed>",
                  stvar(uint32, seed));
        strAppend(&xmp, temp);
    }
    strAppend(&xmp, _S"</rdf:Description>\n");

    strAppend(&xmp, _S"</rdf:RDF>");

    strDup(out, xmp);
    strDestroy(&temp);
    strDestroy(&xmp);
    return true;
}
