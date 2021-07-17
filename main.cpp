#include <iostream>
#include <unordered_map>
#include <vector>
#include <set>
#include <algorithm>

using HTable= std::unordered_map<std::string, std::pair<std::string, void*>>;
const std::string HSET = "hset";
const std::string HPOP = "hpop";

const std::string LPUSH = "lpush";
const std::string LPOP = "lpop";

const std::string RPUSH = "rpush";
const std::string RPOP = "rpop";

enum {
    cmd = 0,
    key,
    value
};

namespace SET {
    void push(HTable & data,  std::vector<std::string>& token) {
        if (data.count(token[key]) == 0) {
            data.insert({token[key],
                          std::make_pair(token[cmd],
                          static_cast<std::set<std::string>*>(new std::set<std::string>({token[key]})))
                         });
        }

        if (data.at(token[key]).first == HSET) {
            static_cast<std::set<std::string>*>(data.at(token[key]).second)->insert(token[value]);
        }
    }

    void pop(HTable & data,  std::vector<std::string>& token) {


    }

};

class redis_server {
public:
    bool db_call(std::string command) {
        _parser(command);

        return true;
    }

private:
    void _parser(std::string command) {
        std::vector<std::string> token;

        while (!command.empty()) {
            std::string firstWord = command.substr(0, command.find(' '));
            token.push_back(firstWord);

            command.erase(0, firstWord.size() + 1);
        }

        if (token[cmd] == HSET) {
            SET::push(_data, token);
        }
        else if (token[cmd]==HPOP) {
            SET::pop(_data, token);
        }
        else if (token[cmd] == LPUSH) {

        }
        else if (token[cmd] == LPOP) {

        }
        else if(token[cmd] == RPUSH) {

        }
        else if (token[cmd] == LPOP) {

        }

//        for (auto &i : _data) {
//            std::cout << *static_cast<std::set<std::string>*>(i.second.second)->begin();
//        }
    }

private:
    HTable _data;
};

class redis_client {
public:
    redis_client()
        : _commands{"hset", "lpush", "rpush", "lpop", "rpop"}
    {}

    bool db_call(std::string command) {
        if (command.empty()) {
            return false;
        }

        std::string firstWord = command.substr(0, command.find(' '));

        std::for_each(firstWord.begin(), firstWord.end(), [](char& c) {
            c = std::tolower(c);
        });

        if (_commands.find(firstWord) != _commands.end()) {
            //send to server

            return true;
        }
        else {
            return false;
        }
    }

    bool is_connect() {

    }

private:
    std::set<std::string> _commands;
};

int main() {
    redis_client C;
    redis_server S;
    return 0;
}
