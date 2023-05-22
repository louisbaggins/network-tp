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

int connect(char* version, char* port){
   if(strcmp(version, "v4") == 0){
      int socketfd4 = socket(AF_INET, SOCK_STREAM, 0);
      if (socketfd4 < 0){
         perror("Error on creating socket");
         return 1;
      }
      struct sockaddr_in server_addr;
      server_addr.sin_family = AF_INET;
      server_addr.sin_port = htons(atoi(port));
      server_addr.sin_addr.s_addr = INADDR_ANY;
       if(bind(
      socketfd4, 
      (struct sockaddr*) &server_addr, 
      sizeof(server_addr)
      ) < 0){
       perror("Error on bind");
        return 1;
      }
      if (listen(socketfd4, 1) < 0){
         perror("Erro ao escutar conexões");
         return 1;
      }

      return socketfd4;
   }
   else{
      cout << atoi(port) << endl;
      struct sockaddr_in6 server_addr;
      server_addr.sin6_family = AF_INET6;
      server_addr.sin6_port = htons(atoi(port));
      server_addr.sin6_addr = in6addr_any;
      int socketfd = socket(AF_INET6, SOCK_STREAM, 0);
      if (socketfd < 0){
         perror("Error on creating socket");
      return 1;
      }
      if(bind(
      socketfd, 
      (struct sockaddr*) &server_addr, 
      sizeof(server_addr)
      ) < 0){
         perror("Error on bind");
         return 1;
      }
      if (listen(socketfd, 1) < 0){
         perror("Erro ao escutar conexões");
         return 1;
      }
      return socketfd;
      }
}


int main(int argc, char* argv[]){
   char* version = argv[1];
   char* port = argv[2];
   int socketfd = connect(version, port);
   int newsockfd = accept(socketfd, NULL, NULL);
   if (newsockfd < 0) {
      perror("Erro ao aceitar conexão");
      return 1;
   }
   cout << "Conexão aceita!" << endl;
   char buffer[MAX_MSG_SIZE];

    while(true){
      int bytes_received = recv(newsockfd, buffer, sizeof(buffer), 0);
      if(strcmp(buffer, "exit") == 0){
         cout << "connection closed" << endl;
         break;
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
      ofstream file;
      file.open(path.c_str());
      if (!file.is_open()) {
         perror("Erro ao criar o arquivo no servidor");
         return 1;
      } 
      timeout.tv_sec = 5;
      timeout.tv_usec = 0;
      setsockopt(newsockfd, SOL_SOCKET, SO_RCVTIMEO, (const char*)&timeout, sizeof(timeout));
      do {
         bytes_received = recv(newsockfd, buffer, sizeof(buffer), 0);
         if (bytes_received > 0) {
            file.write(buffer, bytes_received);
         }
      } while (bytes_received > 0);
      timeout.tv_sec = 0;
      setsockopt(newsockfd, SOL_SOCKET, SO_RCVTIMEO, (const char*)&timeout, sizeof(timeout));

      string file_received = "file " + filename + " " + file_status;
      send(newsockfd, file_received.c_str(), sizeof(file_received), 0);
      cout << file_received << endl;
    };
    close(newsockfd);
    close(socketfd);
    return 0;
}

