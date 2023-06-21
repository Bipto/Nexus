#pragma once

#include "SDL_net.h"

#include <string>
#include <inttypes.h>

namespace Nexus
{
    class IPAddress
    {
    public:
        IPAddress() = delete;
        static IPAddress Create(const std::string &hostName, uint16_t port);

    private:
        IPAddress(IPaddress address);
        IPAddress(const std::string &hostName, uint16_t port);

    private:
        IPaddress m_IP;
        friend class SocketTCP;
        friend class SocketUDP;
        friend struct PacketUDP;
    };

    class SocketTCP
    {
    public:
        static SocketTCP *CreateServer(uint16_t port);
        static SocketTCP *CreateClient(IPAddress ip);

        void Send(const void *data, int length);
        void Receive(void *data, int maxLength);
        SocketTCP *AcceptConnection();

        ~SocketTCP();

    private:
        SocketTCP(TCPsocket socket);

    private:
        TCPsocket m_Socket;
    };

    struct PacketUDP
    {
    public:
        PacketUDP() = delete;
        PacketUDP(UDPpacket *packet)
            : m_Packet(packet) {}
        ~PacketUDP();

        void SetTarget(IPAddress ip);
        void SetData(const void *data, uint32_t size);

        IPAddress GetTarget();
        void *GetData();

        UDPpacket *m_Packet;
    };

    class SocketUDP
    {
    public:
        static SocketUDP *CreateSocket(uint16_t port);
        PacketUDP *CreatePacket(int size);
        void SendPacket(PacketUDP *packet);
        bool ReceivePacket(PacketUDP *packet);
        ~SocketUDP();

    private:
        SocketUDP(UDPsocket socket);

    private:
        UDPsocket m_Socket;
    };

}