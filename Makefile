#Makefile for client and server

GCC = clang++ -Wall -pthread
CLIENT = client.c common.c
SERVER = server.c common.c ClientClass.cpp ServerClass.cpp

cl_sv: $(CLIENT) $(SERVER)
	$(GCC) $(CLIENT) -o cl.out
	$(GCC) $(SERVER) -o sv.out

clean:
	\rm -rf cl.out sv.out

debug_client: $(CLIENT)
	$(GCC) -g $(CLIENT) -o cl.out

debug_server: $(SERVER)
	$(GCC) -g $(SERVER) -o sv.out
