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
#include <errno.h>
#include <fcntl.h>

int connfd = -1;;
int waitingForClients = 1;
uint32_t pcc_total[95]; /// what is the size? !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!1


void signalHandler(){
    if(connfd == 0){
        print
    }
}

int main(int argc, char *argv[]){
    uint16_t serverPort;
    int listenfd = -1;
    int option_value = -1;
    int connfd = -1;
    uint32_t pcc_curr[95];
    struct sockaddr_in serv_addr;
    struct sigaction sigintAction = {
            .sa_handler = &signalHandler,
            .sa_flags = SA_RESTART};
    struct sockaddr_in serv_addr;
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
    if (setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, &option_value, sizeof(int)) < 0) {
        perror("failed to set up the socket");
        exit(1);
    }
    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    serv_addr.sin_port = htons(serverPort);

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

    }
}