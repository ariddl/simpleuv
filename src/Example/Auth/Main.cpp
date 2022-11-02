#include <uvw.hpp>
#include "NetManager.h"
#include "AuthServer.h"

int main()
{
    AuthServer::Ptr server = std::make_shared<AuthServer>();
    
    NetManager::Loop loop = uvw::Loop::getDefault();
    NetManager::Ptr manager = NetManager::Create(loop, server, 0x1000);

    server->Init(manager);
    loop->run();
    return 0;
}