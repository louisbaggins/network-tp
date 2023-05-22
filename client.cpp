#include <sys/socket.h>
#include <netinet/in.h>
#include <cstring>
#include <unistd.h>
#include <arpa/inet.h>
#include <iostream>
#include <fstream>

using namespace std;

const int MAX_MSG_SIZE = 512;
const int PORT = 51511;

bool compareStringsIgnoreCase(const string& str1, const string& str2) {
    if (str1.size() != str2.size()) {
        return false;
    }
    
    for (size_t i = 0; i < str1.size(); ++i) {
        if (tolower(str1[i]) != tolower(str2[i])) {
            return false;
        }
    }
    
    return true;
}

int isFileSupported(string& filename) {
    string extension;
    size_t dotIndex = filename.find_last_of('.');
    if (dotIndex != string::npos) {
        extension = filename.substr(dotIndex + 1);
    }
    
    if (compareStringsIgnoreCase(extension, "txt") || compareStringsIgnoreCase(extension, "c") ||
        compareStringsIgnoreCase(extension, "cpp") || compareStringsIgnoreCase(extension, "py") ||
        compareStringsIgnoreCase(extension, "tex") || compareStringsIgnoreCase(extension, "java")) {
        return 1;
    }
    
    return 0;
}

int connect(const char* version, const char *port){
    if(sizeof(version) > 3){
        int socket_fd = socket(AF_INET, SOCK_STREAM, 0);
        if (socket_fd < 0) {
            perror("Erro ao criar socket");
            return 1;
        }

        struct sockaddr_in serv_addr;
        serv_addr.sin_family = AF_INET;
        serv_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
        serv_addr.sin_port = htons(atoi(port));

        if (connect(socket_fd, (struct sockaddr*) &serv_addr, sizeof(serv_addr)) < 0) {
            perror("Erro ao conectar-se ao servidor");
            return 1;
        }

        cout << "Conectado ao servidor!" << endl;
        return socket_fd;
    }
    else{
        int socket_fd = socket(AF_INET6, SOCK_STREAM, 0);
        if (socket_fd < 0) {
            perror("Erro ao criar socket");
            return 1;
        }

        struct sockaddr_in6 serv_addr;
        serv_addr.sin6_family = AF_INET;
        serv_addr.sin6_addr = in6addr_any;
        serv_addr.sin6_port = htons(atoi(port));

        if (connect(socket_fd, (struct sockaddr*) &serv_addr, sizeof(serv_addr)) < 0) {
            perror("Erro ao conectar-se ao servidor");
            return 1;
        }

        cout << "Conectado ao servidor!" << endl;
        return socket_fd;
    }
}

int main(int argc, char* argv[]){
    int socket_fd = connect(argv[1], argv[2]);

    char buffer[MAX_MSG_SIZE];
    string command, file_name;
    while (true) {
        int has_file;
        cout << "Enter command: ";
        getline(cin, command);
        const char* command_str = command.c_str();
        if (command == "exit"){
            send(socket_fd, command_str, sizeof(command_str), 0);
            break;
        }
        else if(command.substr(0, 12) == "select file "){
            file_name = command.substr(12);
            if(isFileSupported(file_name) == 0){
                cout << file_name << " not valid!" << endl;
                continue;    
            }
            ifstream file(file_name);
            if (file.good()){
                cout << file_name << " selected" << endl;
                has_file = 1;
            }
            else{
                cout << file_name << " does not exist" << endl;
                has_file = 0;
            }
        }
        else if(command == "send file"){
            if(has_file != 1){
                cout << "no file selected!" << endl;
                continue;
            }
            send(socket_fd, file_name.c_str(), sizeof(file_name), 0);
            ifstream file(file_name);
            streampos fileSize = file.tellg();
            while (!file.eof()) {
                file.read(buffer, sizeof(buffer));
                size_t bytesRead = file.gcount();

                if (send(socket_fd, buffer, bytesRead, 0) < 0) {
                    perror("Erro ao enviar dados do arquivo para o servidor");
                    return 1;   
                }

                fileSize -= bytesRead;
            }
            int bytes_received = recv(socket_fd, buffer, sizeof(buffer), 0);
            if(bytes_received > 0){

            }

            file.close();
        }
        else{
            send(socket_fd, "exit", sizeof("exit"), 0);
            break;
        }
    }    

    close(socket_fd);
    return 0;
}
