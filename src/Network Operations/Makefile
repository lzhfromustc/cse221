all: build_dir rtt_client rtt_server bandwidth_client bandwidth_server connection_client connection_server


build_dir:
	if [ ! -d build ];then \
		mkdir -p build; \
	fi

rtt_client: rtt_client.c
	gcc rtt_client.c -o build/rtt_client
rtt_server: rtt_server.c
	gcc rtt_server.c -o build/rtt_server
bandwidth_client: bandwidth_client.c
	gcc bandwidth_client.c -o build/bandwidth_client
bandwidth_server: bandwidth_server.c
	gcc bandwidth_server.c -o build/bandwidth_server
connection_client: connection_client.c
	gcc connection_client.c -o build/connection_client
connection_server: connection_server.c
	gcc connection_server.c -o build/connection_server

clean:
	if [ -d build ];then \
		rm -r build; \
	fi
