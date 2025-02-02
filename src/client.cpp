#include "Client.hpp"
#include <iostream>

int main (int argc, char ** argv) 
{
    if (enet_initialize() != 0)
    {
        std::cerr << "An error occurred while initializing ENet." << std::endl;
        return EXIT_FAILURE;
    }
    atexit (enet_deinitialize);
}

Client::Client() : enetClient(nullptr)
{
    enetClient = enet_host_create (NULL /* create a client host */,
                1 /* only allow 1 outgoing connection */,
                2 /* allow up 2 channels to be used, 0 and 1 */,
                0 /* assume any amount of incoming bandwidth */,
                0 /* assume any amount of outgoing bandwidth */);
    
    if (enetClient == NULL)
    {
        std::cerr << "An error occurred while trying to create an ENet client host." << std::endl;
        exit(EXIT_FAILURE);
    }
}

Client::~Client()
{
    enet_host_destroy(enetClient);
}

void Client::ConnectToServer(const char* address)
{
    ENetAddress enetAddress;
    ENetEvent enetEvent;
    
    /* Connect to some.server.net:1234. */
    enet_address_set_host (&enetAddress, address);
    enetAddress.port = 1234;
    
    /* Initiate the connection, allocating the two channels 0 and 1. */
    enetServer = enet_host_connect (enetClient, &enetAddress, 2, 0);    
    
    if (enetServer == NULL)
    {
        fprintf (stderr, "No available peers for initiating an ENet connection.\n");
        exit (EXIT_FAILURE);
    }
    
    /* Wait up to 5 seconds for the connection attempt to succeed. */
    if (enet_host_service (enetClient, &enetEvent, 5000) > 0 &&
        enetEvent.type == ENET_EVENT_TYPE_CONNECT)
    {
        puts ("Connection to some.server.net:1234 succeeded.");
    }
    else
    {
        /* Either the 5 seconds are up or a disconnect event was */
        /* received. Reset the peer in the event the 5 seconds   */
        /* had run out without any significant event.            */
        enet_peer_reset(enetServer);
        puts ("Connection to some.server.net:1234 failed.");
    }
}

void Client::ProcessEvents()
{
    ENetEvent enetEvent;
    /* Wait up to 1000 milliseconds for an event. */
    while (enet_host_service (enetClient, &enetEvent, 1000) > 0)
    {
        switch (enetEvent.type)
        {
        case ENET_EVENT_TYPE_RECEIVE:
            printf ("A packet of length %u containing %s was received from %s on channel %u.\n",
                    enetEvent.packet -> dataLength,
                    enetEvent.packet -> data,
                    enetEvent.peer -> data,
                    enetEvent.channelID);
    
            /* Clean up the packet now that we're done using it. */
            enet_packet_destroy (enetEvent.packet);
            break;
        
        case ENET_EVENT_TYPE_DISCONNECT:
            printf ("%s disconnected.\n", enetEvent.peer -> data);
            /* Reset the peer's client information. */
            enetEvent.peer -> data = NULL;
        }
    }
}


void Client::SendMessage(const uint8_t* data, const int length)
{
    /* Create a reliable packet of size 7 containing "packet\0" */
    ENetPacket* enetPacket = enet_packet_create ((const void*) data, length, ENET_PACKET_FLAG_RELIABLE);
    
    /* Send the packet to the peer over channel id 0. */
    /* One could also broadcast the packet by         */
    /* enet_host_broadcast (host, 0, packet);         */
    enet_peer_send (enetServer, 0, enetPacket);

    /* One could just use enet_host_service() instead. */
    enet_host_flush(enetClient);
}