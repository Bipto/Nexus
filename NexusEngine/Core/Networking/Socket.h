#pragma once

#if !defined(EMSCRIPTEN)

#include "SDL_net.h"

#include <string>
#include <inttypes.h>

namespace Nexus
{
    /// @brief A class representing an IP address to connect to
    class IPAddress
    {
    public:
        /// @brief An empty IP address cannot be created
        IPAddress() = delete;

        /// @brief A static method to create a new IP address
        /// @param hostName A string containing the hostname of the IP address
        /// @param port The port that the socket should connect to
        /// @return
        static IPAddress Create(const std::string &hostName, uint16_t port);

    private:
        /// @brief A constructor that takes in an IP address
        /// @param address An IP address to copy
        IPAddress(IPaddress address);

        /// @brief A constructor that takes in a hostname and a port
        /// @param hostName A string containing the hostname to convert to an IP address
        /// @param port The port to connect the socket to
        IPAddress(const std::string &hostName, uint16_t port);

    private:
        /// @brief The SDL_net IP address
        IPaddress m_IP;

        /// @brief A friend class to allow sockets to access private members of the IP address
        friend class SocketTCP;

        /// @brief A friend class to allow sockets to access private members of the IP address
        friend class SocketUDP;

        /// @brief A friend class to allow packets to access private members of the IP address
        friend struct PacketUDP;
    };

    /// @brief A class representing a socket connecting through TCP
    class SocketTCP
    {
    public:
        /// @brief A static method to create a server socket
        /// @param port A port to open the server on
        /// @return A pointer to new socket
        static SocketTCP *CreateServer(uint16_t port);

        /// @brief A static method to create a client socket
        /// @param ip An IP address to a server
        /// @return A pointer to the new socket
        static SocketTCP *CreateClient(IPAddress ip);

        /// @brief A method to send data to a socket
        /// @param data A void pointer to the data to send
        /// @param length The length of the data in bytes
        void Send(const void *data, int length);

        /// @brief A method to retrieve data from a socket
        /// @param data A void pointer to put the incoming data into
        /// @param maxLength The maximum length of data that can be receieved
        void Receive(void *data, int maxLength);

        /// @brief A method to accept an incoming client connection
        /// @return A nullptr if no connection could be opened or a pointer to a new socket
        SocketTCP *AcceptConnection();

        /// @brief A destructor to allow resources from the socket to be cleaned up
        ~SocketTCP();

    private:
        /// @brief A private constructor allowing a Nexus TCP socket to be created from an SDL TCP socket
        /// @param socket
        SocketTCP(TCPsocket socket);

    private:
        /// @brief An SDL socket to use to transmit and receive data
        TCPsocket m_Socket;
    };

    /// @brief A struct representing a set of data to transmit through UDP
    struct PacketUDP
    {
    public:
        /// @brief An empty UDP packet cannot be created
        PacketUDP() = delete;

        /// @brief A constructor taking in an SDL_net UDP packet pointer
        /// @param packet
        PacketUDP(UDPpacket *packet);

        /// @brief A destructor to clean up resources from the packet
        ~PacketUDP();

        /// @brief A method to set a target IP address for the packet
        /// @param ip The IP address to send the packet to
        void SetTarget(IPAddress ip);

        /// @brief A method to set the data of the UDP packet
        /// @param data A void pointer to the data to transmit
        /// @param size The total size in bytes of the data to transmit
        void SetData(const void *data, uint32_t size);

        /// @brief A method to get the current target of the UDP socket
        /// @return An IP address pointing to the target of the packet
        IPAddress GetTarget();

        /// @brief A method to get a pointer to the data to be transmitted
        /// @return A void pointer to the data
        void *GetData();

    private:
        /// @brief An SDL_net UDP packet pointer
        UDPpacket *m_Packet;

        /// @brief A friend class to allow UDP sockets to have access to the private properties of a packet
        friend class SocketUDP;
    };

    /// @brief A class representing a UDP socket connection
    class SocketUDP
    {
    public:
        /// @brief A static method that creates a UDP socket from a port
        /// @param port The port to transmit the data from
        /// @return A pointer to a new UDP socket
        static SocketUDP *CreateSocket(uint16_t port);

        /// @brief A method to create a packet from a size
        /// @param size The size of the packet in bytes
        /// @return A pointer to the new packet
        PacketUDP *CreatePacket(int size);

        /// @brief A method to send a packet via UDP
        /// @param packet The packet to send from the socket
        void SendPacket(PacketUDP *packet);

        /// @brief A method to receieve a packet through a socket
        /// @param packet A pointer to a packet to receive data into
        /// @return A boolean value representing whether a packet has been receieved
        bool ReceivePacket(PacketUDP *packet);

        /// @brief A destructor to clean up resources from the socket
        ~SocketUDP();

    private:
        /// @brief A private constructor creating a new UDP socket from an SDL_net UDP socket
        /// @param socket The socket to transmit data from
        SocketUDP(UDPsocket socket);

    private:
        /// @brief A private member containing an SDL_net UDP socket
        UDPsocket m_Socket;
    };

}

#endif