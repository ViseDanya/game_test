#include "Message.h"
#include <enet/enet.h>

class Client
{
public:
    Client();
    ~Client();
    
    void ConnectToServer(const char* address);
    void SendMessage(const uint8_t* data, const int length);
    void ProcessEvents();
private:
    ENetHost* enetClient;
    ENetPeer* enetServer;
};