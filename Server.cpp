#include "Server.h"
Server::Server(/* args */){
}

Server::~Server(){
}
bool Server::CreateServer(int listenCount){
    mSocket = socket(AF_INET, SOCK_STREAM, 0);
    if(mSocket == -1){
        perror("create server socket error");
        return false;
    }
#ifdef __linux
    int32_t opt = 1;
    //SO_REUSEADDR被设置后, 重开程序后bing不会出现Address already in use错误
    setsockopt(mSocket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
#endif
    struct sockaddr_in my_addr;
    my_addr.sin_family = AF_INET; /* host byte order */
    my_addr.sin_addr.s_addr = INADDR_ANY; /* auto-fill with my IP */
    my_addr.sin_port = htons(INTERNET_PORT); /* short, network byte order */
#ifdef WIN32
    ZeroMemory(&my_addr.sin_zero, sizeof(my_addr.sin_zero)); /* zero the rest of the struct */
#else
    bzero(&my_addr.sin_zero, sizeof(my_addr.sin_zero)); /* zero the rest of the struct */
#endif
    if(bind(mSocket, (struct sockaddr *)&my_addr, sizeof(struct sockaddr)) == -1){
        perror("bind error");
        ShutdownServer();
        return false;
    }
    mClients.resize(listenCount);
    if (listen(mSocket, listenCount) == -1){
        perror("listen error");
        ShutdownServer();
        // herror("listen error");
        return false;
    }
    return true;
}
SOCKET Server::AcceptClient(uint32_t client){
    struct sockaddr_in client_addr;
    socklen_t size = sizeof(client_addr);
    SOCKET s = accept(mSocket, (struct sockaddr *)&client_addr, &size);
    mClients[client].SetScoket(s);
    return s;
}
void Server::SendToAllClient(const void *__buf, size_t __n)const{
    for (auto it = mClients.begin(); it != mClients.end(); ++it){
        it->SendTo(__buf, __n);
    }
}
void Server::RecvFromClient(uint32_t client, void *__buf, size_t __n)const{
    mClients[client].RecvFrom(__buf, __n);
}

void Server::SendToClient(uint32_t client, const void *__buf, size_t __n)const{
    mClients[client].SendTo(__buf, __n);
}