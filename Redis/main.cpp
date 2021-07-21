#include "server.h"

int main() {
    redis_server server;
    //std::string  recved_data = server.new_recv();

    server._parser("lpush person fghggh");
    server._parser("lpush person ajdjsad");

    //server._parser("lpop person");
    server._parser("rrange person");

    return 0;
}
