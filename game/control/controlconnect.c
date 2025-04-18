#include "subspacegame.h"
#include "net.h"

bool controlConnect(socket_t *sock) {
    struct sockaddr_in addr   = { 0 };
    addr.sin_family           = AF_INET;
    addr.sin_addr.S_un.S_addr = htonl(settings.addr);
    addr.sin_port             = htons(settings.port);

    socket_t csock = socket(PF_INET, SOCK_STREAM, 0);
    if (!csock) {
        OSShowError("An unexpected network error occurred.");
        return false;
    }

    if (connect(csock, (struct sockaddr*)&addr, sizeof(addr)) != 0) {
        OSShowError("Failed to connect to main Subspace program.");
        return false;
    }

    *sock = csock;
    return true;
}