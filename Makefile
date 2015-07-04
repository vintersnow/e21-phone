#Makefile for client and server

GCC = clang++ -Wall -pthread
CLIENT = client.cpp common.cpp
SERVER = server.cpp common.cpp ClientClass.cpp ServerClass.cpp

cl_sv: $(CLIENT) $(SERVER)
	$(GCC) $(CLIENT) -o cl.out
	$(GCC) $(SERVER) -o sv.out

clean:
	\rm -rf cl.out sv.out

debug_client: $(CLIENT)
	$(GCC) -g $(CLIENT) -o cl.out

debug_server: $(SERVER)
	$(GCC) -g $(SERVER) -o sv.out
