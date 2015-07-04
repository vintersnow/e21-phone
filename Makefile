#Makefile for client and server

GCC = clang++ -Wall -pthread
CLIENT = client.cpp common.cpp fft.cpp
SERVER = server.cpp common.cpp ClientClass.cpp ServerClass.cpp
CL_OUT = cl.out
SV_OUT = sv.out

cl_sv: $(CL_OUT) $(SV_OUT)

$(CL_OUT): $(CLIENT)
	$(GCC) $(CLIENT) -o $(CL_OUT)

$(SV_OUT): $(SERVER)
	$(GCC) $(SERVER) -o $(SV_OUT)

clean:
	\rm -rf cl.out sv.out

debug_client: $(CLIENT)
	$(GCC) -g $(CLIENT) -o cl.out

debug_server: $(SERVER)
	$(GCC) -g $(SERVER) -o sv.out
