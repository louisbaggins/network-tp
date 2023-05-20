#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <iostream>
#include <fstream>

using namespace std;

const int MAX_MSG_SIZE = 512;
const int PORT = 51511;

int main() {
    // Cria um socket IPv4, TCP
    int socket_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (socket_fd < 0) {
        perror("Erro ao criar socket");
        return 1;
    }

    // Configura o endereço e a porta do servidor
    struct sockaddr_in serv_addr;
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    serv_addr.sin_port = htons(51511);

    // Conecta ao servidor
    if (connect(socket_fd, (struct sockaddr*) &serv_addr, sizeof(serv_addr)) < 0) {
        perror("Erro ao conectar-se ao servidor");
        return 1;
    }

    std::cout << "Conectado ao servidor!" << std::endl;

    char buffer[MAX_MSG_SIZE];
    string command, filename;
    while (true) {
        cout << "Enter command: ";
        getline(cin, command);
        ifstream file(command.c_str(), ios::binary | ios::ate);
        if (!file) {
            perror("Erro ao abrir o arquivo");
            return 1;
        }

        // Obter o tamanho do arquivo
        streampos fileSize = file.tellg();
        file.seekg(0, ios::beg);

        
        if (command == "teste"){
           
        }
        if (command == "exit") {
            break;
        }

        if(send(socket_fd, command.c_str(), sizeof(command.c_str()), 0) > 0){
            while(recv(socket_fd, buffer, sizeof(buffer), 0) < 0){
                send(socket_fd, command.c_str(), sizeof(command.c_str()), 0);
            }
            std::cout << buffer << std::endl;
        }

        while (fileSize > 0) {
            // Ler o próximo pacote do arquivo
            file.read(buffer, sizeof(buffer));
            size_t bytesRead = file.gcount();

            // Enviar o pacote para o servidor
            if (send(socket_fd, buffer, bytesRead, 0) < 0) {
                perror("Erro ao enviar dados do arquivo para o servidor");
                return 1;
            }

            fileSize -= bytesRead;
        }

        // Fechar o arquivo
        file.close();
    }    
    // Usa o socket...
    
    // Fecha o socket quando terminar de usá-lo
    close(socket_fd);
    return 0;
}
