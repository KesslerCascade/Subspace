#include "controlclient.h"
#include "osdep.h"
#include "subspacegame.h"
#include "version.h"

#include "ftl/stdlib.h"
#include "atomic.h"
#include "minicrt.h"
#include "net.h"

typedef struct ControlClient {
    atomic(bool) shouldExit;

    socket_t notifysock;   // dummy socket to wake up the thread
    int notifyport;

    lock_t msgqlock;
    MessageQueue* inbound;
    MessageQueue* outbound;
    MessageQueue* inbound_secondary;
    MessageQueue* outbound_secondary;
} ControlClient;

ControlState control;
ControlClient client;

void controlSendStartup(ControlState* cs)
{
    ControlMsgHeader mh = { 0 };
    ControlField vf = { 0 }, majvf = { 0 }, minvf = { 0 };

    mh.nfields = 3;
    strcpy(mh.cmd, "STARTUP");
    strcpy(vf.h.name, "ver");
    vf.h.ftype   = CF_STRING;
    vf.d.cfd_str = (char*)subspace_version_str;

    strcpy(majvf.h.name, "major");
    majvf.h.ftype   = CF_INT;
    majvf.d.cfd_int = subspace_version_maj;

    strcpy(minvf.h.name, "minor");
    minvf.h.ftype   = CF_INT;
    minvf.d.cfd_int = subspace_version_min;

    ControlField* fields[3] = { &vf, &majvf, &minvf };
    controlPutMsg(cs, &mh, fields);
    while (controlSend(cs)) {}
}

static int controlThread(void* data)
{
    fd_set rset, wset;
    struct timeval sto = { 0 };
    sto.tv_sec         = 10;

    while (!atomicLoad(bool, &client.shouldExit, Relaxed)) {
        FD_ZERO(&rset);
        FD_ZERO(&wset);
        FD_SET(client.notifysock, &rset);

        int maxfd = client.notifysock + 1;

        select(maxfd, &rset, NULL, NULL, &sto);

        // clear out the single byte sent to the notify socket
        if (FD_ISSET(client.notifysock, &rset)) {
            char tmp;
            recv(client.notifysock, &tmp, 1, 0);
        }
    }
    return 0;
}

bool controlClientStart(void)
{
    memset(&client, 0, sizeof(ControlClient));
    srand(time(NULL));

    for (int i = 0; i < 10000; i++) {
        struct sockaddr_in addr = { 0 };

        client.notifysock         = socket(PF_INET, SOCK_DGRAM, 0);
        client.notifyport         = rand() % 64512 + 1024;
        addr.sin_family           = AF_INET;
        addr.sin_addr.S_un.S_addr = htonl(0x7f000001);
        addr.sin_port             = htons(client.notifyport);

        if (bind(client.notifysock, (struct sockaddr*)&addr, sizeof(addr)) == 0)
            break;

        netClose(client.notifysock);
        client.notifysock = 0;
    }
    netSetNonblock(client.notifysock, true);

    if (client.notifysock == 0)
        return false;

    osStartThread(controlThread, NULL);
    return true;
}

void controlClientStop(void)
{
    atomicStore(bool, &client.shouldExit, true, Relaxed);
    controlClientNotify();
}

void controlClientNotify(void)
{
    struct sockaddr_in addr   = { 0 };
    addr.sin_family           = AF_INET;
    addr.sin_addr.S_un.S_addr = htonl(0x7f000001);
    addr.sin_port             = htons(client.notifyport);

    char nothing = '\0';
    sendto(client.notifysock, &nothing, 1, 0, (struct sockaddr*)&addr, sizeof(addr));
}
