#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <sys/time.h>
#include <netdb.h>
#include <netinet/in.h>

#define SERVER_PORT 1234
#define TIMEOUT 1


int main(int argc, char **argv){

    struct hostent *hostnm;
    struct in_addr *ipaddr;
    struct sockaddr_in srv_addr;
    struct timeval tv1, tv2, timeout;
    long rtt_sec, rtt_usec;
    int client_socket_fd, send_err, rcv_err, close_err, nmbrs_fd, time_err, print_err;
    fd_set  read_set;
    socklen_t flen;
    uint32_t send_packages, rcv_packages, rcv_network_order, send_network_order;

    if(argc != 2){
        print_err = printf("Usage: ./pingclient3 <input_host_name_with_no_space_inside_it> /n/n");
        if(print_err < 0){
            perror("Couldn't print the message: Usage....\n");
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

    send_packages = 0;
    rcv_packages = 0;


    flen = sizeof(struct sockaddr_in);


    client_socket_fd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if(client_socket_fd == -1){
        perror("Error bind client_socket\n");
        exit(EXIT_FAILURE);
    }

    for(;;){
        
        //create set of descriptors
        FD_ZERO(&read_set);
        FD_SET(client_socket_fd, &read_set);    

        //set time to calculate rtt
        time_err = gettimeofday(&tv1, NULL);
        if(time_err == -1){
            perror("Error setting time! \n");
            exit(EXIT_FAILURE);
        }
        
        //network order
        send_network_order = htonl(send_packages);    

        send_err = sendto(client_socket_fd, &send_network_order, sizeof(uint32_t), 0, (struct sockaddr*) &srv_addr, flen);
        if(send_err == -1){
            perror("Error occured while sending a message. \n");
            exit(EXIT_FAILURE);

        }else{
            send_packages++;
        }

        //set timeout
        timeout.tv_sec = TIMEOUT;

        //wait for incoming message
        nmbrs_fd = select(client_socket_fd+1, &read_set, NULL, NULL, &timeout);

        if(nmbrs_fd < 0){
            perror("Error occured while selecting.\n");
            exit(EXIT_FAILURE);
        }
        else if(FD_ISSET(client_socket_fd, &read_set)){        
           
            rcv_err = recvfrom(client_socket_fd, &rcv_network_order, sizeof(uint32_t), 0, (struct sockaddr*) &srv_addr, &flen);
            if(rcv_err == -1){
                perror("Error occured while receiving a message. \n");
                exit(EXIT_FAILURE);
            }
            
            //network order
            rcv_packages = ntohl(rcv_network_order);
  

            //set time to calculate rtt
            time_err = gettimeofday(&tv2, NULL);
            if(time_err == -1){
                perror("Error setting time! \n");
                exit(EXIT_FAILURE);
            }
            
            //calculate rtt
            rtt_usec = tv2.tv_usec - tv1.tv_usec;
            rtt_sec = tv2.tv_sec - tv1.tv_sec;


            if(send_packages != rcv_packages){
                print_err = printf("Packet %u: wrong counter! Received %u instead of %u.\n", send_packages, rcv_packages, send_packages);
                if(print_err < 0){
                    perror("Couldn't print the message: ...wrong counter!....\n");
                    exit(EXIT_FAILURE);
                }
            }else{
                print_err = printf("Packet %u: %ld.%ld seconds.\n", send_packages, rtt_sec, rtt_usec);
                if(print_err < 0){
                    perror("Couldn't print the message: Packet....\n");
                    exit(EXIT_FAILURE);
                }
            }
        }
        else{
            print_err = printf("The packet was lost.\n");
            if(print_err < 0){
                perror("Couldn't print the message: The packet was lost...\n");
                exit(EXIT_FAILURE);
            }    
        }

    }


    close_err = close(client_socket_fd);
    if(close_err == -1){
        perror("Error occured while closing the socket.\n");
        exit(EXIT_FAILURE);
    }

    return EXIT_SUCCESS;

}
