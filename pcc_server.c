#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <signal.h>

int connfd = -1;
int waitingForClients = 1;
uint32_t pcc_total[95]; /// what is the size? !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!1
void printfunc(){
    int i;
    for(i=0; i<95; i++){
        printf("char '%c' : %u times\n",(char) (32+i), pcc_total[i]);
    }
}

void signalHandler(){
    if(connfd == -1){
        printfunc();
        exit(0);
    }
    waitingForClients = 0;
}

int main(int argc, char *argv[]){
    char* nBuffer;
    char* cBuffer;
    uint32_t n;
    int pastBytes, currBytes, futureBytes;
    int messageLen, i;
    struct sockaddr_in serv_addr;
    int listenfd = -1;
    int option_value = -1;
    int connfd = -1;
    uint32_t pcc_curr[95];
    char messageBuffer[1000000];
    uint32_t c = 0;
    struct sigaction sigintAction = {
            .sa_handler = signalHandler,
            .sa_flags = SA_RESTART};
    

    //command line arguments number validation 
    if (argc != 2){
        perror("Wrong number of command line arguments");
        exit(1);
    }
    // ppc_total initialize
    memset(pcc_total, 0, sizeof(pcc_total));
    // Handling of SIGINT:
    if (sigaction(SIGINT, &sigintAction, NULL) == -1) {
        perror("Signal handle failed");
        exit(1);
    }
    // create the TCP connection //////////////////////////(socket + listen + bind):
    if ((listenfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("failed to creat socket");
        exit(1);
    }
    if (setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, &option_value, sizeof(serv_addr)) < 0) {
        perror("failed to set up the socket");
        exit(1);
    }
    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    serv_addr.sin_port = htons(atoi(argv[1]));

    if (bind(listenfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) != 0) {
        perror("Bind Failed");
        exit(1);
    }
    if (listen(listenfd, 10) != 0) {
        perror("Listen Failed");
        exit(1);
    }
    while(waitingForClients){
        //accept TPC connection
        connfd = accept(listenfd, NULL, NULL);
        if(connfd<0){
            perror("failed to accept");
            exit(1);
        }
        // getting N from client
        futureBytes = 4;
        pastBytes = 0;
        while(futureBytes > 0){
            nBuffer = (char*)&n + pastBytes;
            currBytes = read(connfd, nBuffer, futureBytes);
            if(currBytes <= 0){
                perror("failed to get N from client ");
                close(connfd);
                if((errno != ETIMEDOUT && errno != ECONNRESET && errno != EPIPE) && currBytes < 0){
                    exit(1);
                }
                futureBytes = 0;
                connfd = -1;
            }
            else{
                pastBytes += currBytes;
                futureBytes -= currBytes;
            } 
        }
        if(connfd == -1){
            continue;
        }
        n = ntohl(n);

        //getting n long message and counting printable chars
        memset(pcc_total, 0, sizeof(pcc_total));
        pastBytes = 0;
        futureBytes = n;
        while(futureBytes > 0){
            if(futureBytes> sizeof(messageBuffer)){
                messageLen = sizeof(messageBuffer);
            }
            else{
                messageLen = futureBytes;
            }
            currBytes = read(connfd, (char *) &messageBuffer, messageLen);
            if(currBytes<=0){
                perror("failed to get message from client");
                close(connfd);
                if((errno != ETIMEDOUT && errno != ECONNRESET && errno != EPIPE) && currBytes < 0){
                    exit(1);
                }
                futureBytes = 0;
                connfd = -1;
            }
            else{
                for(i=0; i<currBytes; i++){
                    if(messageBuffer[i] >= 32 && messageBuffer[i] <= 126){
                        pcc_curr[(int)(messageBuffer[i])-32]++;
                        c++;
                    }
                }
                pastBytes += currBytes;
                futureBytes -= currBytes;
            }
        }
        if(connfd == -1){
            continue;
        }
        c = htonl(c);
        pastBytes = 0;
        futureBytes = 4;
        while(futureBytes > 0){
            cBuffer = (char*)&c + pastBytes;          
            currBytes = write(connfd, cBuffer, futureBytes);
            if(currBytes<=0){
                perror("failed to send c to client");
                close(connfd);
                if((errno != ETIMEDOUT && errno != ECONNRESET && errno != EPIPE) && currBytes < 0){
                    exit(1);
                }
                futureBytes = 0;
                connfd = -1;
            }
            else{
                pastBytes += currBytes;
                futureBytes -= currBytes;
            }   
        }
        if (connfd == -1){
            continue;
        }
        for(i=0; i<95; i++){
            pcc_total[i]+= pcc_curr[i];
            pcc_curr[i] = 0;
        }
        close(connfd);
        connfd = -1;
        c = 0;
    }
    printfunc();
    exit(0);
}
