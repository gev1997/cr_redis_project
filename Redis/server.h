#ifndef SERVER_H
#define SERVER_H

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
#include <iterator>

using HTable = std::unordered_map<std::string, std::pair<std::string, void*>>;

const std::string SADD = "sadd";
const std::string SPOP = "spop";
const std::string SMEMBERS = "smembers";

const std::string LPUSH = "lpush";
const std::string LPOP = "lpop";
const std::string LRANGE = "lrange";

const std::string RPUSH = "rpush";
const std::string RPOP = "rpop";
const std::string RRANGE = "rrange";

enum {
    cmd = 0,
    key,
    value
};

namespace Connection {

void logError(std::string message) {
    std::cerr << "error: " << message << " " << strerror(errno) << std::endl;
    exit(1);
}

void new_send(std::string message) {
    sockaddr_in address;
    memset(&address, 0, sizeof(address));

    address.sin_addr.s_addr = inet_addr("0.0.0.0");
    address.sin_family = AF_INET;
    address.sin_port = htons(2000);

    int socketDes = socket(AF_INET, SOCK_STREAM, 0);

    if (socketDes == -1) {
        logError("Can't create socket");
    }

    int bindF = bind(socketDes, (sockaddr*)&address, sizeof(address));

    if (bindF == -1) {
        logError("Cant bind");
    }

    if (listen(socketDes, 0) == -1) {
        logError("Can't listen");

    }

    int clientSocket = accept(socketDes, 0, 0);

    if (clientSocket == -1) {
        logError("Can't accept");
    }

    size_t sended = send(clientSocket, message.c_str(), message.size(), 0);

    if (sended != message.size()) {
        logError("Can't send data");
    }

    shutdown(clientSocket, SHUT_RDWR);

    close(socketDes);
    close(clientSocket);
}

std::string new_recv(){
    sockaddr_in address;
    memset(&address, 0, sizeof(address));

    address.sin_addr.s_addr = inet_addr("127.0.0.1");
    address.sin_family = AF_INET;
    address.sin_port = htons(1600);

    int socketDes = socket(AF_INET, SOCK_STREAM, 0);

    if (socketDes == -1) {
        logError("Can't create socket");
    }

    int bindF = bind(socketDes, (sockaddr*)&address, sizeof(address));

    if (bindF == -1) {
        logError("Cant bind");
    }

    if (listen(socketDes, 0) == -1) {
        logError("Can't listen");
    }

    int clientSocket = accept(socketDes, 0, 0);

    if (clientSocket == -1) {
        logError("Can't accept");
    }

    char buffer[100];

    int received = recv(clientSocket, buffer, 100, 0); //changed

    if (received == -1) {
        logError("Can't send data");
    }

    buffer[received] = '\0';

    std::cout << buffer << std::endl;

    std::string tmp;

    for (int i = 0; buffer[i] != '\0'; ++i){
        tmp.push_back(buffer[i]);
    }

    shutdown(clientSocket, SHUT_RDWR);

    close(socketDes);
    close(clientSocket);

    return tmp;
}

}; //connection


namespace SET {
    void sadd(HTable& data, std::vector<std::string>& token) {
        if (data.count(token[key]) == 0) {
            data.insert({ token[key],
                            std::make_pair(token[cmd],
                            static_cast<std::set<std::string>*>(new std::set<std::string>({token[key]})))
                        });
        }
        if (data.at(token[key]).first == SADD) {
            static_cast<std::set<std::string>*>(data.at(token[key]).second)->insert(token[value]);
        }
    }

    void spop(HTable& data, std::vector<std::string>& token) {
        if (data.count(token[key]) == 0) {
            std::cout << "Set does not contain element with that value";
        }
        else {
            static_cast<std::set<std::string>*>(data.at(token[key]).second)->erase(token[value]);
        }
    }

    void smembers(HTable& data, std::vector<std::string>& token) {
        if (data.count(token[key]) == 0) {
            std::cout << "Set with " << token[key] << " is not found!" << std::endl;
        }
        else {
            std::copy(static_cast<std::set<std::string>*>(data.at(token[key]).second)->begin(),
                static_cast<std::set<std::string>*>(data.at(token[key]).second)->end(),
                std::ostream_iterator<std::string>(std::cout, " "));
        }
    }
};

namespace LIST {
    void lpush(HTable& data, std::vector<std::string>& token) {
        if (data.count(token[key]) == 0) {
            data.insert({ token[key],
                          std::make_pair(token[cmd],
                          static_cast<std::list<std::string>*>(new std::list<std::string>({token[key]})))
                        });
        }

        if (data.at(token[key]).first == LPUSH) {
            static_cast<std::list<std::string>*>(data.at(token[key]).second)->push_front(token[value]);
        }
    }

    void rpush(HTable& data, std::vector<std::string>& token) {
        if (data.count(token[key]) == 0) {
            data.insert({ token[key],
                          std::make_pair(token[cmd],
                          static_cast<std::list<std::string>*>(new std::list<std::string>({token[key]})))
                });
        }
        if (data.at(token[key]).first == RPUSH) {
            static_cast<std::list<std::string>*>(data.at(token[key]).second)->push_back(token[value]);
        }
    }

    void lpop(HTable& data, std::vector<std::string>& token) {
        if (data.count(token[key]) == 0) {
            std::cout << "Set does not contain element with that value";
        }
        else {
            static_cast<std::list<std::string>*>(data.at(token[key]).second)->pop_front();
        }
    }

    void rpop(HTable& data, std::vector<std::string>& token) {
        if (data.count(token[key]) == 0) {
            std::cout << "List does not contain element with that value ";
        }
        else {
            static_cast<std::list<std::string>*>(data.at(token[key]).second)->pop_back();
        }
    }

    void lrange(HTable& data, std::vector<std::string>& token) {
            std::copy(static_cast<std::list<std::string>*>(data.at(token[key]).second)->begin(),
                --static_cast<std::list<std::string>*>(data.at(token[key]).second)->end(),
                std::ostream_iterator<std::string>(std::cout, " "));
    }

    void rrange(HTable& data, std::vector<std::string>& token) {
        std::copy(++static_cast<std::list<std::string>*>(data.at(token[key]).second)->rbegin(),
            static_cast<std::list<std::string>*>(data.at(token[key]).second)->rend(),
            std::ostream_iterator<std::string>(std::cout, " "));
    }
};

class redis_server {
public:
    std::string new_recv() {
        std::string str = Connection::new_recv();
        return str;
    }

    void new_send(std::string a) {
        Connection::new_send(a);
    }

    void _parser(std::string command) {
        std::vector<std::string> token;

        while (!command.empty()) {
            std::string firstWord = command.substr(0, command.find(' '));
            token.push_back(firstWord);
            command.erase(0, firstWord.size() + 1);
        }

        if (token[cmd] == SADD) {
            SET::sadd(_data, token);
        }
        else if (token[cmd] == SPOP) {
            SET::spop(_data, token);
        }
        else if (token[cmd] == LPUSH) {
            LIST::lpush(_data, token);
        }
        else if (token[cmd] == LPOP) {
            LIST::lpop(_data, token);
        }
        else if (token[cmd] == RPUSH) {
            LIST::rpush(_data, token);
        }
        else if (token[cmd] == LPOP) {
            LIST::rpop(_data, token);
        }
        else if (token[cmd] == LRANGE) {
            LIST::lrange(_data, token);
        }
        else if (token[cmd] == RRANGE) {
            LIST::rrange(_data, token);
        }
    }

private:
    HTable _data;
};

#endif // SERVER_H
