#include "Socket.hpp"

#if !defined(EMSCRIPTEN)

#include "Core/Logging/Log.hpp"

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

    SocketTCP *SocketTCP::CreateClient(IPAddress ip)
    {
        TCPsocket socket = SDLNet_TCP_Open(&ip.m_IP);
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

    SocketUDP *SocketUDP::CreateSocket(uint16_t port)
    {
        UDPsocket socket = SDLNet_UDP_Open(port);
        return new SocketUDP(socket);
    }

    PacketUDP *SocketUDP::CreatePacket(int size)
    {
        UDPpacket *packet = SDLNet_AllocPacket(size);
        return new PacketUDP(packet);
    }

    void SocketUDP::SendPacket(PacketUDP *packet)
    {
        SDLNet_UDP_Send(m_Socket, -1, packet->m_Packet);
    }

    bool SocketUDP::ReceivePacket(PacketUDP *packet)
    {
        auto p = packet->m_Packet;
        return SDLNet_UDP_Recv(m_Socket, p);
    }

    SocketUDP::~SocketUDP()
    {
        SDLNet_UDP_Close(m_Socket);
    }

    SocketUDP::SocketUDP(UDPsocket socket)
    {
        m_Socket = socket;
    }

    IPAddress IPAddress::Create(const std::string &hostName, uint16_t port)
    {
        return IPAddress(hostName, port);
    }

    IPAddress::IPAddress(IPaddress address)
    {
        m_IP = address;
    }

    IPAddress::IPAddress(const std::string &hostName, uint16_t port)
    {
        SDLNet_ResolveHost(&m_IP, hostName.c_str(), port);
    }

    PacketUDP::PacketUDP(UDPpacket *packet)
        : m_Packet(packet)
    {
    }

    PacketUDP::~PacketUDP()
    {
        SDLNet_FreePacket(m_Packet);
    }

    void PacketUDP::SetTarget(IPAddress ip)
    {
        m_Packet->address.host = ip.m_IP.host;
        m_Packet->address.port = ip.m_IP.port;
    }

    void PacketUDP::SetData(const void *data, uint32_t size)
    {
        m_Packet->data = (Uint8 *)data;
        m_Packet->len = size;
    }

    IPAddress PacketUDP::GetTarget()
    {
        return Nexus::IPAddress(m_Packet->address);
    }

    void *PacketUDP::GetData()
    {
        return m_Packet->data;
    }
}

#endif