
cpp = g++    
MAKE = make
RM = rm

all:
#Compile the main server
	g++ serverM.cpp -o serverM
#Compile the Credential Server
	g++ serverC.cpp -o serverC
#Compile the CS Server 
	g++ serverCS.cpp -o serverCS
#Compile the EE Server
	g++ serverEE.cpp -o serverEE
#Compile the client
	g++ client.cpp -o client

.PHONY: serverM serverC serverCS serverEE client clean

serverM:
	./serverM
serverC:
	./serverC
serverCS:
	./serverCS
serverEE:
	./serverEE
client:
	./client

clean:
	rm serverM
	rm serverC
	rm serverCS
	rm serverEE
	rm client
