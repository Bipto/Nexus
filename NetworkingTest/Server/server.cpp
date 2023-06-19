#include <iostream>

#include "Core/Networking/Socket.h"

#include "SDL_net.h"
#include <cstring>

int main(int argc, char **argv)
{
    if (SDLNet_Init() == -1)
    {
        std::cout << SDLNet_GetError() << std::endl;
        return -1;
    }

    const char *text = "HELLO CLIENT!\n";

    Nexus::SocketTCP *server = Nexus::SocketTCP::CreateServer(9999);

    if (server)
    {
        while (true)
        {
            Nexus::SocketTCP *client = server->AcceptConnection();
            if (client)
            {
                client->Send(text, strlen(text) + 1);
                delete client;
            }
        }
    }

    delete server;
    SDLNet_Quit();

    return 0;
}