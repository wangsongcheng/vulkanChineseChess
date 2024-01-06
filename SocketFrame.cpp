#include "SocketFrame.h"
void Recv(int __fd, void *__buf, size_t __n, int __flags){
    int recvSize, offset = 0;
    if(__fd != INVALID_SOCKET){
        do{
            recvSize = recv(__fd, (char *)__buf + offset, __n - offset, __flags);
            if(recvSize == -1){
                perror("recv error");
                break;
            }
            // else {
            //     printf("success recv message %d byte, residue:%d, offset = %d, __n = %d\n", recvSize, __n - offset - recvSize, offset + recvSize, __n);
            // }
            offset += recvSize;
        } while (offset < __n);
    }
}
void Send(int __fd, const void *__buf, size_t __n, int __flags){
    int sendSize, offset = 0;
    if(__fd != INVALID_SOCKET){
        do{
            sendSize = send(__fd, (const char *)__buf + offset, __n - offset, __flags);
            if(sendSize == -1){
                perror("send error");
                break;
            }
            // else {
            //     printf("success send message %d byte, residue:%d, offset = %d, __n = %d\n", sendSize, __n - offset - sendSize, offset + sendSize, __n);
            // }
            offset += sendSize;
        } while (offset < __n);
    }
}