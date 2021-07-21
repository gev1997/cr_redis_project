#ifndef CLIENT_H
#define CLIENT_H

#include <iostream>
#include <unordered_map>
#include <vector>
#include <set>
#include <list>
#include <algorithm>
#include <sys/socket.h>
#include <string.h>
#include <errno.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <unistd.h>

#include <netdb.h>
#include <unistd.h>


namespace Connection {

void logError(std::string message) {
    std::cerr << "error: " << message << " " << strerror(errno) << std::endl;
    exit(1);
}

void new_send() {
    sockaddr_in addres;
    memset(&addres, 0, sizeof(addres));

    //addres.sin_addr.s_addr = htons(INADDR_ANY);//inet_addr("127.0.0.1");
    addres.sin_family = AF_INET;
    addres.sin_port = htons(1600);

    inet_pton(AF_INET, "127.0.0.1", &addres.sin_addr);

    int sockDes = socket(AF_INET, SOCK_STREAM, 0);

    if (sockDes == -1) {
        logError("Can't create socket");
    }

    int connF = connect(sockDes, reinterpret_cast<struct sockaddr*>(&addres), sizeof(addres));

    if (connF == -1) {
        logError("Can't connect");
    }

    //send
    std::string message;
    getline(std::cin, message);

    size_t sendTo = send(sockDes, message.c_str(), message.size(), 0);

    if (sendTo != message.size()) {
        logError("Can't send data");
    }

    shutdown(sockDes, SHUT_RDWR);
    close(sockDes);
}

void new_recv() {
    sockaddr_in addres;
    memset(&addres, 0, sizeof(addres));

    addres.sin_addr.s_addr = INADDR_ANY;//inet_addr("127.0.0.1");
    addres.sin_family = AF_INET;
    addres.sin_port = htons(2000);

    int sockDes = socket(AF_INET, SOCK_STREAM, 0);

    if (sockDes == -1) {
        logError("Can't create socket");
    }

    int connF = connect(sockDes, reinterpret_cast<sockaddr*>(&addres), sizeof(addres));

    if (connF == -1) {
        logError("Can't connect");
    }

    //recive
    char buffer[100];
    int recvFrom = recv(sockDes, buffer, 100, 0);

    if (recvFrom == -1) {
        logError("Can't send data");
    }

    buffer[recvFrom] = '\0';

    std::cout << "message from server " << buffer << std::endl;

    shutdown(sockDes, SHUT_RDWR);
    close(sockDes);
}

}; //Connection

class redis_client {
public:
    redis_client()
        : _commands{ "sadd", "lpush", "rpush", "lpop", "rpop","lrange","spop","smembers"}
    {}

    void get_send() {
        Connection::new_send();
    }

    bool db_call(std::string command) {
        if (command.empty()) {
            return false;
        }

        std::string firstWord = command.substr(0, command.find(' '));
        std::for_each(firstWord.begin(), firstWord.end(), [](char& c) {
            c = std::tolower(c);
            });

        if (_commands.find(firstWord) != _commands.end()) {
            Connection::new_send();
            return true;
        }
        else {
            return false;
        }
    }

private:
    std::set<std::string> _commands;
};

#endif // CLIENT_H
