#pragma once

#include "SDL_net.h"

#include <string>
#include <inttypes.h>

namespace Nexus
{
    class SocketTCP
    {
    public:
        static SocketTCP *CreateServer(uint16_t port);
        static SocketTCP *CreateClient(const std::string &ip, uint16_t port);

        void Send(const void *data, int length);
        void Receive(void *data, int length);
        SocketTCP *AcceptConnection();

        ~SocketTCP();

    private:
        SocketTCP(TCPsocket socket);

    private:
        TCPsocket m_Socket;
    };
}