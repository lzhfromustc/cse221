#include <server.h>

int main(){
    char* addr = "127.0.0.1";
    unsigned short port = 9999;
    round_trip_server(addr, port);
    return 0;
}