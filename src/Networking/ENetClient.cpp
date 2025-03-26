#include "ENetClient.h"
#include <iostream>

ENetClient::ENetClient() : client(nullptr)
{
    client = enet_host_create (NULL /* create a client host */,
                1 /* only allow 1 outgoing connection */,
                2 /* allow up 2 channels to be used, 0 and 1 */,
                0 /* assume any amount of incoming bandwidth */,
                0 /* assume any amount of outgoing bandwidth */);
    
    if (client == NULL)
    {
        std::cerr << "An error occurred while trying to create an ENet client host." << std::endl;
        exit(EXIT_FAILURE);
    }
}

ENetClient::~ENetClient()
{
    enet_host_destroy(client);
}

void ENetClient::connectToServer(const char* address)
{
    ENetAddress enetAddress;
    ENetEvent enetEvent;
    
    /* Connect to some.server.net:1234. */
    enet_address_set_host (&enetAddress, address);
    enetAddress.port = 1234;
    
    /* Initiate the connection, allocating the two channels 0 and 1. */
    server = enet_host_connect (client, &enetAddress, 2, 0);    
    
    if (server == nullptr)
    {
        std::cerr <<  "No available peers for initiating an ENet connection." << std::endl;
        exit (EXIT_FAILURE);
    }
    
    /* Wait up to 5 seconds for the connection attempt to succeed. */
    if (enet_host_service (client, &enetEvent, 5000) > 0 &&
        enetEvent.type == ENET_EVENT_TYPE_CONNECT)
    {
        std::cout << "Connection to some.server.net:1234 succeeded." << std::endl;
    }
    else
    {
        /* Either the 5 seconds are up or a disconnect event was */
        /* received. Reset the peer in the event the 5 seconds   */
        /* had run out without any significant event.            */
        enet_peer_reset(server);
        std::cerr << "Connection to some.server.net:1234 failed." << std::endl;
    }
}

void ENetClient::processEvents()
{
    ENetEvent enetEvent;
    while (enet_host_service (client, &enetEvent, 0) > 0)
    {
        switch (enetEvent.type)
        {
        case ENET_EVENT_TYPE_RECEIVE:
            // std::cout << "A packet of length " << enetEvent.packet->dataLength <<
            // // " containing " << enetEvent.packet->data <<
            // " was received from " << enetEvent.peer->data <<
            // " on channel " << enetEvent.channelID << std::endl;
    
            handleMessageReceived(enetEvent);

            /* Clean up the packet now that we're done using it. */
            enet_packet_destroy (enetEvent.packet);
            break;
        
        case ENET_EVENT_TYPE_DISCONNECT:
            std::cout << enetEvent.peer->data << " disconnected." << std::endl;
            /* Reset the peer's client information. */
            enetEvent.peer -> data = nullptr;

            handleServerDisconnected(enetEvent);
            break;
        
        default:
            std::cout << "No or invalid event." << std::endl;
        }
    }
}


void ENetClient::sendMessageToServer(const char* data, const int length)
{
    /* Create a reliable packet of size 7 containing "packet\0" */
    ENetPacket* enetPacket = enet_packet_create ((const void*) data, length, ENET_PACKET_FLAG_RELIABLE);
    
    /* Send the packet to the peer over channel id 0. */
    /* One could also broadcast the packet by         */
    /* enet_host_broadcast (host, 0, packet);         */
    enet_peer_send (server, 0, enetPacket);

    /* One could just use enet_host_service() instead. */
    enet_host_flush(client);
}