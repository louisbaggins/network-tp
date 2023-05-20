#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <iostream>


const char *PORT = "51511";
const int MAX_MSG_SIZE = 512;

void error(const char *msg){
   perror(msg);
   exit(1);
}

int main(){
   int socketfd = socket(AF_INET, SOCK_STREAM, 0);
   if (socketfd < 0){
      perror("Error on creating socket");
      return 1;
   }

   struct sockaddr_in server_addr;
   server_addr.sin_family = AF_INET;
   server_addr.sin_port = htons(51511);
   server_addr.sin_addr.s_addr = INADDR_ANY;
   

   if(bind(
      socketfd, 
      (struct sockaddr*) &server_addr, 
      sizeof(server_addr)
      ) < 0){
       perror("Error on bind");
        return 1;
   }
   std::cout << "binded!" << std::endl;
   std::cout<<listen(socketfd, 1) << std::endl;
   if (listen(socketfd, 1) < 0){
      perror("Erro ao escutar conexões");
      return 1;
   }

   int newsockfd = accept(socketfd, NULL, NULL);
   std::cout << newsockfd << std::endl;
   if (newsockfd < 0) {
        perror("Erro ao aceitar conexão");
        return 1;
   }

    std::cout << "Conexão aceita!" << std::endl;
    char buffer[MAX_MSG_SIZE];

    while(true){
      int bytes_received = recv(newsockfd, buffer, sizeof(buffer), 0);
      if(bytes_received > 0){
         send(newsockfd, "ack", sizeof("ack"), 0);
      }  
      std::cout << "Dados recebidos do cliente: " << buffer << std::endl;

    };
    close(newsockfd);
    close(socketfd);
    return 0;
}
