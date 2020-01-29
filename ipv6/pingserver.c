#include <stdio.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <time.h>
#include <unistd.h>
#include <netinet/in.h>

#define SERVER_PORT 1234

int main(void){

    struct sockaddr_in6 srv_addr, cln_addr;
    int socket_server_fd, bind_err, rcv_err, send_err, close_err, random_number;
    socklen_t flen;
    uint32_t rcv_packages, rcv_byte_order;

    flen = sizeof(struct sockaddr_in6);    

    //Server information
    srv_addr.sin6_flowinfo = 0;
    srv_addr.sin6_family = AF_INET6;
    srv_addr.sin6_port = htons(SERVER_PORT);
    srv_addr.sin6_addr = in6addr_any;


    socket_server_fd = socket(PF_INET6, SOCK_DGRAM, IPPROTO_UDP);
    if(socket_server_fd == -1){
        perror("Error creating socket_server.\n");
        exit(EXIT_FAILURE);
    }
    
    
    bind_err = bind(socket_server_fd, (struct sockaddr *) &srv_addr, sizeof(struct sockaddr_in6));
    if(bind_err == -1){
        perror("Error binding socket. \n");
        exit(EXIT_FAILURE);
    }
    
    srand(time(NULL));   

    //receive and send multiple messages to the server
    for(;;){
        
        rcv_err = recvfrom(socket_server_fd, &rcv_byte_order, sizeof(uint32_t), 0, (struct sockaddr*) &cln_addr, &flen);
        if(rcv_err == -1){
            perror("Error occured while receiving a message. \n");
        }
    
        //network byte order
        rcv_packages = ntohl(rcv_byte_order);

        //wait a random time
        //Switch between the values 2 and 3 to check if pingclient2 and pingclient3 work properly.
        random_number = rand() % 3; 
        sleep(random_number);
        rcv_packages++;

        //network byte order        
        rcv_byte_order = htonl(rcv_packages);
        
        send_err = sendto(socket_server_fd, &rcv_byte_order, sizeof(uint32_t), 0, (struct sockaddr*) &cln_addr, flen);
        if(send_err == -1){
            perror("Error occured while sending a message. \n");

        }
    }

    close_err = close(socket_server_fd);
    if(close_err == -1){
        perror("Error occured during closing. \n");
        exit(EXIT_FAILURE);
    }
    return EXIT_SUCCESS;

}



