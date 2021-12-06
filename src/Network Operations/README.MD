# Network Operations
Before running the server and client program of each experiment, you may need to go to the main function of client source file to change the corresponding server IP address. The default server IP address is 127.0.0.1. After that, you can run "make all" to compile executable client and server programs in the "build" sub-folder.
## Round-trip Time
To run round-trip time experiment. Run rtt_server on the server and rtt_client on the client. Then it will print out the cpu cycles and time (on a 3.6 GHz CPU) it takes for a round-trip on the client.
## Peak Bandwidth
To run peak bandwidth experiment. Run bandwidth_server on the server and bandwidth_client on the client. Then it will print out the peak bandwidth (on a 3.6 GHz CPU) with a unit of MB/s.
## Connection Overhead
To run connection overhead experiment. Run connection_server on the server and connection_client on the client. Then it will print out the cpu cycles and time (on a 3.6 GHz CPU) it takes for a TCP setup followed by the cpu cycles and time it takes for a TCP teardown on the client.
## Note
On the server side, you may need to kill a server program to release the network port it occupies for another experiment.
