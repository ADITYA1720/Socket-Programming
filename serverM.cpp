#include<stdio.h>
#include<stdlib.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<sys/time.h>
#include<netinet/in.h>
#include<string.h>
#include<unistd.h>
#include<arpa/inet.h>
#include<time.h>
#include<iostream>
#include<fstream>


using namespace std;

#define ServerM_TCP_Port 25697
#define ServerM_UDP_Port 24697
#define ServerC_UDP_Port 21697
#define ServerCS_UDP_Port 22697
#define ServerEE_UDP_Port 23697

//Encryption Function
//Reference from - https://www.geeksforgeeks.org/caesar-cipher-in-cryptography/
string encryption(string s){

    //If char is from 'A-Z' or 'a-z' or '0-9' shift it by four
    int length = s.length();
    string encrypt_s="";

    for(int i=0;i<length;i++){
        if ((int)s[i]>=int('A') and (int)s[i]<=(int)('Z')){
            encrypt_s+=char(int(s[i] + 4 - 65) % 26 + 65);
        }

        else if ((int)s[i]>=int('a') and (int)s[i]<=(int)('z')){
            encrypt_s+=char(int(s[i] + 4 - 97) % 26 + 97);
        }

        else if ((int)s[i]>=48 and (int)s[i]<=57){
            encrypt_s+=char(int(s[i] + 4 - 48) % 10 + 48);
        }
        else{
            encrypt_s+=s[i];
        }
        
}
return encrypt_s;
}

int main(){


    //A TCP Socket is created using the SOCK_STREAM field and AF_INET SIgnifies 32 bit IP address.
    int tcp_serverM_socket;
    int tcp_child_socket;
    tcp_serverM_socket = socket(AF_INET,SOCK_STREAM,0);

    struct sockaddr_in serverM;
    struct sockaddr_in client;

    memset(&serverM,0,sizeof(serverM));
    serverM.sin_family = AF_INET;
	serverM.sin_port = htons(ServerM_TCP_Port);
	serverM.sin_addr.s_addr = inet_addr("127.0.0.1");

    //Binds the server with itself.
    if(::bind(tcp_serverM_socket,(struct sockaddr*)&serverM,sizeof(serverM))!=-1){
        cout<<"The main server is up and running"<<endl;
    }
    else{
        cout<<"Error in Bind"<<endl;
        return 0;
    }

    listen(tcp_serverM_socket,10);
    socklen_t address;
    address = sizeof(client);

    //Connection was established at the Parent Socket and we accept at child socket
    tcp_child_socket = accept(tcp_serverM_socket,(struct sockaddr*)&client,&address);

    //Check if the UDP Socket is created
    int serverM_UDP_socket = socket(AF_INET,SOCK_DGRAM,0);
    if (serverM_UDP_socket==-1){
        cout<<"Socket Creation Failed"<<endl;
        return 0;
    }

    //Create a UDP Socket and send it to Server C for authentication
    //UDP Socket for Credential Server
    //ServerM will send data to the Credential Server

    struct sockaddr_in serverC_udp, serverM_udp, serverCS_udp, serverEE_udp;
    memset(&serverC_udp,0,sizeof(serverC_udp));
    serverC_udp.sin_family = AF_INET;
	serverC_udp.sin_port = htons(ServerC_UDP_Port);
	serverC_udp.sin_addr.s_addr =inet_addr("127.0.0.1");

    memset(&serverCS_udp,0,sizeof(serverCS_udp));
    serverCS_udp.sin_family = AF_INET;
	serverCS_udp.sin_port = htons(ServerCS_UDP_Port);
	serverCS_udp.sin_addr.s_addr =inet_addr("127.0.0.1");
    
    memset(&serverEE_udp,0,sizeof(serverEE_udp));
    serverEE_udp.sin_family = AF_INET;
	serverEE_udp.sin_port = htons(ServerEE_UDP_Port);
	serverEE_udp.sin_addr.s_addr =inet_addr("127.0.0.1");

    memset(&serverM,0,sizeof(serverM));
    serverM_udp.sin_family = AF_INET;
	serverM_udp.sin_port = htons(ServerM_UDP_Port);
	serverM_udp.sin_addr.s_addr =inet_addr("127.0.0.1");

    if(::bind(serverM_UDP_socket,(struct sockaddr*)&serverM_udp,sizeof(serverM_udp))!=-1){
    
    cout<<endl;
    }
    else{
        cout<<"The Main Server failed to start up"<<endl;
        return 0;
    }

    while(true){
    char username[100];
    char password[100];

    //Receive the username from the Client
    bzero(username,100);
    read(tcp_child_socket,username,100);
    
    //Receive the password from the Client
    bzero(password,100);
    read(tcp_child_socket,password, 100);

    cout<<"The main server received the authentication for "<<username<< " using TCP over port "<< ServerM_TCP_Port<<endl;
    string user_encrypted = "";
    for (int i = 0; i < strlen(username); i++) {
        user_encrypted = user_encrypted + username[i];
    }

    string pass_encrypted = "";
    for (int i = 0; i < strlen(password); i++) {
        pass_encrypted = pass_encrypted + password[i];
    }

    //Encrypt the input receive from the client before sending to the Credential Server
    user_encrypted = encryption(username);
    pass_encrypted = encryption(password);

    char send_encrypted_username[100];
    for(int i=0; i<strlen(username);i++){
    send_encrypted_username[i]=user_encrypted[i];
    }

    //Send encrypted username to credential server
    sendto(serverM_UDP_socket, send_encrypted_username, sizeof(send_encrypted_username),0,(const struct sockaddr*)&serverC_udp, sizeof(serverC_udp));
    

    char send_encrypted_password[100];
    for(int i=0;i<strlen(password);i++){
    send_encrypted_password[i]=pass_encrypted[i];
    }
    //Send encrypted password to credential server
    sendto(serverM_UDP_socket, send_encrypted_password, sizeof(send_encrypted_password),0,(const struct sockaddr*)&serverC_udp, sizeof(serverC_udp));

    cout<<"The main server sent an authentication request to serverC"<<endl;

    socklen_t serverC_udp_address;
    //Receive a character value to check if authentication was successful or not
    char receive_auth_status[1];
    recvfrom(serverM_UDP_socket, receive_auth_status, sizeof(receive_auth_status),0, (struct sockaddr*)&serverC_udp, &serverC_udp_address);
    
    cout<<"The main server received the result of the authentication request from ServerC using UDP over port " <<ServerM_UDP_Port<<endl;
    
    //Send this value to the client and print if auth is successful however this will happend over TCP
    char send_auth_status[1];
    bzero(send_auth_status, sizeof(send_auth_status));
    write(tcp_child_socket, receive_auth_status,sizeof(receive_auth_status));
    cout<<"The main server sent the authentication result to the client"<<endl;
    cout<<endl;

    //If Authentication Status sent was 2 that means that it can now query with CS and EE servers.
    if(receive_auth_status[0]=='2'){
        while (true){
        x:char receive_query[100];
        bzero(receive_query,100);
        read(tcp_child_socket,receive_query,100);
        string query = string(receive_query);

        //Uses find to get the index of space character. 
        int space_finder = query.find(" ");
        string course_code = query.substr(0,space_finder);
        string category = query.substr(space_finder+1, query.length());
        
        cout<<"The main server received from "<<username<<" to query course " <<course_code<<" about "<<category<<" using TCP over port " <<ServerM_TCP_Port<<endl;
        cout<<receive_query<<endl;
        string send_query=string(receive_query);
        if(send_query.substr(0,2)=="EE"){
	
	    memset(&serverEE_udp,0,sizeof(serverEE_udp));
            serverEE_udp.sin_family = AF_INET;
	    serverEE_udp.sin_port = htons(ServerEE_UDP_Port);
	    serverEE_udp.sin_addr.s_addr =inet_addr("127.0.0.1");
            socklen_t serverEE_udp_address;
            //Send query to the EE Server on UDP if it asks info about a EE Course.
            sendto(serverM_UDP_socket, receive_query, sizeof(receive_query),0,(const struct sockaddr*)&serverEE_udp, sizeof(serverEE_udp));
            cout<<"The Main Server sent a request to EE Server"<<endl;

            //Receive the result of the query.
            char receive_ee_reply[100];
            recvfrom(serverM_UDP_socket, receive_ee_reply, sizeof(receive_ee_reply),0, (struct sockaddr*)&serverEE_udp, &serverEE_udp_address);
            cout<<"The main server received the response from server EE using UDP over port "<<ServerEE_UDP_Port<<endl;
            cout<<receive_ee_reply<<endl;

            //Send this information to Client
            char send_ee_reply[100];
            bzero(send_ee_reply,100);
            strncpy(send_ee_reply,receive_ee_reply,sizeof(send_ee_reply));
            write(tcp_child_socket, send_ee_reply, sizeof(send_ee_reply));
            cout<<"The main server sent the query information to the client"<<endl;
            cout<<endl;
            goto x;
            }

            else if(send_query.substr(0,2)=="CS"){
		    
	    memset(&serverCS_udp,0,sizeof(serverCS_udp));
            serverCS_udp.sin_family = AF_INET;
	    serverCS_udp.sin_port = htons(ServerCS_UDP_Port);
	    serverCS_udp.sin_addr.s_addr =inet_addr("127.0.0.1");

            socklen_t serverCS_udp_address;
            //Server to CS Server on UDP
            sendto(serverM_UDP_socket, receive_query, sizeof(receive_query),0,(const struct sockaddr*)&serverCS_udp, sizeof(serverCS_udp));
            cout<<"The Main Server sent a request to CS Server"<<endl;
            //Receive the result of the query
            char receive_cs_reply[100];
            recvfrom(serverM_UDP_socket, receive_cs_reply, sizeof(receive_cs_reply),0, (struct sockaddr*)&serverEE_udp, &serverCS_udp_address);
            cout<<"The main server received the response from server CS using UDP over port "<<ServerCS_UDP_Port<<endl;
            cout<<receive_cs_reply<<endl;
            //Send this result to the Client
            char send_cs_reply[100];
            bzero(send_cs_reply,100);
            strncpy(send_cs_reply,receive_cs_reply,sizeof(send_cs_reply));
            write(tcp_child_socket, send_cs_reply, sizeof(send_cs_reply));
            cout<<"The main server sent the query information to the client"<<endl;  
            cout<<endl;
            goto x;
        }
    }

    }
    }
    return 0;

}
