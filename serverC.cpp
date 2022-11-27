#include<stdio.h>
#include<stdlib.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<sys/time.h>
#include<netinet/in.h>
#include<string.h>
#include<cstring>
#include<unistd.h>
#include<arpa/inet.h>
#include<time.h>
#include<iostream>
#include <fstream>

using namespace std;

#define ServerC_UDP_Port 21697

int main(){ 

    //File read function for Credential Server
    //Reference from:https://cplusplus.com/doc/tutorial/files/
    ifstream input("cred.txt");
    string line;

    string cred[100];
    int i=0;
    while(getline( input, line) ) {
        // cout<<line<<'\n';
        cred[i]=line;
        i=i+1;
    }
    input.close();

    //Create a UDP Socket at the Server Side: SOCK_DGRAM specifies a UDP socket, AF_INET specifies a 32bit IP address.
    int serverC_UDP_socket = socket(AF_INET,SOCK_DGRAM,0);
    if (serverC_UDP_socket==-1){
        cout<<"The ServerC was unable to start up"<<endl;
        return 0;
    }

    struct sockaddr_in serverC_udp;
    memset(&serverC_udp,0,sizeof(serverC_udp));
    serverC_udp.sin_family = AF_INET;
	serverC_udp.sin_port = htons(ServerC_UDP_Port);
	serverC_udp.sin_addr.s_addr =inet_addr("127.0.0.1");

    if(::bind(serverC_UDP_socket,(struct sockaddr*)&serverC_udp,sizeof(serverC_udp))!=-1){
    cout<<"The ServerC is up and running using UDP on port 21697"<<endl;
    }
    else{
    cout<<"The ServerC was unable to start up"<<endl;
    return 0;
    }


    while (true){
    struct sockaddr_in serverM_udp;

    //Receive the encrypted username from the ServerM
    char receive_encrypted_username[100];
    socklen_t serverM_udp_address;
    serverM_udp_address=sizeof(serverM_udp);
    recvfrom(serverC_UDP_socket, receive_encrypted_username, sizeof(receive_encrypted_username),0, (struct sockaddr*)&serverM_udp, &serverM_udp_address);

    //Receive the encrypted password from ServerM
    char receive_encrypted_password[100];
    recvfrom(serverC_UDP_socket, receive_encrypted_password, sizeof(receive_encrypted_password),0, (struct sockaddr*)&serverM_udp, &serverM_udp_address);
    cout<<"The ServerC received an authentication request from the Main Server"<<endl;

    int flag_username=0;
    int flag_password=0;

    string username = string(receive_encrypted_username);
    string password = string(receive_encrypted_password);

    int k=0;

    // Split a string from a delimiter Eg: brittany,*Tri8tium9 will be splitted into brittany and *Tri8tium9
    //https://stackoverflow.com/questions/14265581/parse-split-a-string-in-c-using-string-delimiter-standard-c
    for(int j=0; j<i;j++){

        // , is a delimiter and this piece of code will separate strings before a comma
        string str=cred[j];
        str = str+",";
        std::string delimiter = ",";
        string check_username[100];
        string check_password[100];
        size_t pos = 0;
        std::string token;
        int a=0;
        
        while ((pos = str.find(delimiter)) != std::string::npos) {
        token = str.substr(0, pos);
        if(a%2==0){
            //stores all usernames in a check username array
            // all even indexed entries will be usernames
        check_username[k]=token;
        }
        else{
            //There is not \n character at the last line that's why the if check
            // stores all passwords in a password array
            // all odd indexed entries will be stored in a password array
            check_password[k]=token;
            if(i-k!=1){
            check_password[k]=check_password[k].substr(0, check_password[k].length()-1);
            }
            k=k+1;
        }

        str.erase(0, pos + delimiter.length());
        a=a+1;
        }
        for(int l=0;l<k;l++){
        if(check_username[l]==username){
            flag_username=1;
            if(check_password[l]==password){
                flag_password=1;
                break;        
            }
        }
    }
 }
       
    //Signal variable will send the authentication status to the main server about the status
    // 0-Success; 1-
    char signal[1];
    if(flag_username==0){
        //Send a value to ServerM to understand that Incorrect Username was entered
        signal[0]='0';
        sendto(serverC_UDP_socket, signal, sizeof(signal),0,(const struct sockaddr*)&serverM_udp, sizeof(serverM_udp));
        cout<<"The ServerC finished sending the response to the Main Server"<<endl;
    }
    else if(flag_password==0){
        //Send a value to ServerM to understand that Incorrect Password was entered
        signal[0]='1';
        sendto(serverC_UDP_socket, signal, sizeof(signal),0,(const struct sockaddr*)&serverM_udp, sizeof(serverM_udp));
        cout<<"The ServerC finished sending the response to the Main Server"<<endl;
    }
    else{
        //Send a value to ServerM to understand that Authentication is successful
        signal[0]='2';
        sendto(serverC_UDP_socket, signal, sizeof(signal),0,(const struct sockaddr*)&serverM_udp, sizeof(serverM_udp));
        cout<<"The ServerC finished sending the response to the Main Server"<<endl;
    }
    }
    return 0;

}

        