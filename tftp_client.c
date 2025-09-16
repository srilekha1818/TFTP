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
    if(strncmp(command,"connect",7)==0){
        char ip[INET_ADDRSTRLEN];
        if(sscanf(command,"connect %15s",ip)==1){
            connect_to_server(client,ip,PORT);

        }
        else{
            printf("Usage:connect <server_ip>\n");

        }
    }
    else if(strncmp(command,"put",3)==0){
        char filename[128];
        if(sscanf(command,"put %127s",filename)==1){
            put_file(client,filename);
        }
        else{
            printf("Usage:put <filename>\n");

        }
    }
    else if(strncmp(command,"get",3)==0){
        char filename[128];
          if(sscanf(command,"get %127s",filename)==1){
            get_file(client,filename);
        }
        else{
            printf("Usage:get <filename>\n");

        }
    }
    else if(strncmp(command,"mode",4)==0){
        //need to implement yet
    }
    else if(strcmp(command,"exit")==0){
        disconnect(client);
        exit(0);

    }
  else if(strcmp(command, "help") == 0) {
    printf("\n commands to be followed :\n");
    printf("  connect <server_ip>   - connect to TFTP server\n");
    printf("  put <filename>        - upload a file to server\n");
    printf("  get <filename>        - download a file from server\n");
    printf("  mode <octet/netascii> - set transfer mode\n");
    printf("  exit                  - disconnect and exit\n");
   
}

    else{
        printf("Unknown command.Type help for options.\n");
    }
   
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
     FILE *fp=fopen(filename,"rb");
    if(!fp){
        printf("File not found %s\n",filename);
        return;
    }
    fclose(fp);
    send_request(client->sockfd,client->server_addr,filename,WRQ);
    receive_request(client->sockfd,client->server_addr,filename,WRQ); 

}

void get_file(tftp_client_t *client, char *filename) {
    // Send RRQ and recive file 
    send_request(client->sockfd,client->server_addr,filename,RRQ);
    receive_request(client->sockfd,client->server_addr,filename,RRQ);
  
}

void disconnect(tftp_client_t *client) {
    // close fd
    if(client->sockfd>=0){
        close(client->sockfd);
        client->sockfd=-1;
    }
   
}
void send_request(int sockfd, struct  sockaddr_in server_addr, char *filename, int opcode)
{
  tftp_packet pkt;
  pkt.opcode=htons(opcode);
  strncpy(pkt.body.request.filename,filename,sizeof(pkt.body.request.filename));
  strcpy(pkt.body.request.mode,"octet");
int sent=sendto(sockfd,&pkt,sizeof(pkt),0,(struct sockaddr *)&server_addr,sizeof(server_addr));
 if(sent<0){
        perror("sendto");
        return;
    }
    printf("%s request sent successfully for file %s to the server\n ",opcode==WRQ?"WRQ":"RRQ",filename);
}

void receive_request(int sockfd, struct  sockaddr_in server_addr, char *filename, int opcode)
{
    tftp_packet pkt,ack;
    socklen_t addrlen=sizeof(server_addr);
    if(opcode==WRQ){
        FILE *fp=fopen(filename,"rb");
        if(!fp){
            printf("File open failed.\n");
            return;
        }
        unit16_t block=0;
        while(1){
            block++;
            size_t bytes_read=fread(pkt.body.data_packet.data,1,512,fp);
            pkt.opcode=htons(DATA);
            pkt.body.data_packet.block_numbr=htons(block);
            sendto(sockfd,&pkt,bytes_read+4,0,(struct sockaddr*)&server_addr,addrlen);
            int n=recvfrom(sockfd,&ack,sizeof(ack),0,(struict sockaddr*)&server_addr,&addrlen);
            if(n<0){
                perror("ACK timeout");
                break;
            }
            if(ntohs(ack.opcode)==ACK &&ntohs(ack.body.ack_packet.block_number)==block){
                if(bytes_read<512)break;
            }
        }
        fclose(fp);
        printf("File %s uploaded successfully\n",filename);
    }
    else if(opcode==RRQ){
        
    }


}