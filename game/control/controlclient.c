#include "subspacegame.h"
#include "controlclient.h"
#include "version.h"

#include "minicrt.h"

ControlState control;

void controlSendHello(ControlState* cs)
{
    ControlMsgHeader mh = {0};
    ControlField vf = {0}, majvf = {0}, minvf = {0};

    mh.nfields = 3;
    strcpy(mh.cmd, "HELLO");
    strcpy(vf.h.name, "ver");
    vf.h.ftype = CF_STRING;
    vf.d.cfd_str = (char*)subspace_version_str;

    strcpy(majvf.h.name, "major");
    majvf.h.ftype = CF_INT;
    majvf.d.cfd_int = subspace_version_maj;

    strcpy(minvf.h.name, "minor");
    minvf.h.ftype = CF_INT;
    minvf.d.cfd_int = subspace_version_min;

    ControlField* fields[3] = { &vf, &majvf, &minvf };
    controlPutMsg(cs, &mh, fields);
    while (controlSend(cs)) {}
}