#include "Socket.h"

#include "Core/Logging/Log.h"

#include <string>

namespace Nexus
{
    SocketTCP *SocketTCP::CreateServer(uint16_t port)
    {
        IPaddress ip;
        if (SDLNet_ResolveHost(&ip, nullptr, port))
        {
            return nullptr;
        }

        TCPsocket socket = SDLNet_TCP_Open(&ip);
        if (socket != nullptr)
        {
            return new SocketTCP(socket);
        }

        return nullptr;
    }

    SocketTCP *SocketTCP::CreateClient(const std::string &hostName, uint16_t port)
    {
        IPaddress ip;
        if (SDLNet_ResolveHost(&ip, hostName.c_str(), port))
        {
            return nullptr;
        }

        TCPsocket socket = SDLNet_TCP_Open(&ip);
        if (socket != nullptr)
        {
            return new SocketTCP(socket);
        }

        return nullptr;
    }

    void SocketTCP::Send(const void *data, int length)
    {
        SDLNet_TCP_Send(m_Socket, data, length);
    }

    void SocketTCP::Receive(void *data, int maxLength)
    {
        SDLNet_TCP_Recv(m_Socket, data, maxLength);
    }

    SocketTCP *SocketTCP::AcceptConnection()
    {
        TCPsocket socket = SDLNet_TCP_Accept(m_Socket);
        if (socket)
        {
            return new SocketTCP(socket);
        }
        else
        {
            return nullptr;
        }
    }

    SocketTCP::~SocketTCP()
    {
        SDLNet_TCP_Close(m_Socket);
    }

    SocketTCP::SocketTCP(TCPsocket socket)
    {
        m_Socket = socket;
    }
}