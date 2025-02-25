#include "ENetServer.h"
#include <enet/enet.h>
#include <iostream>

ENetServer::~ENetServer()
{
    enet_host_destroy(server);
}

void ENetServer::startServer()
{
    ENetAddress address;
 
    /* Bind the server to the default localhost.     */
    /* A specific host address can be specified by   */
    /* enet_address_set_host (& address, "x.x.x.x"); */
     
    address.host = ENET_HOST_ANY;
    /* Bind the server to port 1234. */
    address.port = 1234;
    server = enet_host_create (&address /* the address to bind the server host to */, 
        32      /* allow up to 32 clients and/or outgoing connections */,
         2      /* allow up to 2 channels to be used, 0 and 1 */,
         0      /* assume any amount of incoming bandwidth */,
         0      /* assume any amount of outgoing bandwidth */);

    if (server == nullptr)
    {
        std::cerr << "An error occurred while trying to create an ENet server host." << std::endl;
        exit (EXIT_FAILURE);
    }
}

void ENetServer::processEvents()
{
    ENetEvent enetEvent;
 
    while (enet_host_service (server, & enetEvent, 0) > 0)
    {
        switch (enetEvent.type)
        {
        case ENET_EVENT_TYPE_CONNECT:
            std::cout << "A new client connected from " << enetEvent.peer->address.host <<
            ":" << enetEvent.peer->address.port << std::endl;
     
            /* Store any relevant client information here. */
            // event.peer -> data = "Client information";
            handleClientConnected(enetEvent);
            break;
     
        case ENET_EVENT_TYPE_RECEIVE:
            std::cout << "A packet of length " << enetEvent.packet->dataLength <<
            " containing " << enetEvent.packet->data <<
            " was received from " << enetEvent.peer->data <<
            " on channel " << enetEvent.channelID << std::endl;
     
            handleMessageReceived(enetEvent);

            /* Clean up the packet now that we're done using it. */
            enet_packet_destroy (enetEvent.packet);
            
            break;
           
        case ENET_EVENT_TYPE_DISCONNECT:
            std::cout << "Client " << enetEvent.peer->data << " disconnected." << std::endl;
            
            handleClientDisconnected(enetEvent);

            /* Reset the peer's client information. */
            enetEvent.peer -> data = NULL;
    
        default:
            std::cout << "No or invalid event." << std::endl;
        }
    }
}

void ENetServer::broadcastMessageToClients(const uint8_t* data, const int length)
{
    ENetPacket* packet = enet_packet_create ((const void*) data, length, ENET_PACKET_FLAG_RELIABLE);
    enet_host_broadcast (server, 0, packet);
    enet_host_flush(server);
}