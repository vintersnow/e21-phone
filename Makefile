#Makefile for client and server

GCC = gcc
CLIENT = client.c common.c
SERVER = server.c common.c

cl_sv: $(CLIENT) $(SERVER)
	$(GCC) $(CLIENT) -o cl.out
	$(GCC) $(SERVER) -o sv.out

clean:
	\rm -rf cl.out sv.out
