#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include "tftp.h"
#include "tftp_client.h"


int main() {
    char command[256];
    tftp_client_t client;
    memset(&client, 0, sizeof(client));  // Initialize client structure

    char ip[INET_ADDRSTRLEN];
    int port=PORT;
    printf("connect ");
    scanf("%15s",ip);
    connect_to_server(&client,ip,port);
   


    // Main loop for command-line interface
    while (1) {
        printf("tftp> ");
        fgets(command, sizeof(command), stdin);

        // Remove newline character
        command[strcspn(command, "\n")] = 0;

        // Process the command
        process_command(&client, command);
    }

    return 0;
}

// Function to process commands
void process_command(tftp_client_t *client, char *command) {
   
}

// This function is to initialize socket with given server IP, no packets sent to server in this function
void connect_to_server(tftp_client_t *client, char *ip, int port) {
    // Create UDP socket
   client->sockfd=socket(AF_INET,SOCK_DGRAM,0);
   if(client->sockfd<0){
    perror("socket");
    exit(EXIT_FAILURE);

   }
   memset(&client->server_addr,0,sizeof(client->server_addr));
   client->server_addr.sin_family=AF_INET;
   client->server_addr.sin_port=htons(port);
   if(inet_pton(AF_INET,ip,&client->server_addr.sin_addr)<=0){
    printf("Invalid IP address\n");
    close(client->sockfd);
    client->sockfd=-1;
    return;
   }
   strncpy(client->server_ip,ip,sizeof(client->server_ip)-1);
   client->server_ip[sizeof(client->server_ip)-1]='\0';
   client->server_len=sizeof(client->server_addr);
   struct timeval tv={TIMEOUT_SEC,0};
   setsockopt(client->sockfd,SOL_SOCKET,SO_RCVTIMEO,&tv,sizeof(tv));
   printf("connected to the server %s : %d successfully\n",client->server_ip,port);

  

    // Set socket timeout option


    // Set up server address

}

void put_file(tftp_client_t *client, char *filename) {
    // Send WRQ request and send file
    FILE *fp=fopen(filename,"rb");
    if(!fp){
        printf("File not found %s\n",filename);
        return;
    }
    fclose(fp);
    //BUILD WRQ
    char buffer[BUFFER_SIZE];
    int len=0;
    buffer[len++]=0;
    buffer[len++]=WRQ;
    strcpy(&buffer[len],filename);
    len+=strlen(filename)+1;
    strcpy(&buffer[len],"octet");
    len+=strlen("octet")+1;
    int sent=sendto(client->sockfd,buffer,len,0,(struct sockaddr *)&client->server_addr,client->server_len);
    if(sent<0){
        perror("sendto");
        return;
    }
    printf("WRQ request sent successfully for file %s to the server\n ",filename);

}

void get_file(tftp_client_t *client, char *filename) {
    // Send RRQ and recive file 
  
}

void disconnect(tftp_client_t *client) {
    // close fd
   
}
void send_request(int sockfd, sockaddr_in server_addr, char *filename, int opcode)
{

}

void receive_request(int sockfd, sockaddr_in server_addr, char *filename, int opcode)
{
}