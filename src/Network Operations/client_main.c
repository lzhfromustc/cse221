#include <client.h>

int main(){
    char* addr = "127.0.0.1";
    unsigned short port = 9999;
    
    get_round_trip_time(addr, port);
    
    return 0;
}