#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <sys/time.h>
#include <netdb.h>
#include <netinet/in.h>
#include <assert.h>

#define SERVER_PORT 1234
#define TIMEOUT 2


int main(int argc, char **argv){

    struct hostent *hostnm;
    struct in_addr *ipaddr;
    struct sockaddr_in srv_addr;
    struct timeval tv1, tv2, timeout;
    long rtt_sec, rtt_usec;
    int client_socket_fd, send_err, rcv_err, close_err, nmbrs_fd, time_err, print_err;
    fd_set  read_set;
    socklen_t flen;
    uint32_t value, network_byte_order;

    
    if(argc != 2){
        print_err = printf("Usage: ./pingclient2 <input_host_name_with_no_space_inside_it> /n/n");
        if(print_err < 0){
            perror("Couldn't print the message: Usage...\n");
            exit(EXIT_FAILURE);
        }
        return EXIT_FAILURE;
    }

    
    //get ipv4 address
    hostnm = gethostbyname(argv[1]);
    if(hostnm == NULL){
        perror("Error getting hostname!\n");
        exit(EXIT_FAILURE);
    }
    
    //server information
    ipaddr = (struct in_addr*) hostnm->h_addr_list[0];
    srv_addr.sin_family = AF_INET;
    srv_addr.sin_port = htons(SERVER_PORT);
    srv_addr.sin_addr.s_addr = inet_addr(inet_ntoa(*ipaddr));

    flen = sizeof(struct sockaddr_in);


    client_socket_fd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if(client_socket_fd == -1){
        perror("Error bind client_socket\n");
        exit(1);
    }

    //create set of descriptors
    FD_ZERO(&read_set);
    FD_SET(client_socket_fd, &read_set);

    //set timeout
    timeout.tv_sec = 1;
    timeout.tv_usec = 0;
    
   
    //set time to calculate rtt
    time_err = gettimeofday(&tv1, NULL);
    if(time_err == -1){
        perror("Error setting time! \n");
        exit(EXIT_FAILURE);
    }
    
     value = 1;
    //network ordering
    network_byte_order = htonl(value);


    send_err = sendto(client_socket_fd, &network_byte_order, sizeof(uint32_t), 0, (struct sockaddr*) &srv_addr, flen);
    if(send_err == -1){
        perror("Error occured while sending a message. \n");
        exit(EXIT_FAILURE);
    }
    
   //wait for message
    nmbrs_fd = select(client_socket_fd+1, &read_set, NULL, NULL, &timeout);
    if(nmbrs_fd < 0){
        perror("Error occured while selecting.\n");
        exit(EXIT_FAILURE);

    }
    if(nmbrs_fd == 0){
        print_err = printf("The packet was lost.\n");
        if(print_err < 0){
            perror("Couldn't print the message: The packet was lost.\n");
            exit(EXIT_FAILURE);
        }
        
    }

    if(FD_ISSET(client_socket_fd, &read_set)){        
        rcv_err = recvfrom(client_socket_fd, &network_byte_order, sizeof(uint32_t), 0, (struct sockaddr*) &srv_addr, &flen);
        if(rcv_err == -1){
            perror("Error occured while receiving a message. \n");
            exit(EXIT_FAILURE);
        }

        value = ntohl(network_byte_order);
        assert(value == 2);

        //set time to calculate rtt
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
        

    }


    close_err = close(client_socket_fd);
    if(close_err == -1){
        perror("Error occured while closing the socket.\n");
        exit(EXIT_FAILURE);
    }

    return EXIT_SUCCESS;

}
