#include <stdio.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <time.h>
#include <unistd.h>
#include <netinet/in.h>

#define SERVER_PORT 1234

int main(void){

    struct sockaddr_in srv_addr, cln_addr;
    int socket_server_fd, bind_err, rcv_err, send_err, close_err, random_number;
    socklen_t flen;
    uint32_t rcv_packages, rcv_network_order;

    flen = sizeof(struct sockaddr_in);    

    //Server information
    srv_addr.sin_family = AF_INET;
    srv_addr.sin_port = htons(SERVER_PORT);
    srv_addr.sin_addr.s_addr = htonl(INADDR_ANY);


    socket_server_fd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if(socket_server_fd == -1){
        perror("Error creating socket_server.\n");
        exit(EXIT_FAILURE);
    }
    
    
    bind_err = bind(socket_server_fd, (struct sockaddr *) &srv_addr, sizeof(struct sockaddr_in));
    if(bind_err == -1){
        perror("Error binding socket. \n");
        exit(EXIT_FAILURE);
    }
    
    srand(time(NULL));   

    for(;;){

        rcv_err = recvfrom(socket_server_fd, &rcv_network_order, sizeof(uint32_t), 0, (struct sockaddr*) &cln_addr, &flen);
        if(rcv_err == -1){
            perror("Error occured while receiving a message. \n");
            exit(EXIT_FAILURE);
        }
        //network ordering
        rcv_packages = ntohl(rcv_network_order);
        

        //wait random time
        //switch between the values 2 and 3 to check if pingclient2 and pingclient3 work properly
        random_number = rand() % 3; 
        sleep(random_number);
        rcv_packages++;

        //network ordering
        rcv_network_order = htonl(rcv_packages);

        send_err = sendto(socket_server_fd, &rcv_network_order, sizeof(uint32_t), 0, (struct sockaddr*) &cln_addr, flen);
        if(send_err == -1){
            perror("Error occured while sending a message. \n");
            exit(EXIT_FAILURE);
        }
    }

    close_err = close(socket_server_fd);
    if(close_err == -1){
        perror("Error occured during closing. \n");
    }

    return EXIT_SUCCESS;

}



