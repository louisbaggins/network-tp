#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <iostream>
#include <fstream>

using namespace std;

const char *PORT = "51511";
const int MAX_MSG_SIZE = 512;

void error(const char *msg){
   perror(msg);
   exit(1);
}

struct timeval timeout;

int hasFile(string file_name){
   ifstream file(file_name);
         if (file.good()){
            return 1;
         }
   return 0;
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
      if(buffer == "exit"){
         std::cout << "connection closed" << std::endl;
         break;
      }
      int bytes_received = recv(newsockfd, buffer, sizeof(buffer), 0);
      if (bytes_received > 0){
         cout << "Recebido " << buffer << endl;
      }
      string directory = "./server/";
      string filename(buffer);
      string path = directory + filename;

      string file_status;
      if(hasFile(path) == 0){
         file_status = "received";
      }
      else{
         file_status = "overwritten";
      }
      cout << file_status << endl;
      ofstream file;
      file.open(path.c_str());
      if (!file.is_open()) {
         perror("Erro ao criar o arquivo no servidor");
         return 1;
      } 
      timeout.tv_sec = 5;
      timeout.tv_usec = 0;
      setsockopt(newsockfd, SOL_SOCKET, SO_RCVTIMEO, (const char*)&timeout, sizeof(timeout));
      cout << "Aqui" << endl;
      do {
         bytes_received = recv(newsockfd, buffer, sizeof(buffer), 0);
         if (bytes_received > 0) {
            file.write(buffer, bytes_received);
         }
      } while (bytes_received > 0);
      timeout.tv_sec = 5;
      setsockopt(newsockfd, SOL_SOCKET, SO_RCVTIMEO, (const char*)&timeout, sizeof(timeout));

      cout << "veio" << endl;
      string file_received = "file " + filename + " " + file_status;
      send(newsockfd, file_received.c_str(), sizeof(file_received), 0);
      cout << file_received << endl;
    };
    close(newsockfd);
    close(socketfd);
    return 0;
}

