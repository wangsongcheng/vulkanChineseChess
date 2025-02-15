#include "Client.h"
Client::Client(/* args */){
    mSocket = INVALID_SOCKET;
}

Client::~Client(){
}

bool Client::CreateClient(const char *serverIp){
    struct in_addr address;
    struct sockaddr_in addr;
#ifdef WIN32
    if (1 != inet_pton(AF_INET, serverIp, &address)) {
        perror("inet_aton error");
        return false;
    }
#else
    if(!inet_aton(serverIp, &address)){
        perror("inet_aton error");
        return false;
    }
#endif
    addr.sin_addr = address;
    mSocket = socket(AF_INET, SOCK_STREAM, 0);
    if(mSocket == INVALID_SOCKET){
        perror("create client socket error");
        return false;
    }
    addr.sin_family = AF_INET;
    addr.sin_port = htons(INTERNET_PORT);
#ifdef WIN32
    ZeroMemory(&addr.sin_zero, sizeof(addr.sin_zero)); /* zero the rest of the struct */
#else
    bzero(&addr.sin_zero, sizeof(addr.sin_zero)); /* zero the rest of the struct */
#endif // WIN32
    if(connect(mSocket,(struct sockaddr *)&addr,sizeof(struct sockaddr)) == INVALID_SOCKET){
        perror("connect error");
        shutdown(mSocket, SHUT_RDWR);
        return false;
    }
    return true;
}
void Client::RecvFrom(void *__buf, size_t __n)const{
    if(mSocket != INVALID_SOCKET)
        Recv(mSocket, __buf, __n, 0);
}
void Client::SendTo(const void *__buf, size_t __n)const{
    if(mSocket != INVALID_SOCKET)
        Send(mSocket, __buf, __n, 0);
}