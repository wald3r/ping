#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <sys/time.h>
#include <netdb.h>
#include <string.h>
#include <netinet/in.h>
#include <assert.h>

#define SERVER_PORT "1234"


int main(int argc, char **argv){

  
    struct addrinfo before, *after;
    struct timeval tv1, tv2;
    long rtt_sec, rtt_usec;
    int client_socket_fd, send_err, rcv_err, close_err, time_err, print_err, status;
    uint32_t value, network_byte_order;
    socklen_t flen;

    if(argc != 2){
        print_err = printf("Usage: ./pingclient1 <input_host_name_with_no_space_inside_it> /n/n");
        if(print_err < 0){
            perror("Couldn't print the message: Usage....\n");
            exit(EXIT_FAILURE);
        }
		return EXIT_FAILURE;
    }
      
    //Set the options to get the ipv6 address
    memset(&before, 0, sizeof(struct addrinfo));
    before.ai_family = AF_INET6;
    before.ai_socktype = SOCK_DGRAM;
    before.ai_protocol = IPPROTO_UDP;

    //get ipv6 address
    status = getaddrinfo(argv[1], SERVER_PORT, &before, &after);
    if(status != 0){
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(status));
        exit(EXIT_FAILURE);

    }

    flen = sizeof(struct sockaddr_in6);



    client_socket_fd = socket(PF_INET6, SOCK_DGRAM, IPPROTO_UDP);
    if(client_socket_fd == -1){
        perror("Error bind client_socket\n");
        exit(EXIT_FAILURE);
    }


    //get time to calculate rtt 
    time_err = gettimeofday(&tv1, NULL);
    if(time_err == -1){
        perror("Error setting time!\n");
        exit(EXIT_FAILURE);
    }


    value = 1;
    //network byte order
    network_byte_order = htonl(value);

    send_err = sendto(client_socket_fd, &network_byte_order, sizeof(uint32_t), 0, (struct sockaddr*) after->ai_addr, flen);
    if(send_err == -1){
        perror("Error occured while sending a message. \n");

    }

    rcv_err = recvfrom(client_socket_fd, &network_byte_order, sizeof(uint32_t), 0, (struct sockaddr*) after->ai_addr, &flen);
    if(rcv_err == -1){
       perror("Error occured while receiving a message. \n");
    }
    
    //network byte order
    value = ntohl(network_byte_order);
    assert(value == 2);
    
    //get time to calculate rtt
    time_err = gettimeofday(&tv2, NULL);
    if(time_err == -1){
        perror("Error setting time! \n");
        exit(EXIT_FAILURE);
    }
   

    //calculate rtt
    rtt_usec = tv2.tv_usec - tv1.tv_usec;
    rtt_sec = tv2.tv_sec - tv1.tv_sec;

    print_err = printf("The RTT was: %ld.%ld seconds.\n", rtt_sec, rtt_usec);
    if(print_err < 0){
        perror("Couldn't print the message: The RTT was....\n");
        exit(EXIT_FAILURE);
    }
       


    close_err = close(client_socket_fd);
    if(close_err == -1){
        perror("Error occured while closing the socket.\n");
        exit(EXIT_FAILURE);
    }

    return EXIT_SUCCESS;

}
