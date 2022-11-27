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
#include <fstream>
#include<cstring>

using namespace std;

#define ServerM_TCP_Port 25697

int main(){

    //A TCP Socket is created using the SOCK_STREAM field and AF_INET SIgnifies 32 bit IP address
    int client_socket;
    client_socket=socket(AF_INET,SOCK_STREAM,0);
    // Check if socket is created or not : -1 is returned if socket creation fails
    if (client_socket==-1){
        cout<<"\nSocket not created"<<endl;
        return 0;
    }

    struct sockaddr_in server_M;
    memset(&server_M,0,sizeof(server_M));
	server_M.sin_family=AF_INET;
	server_M.sin_port = htons(ServerM_TCP_Port);
	server_M.sin_addr.s_addr = inet_addr("127.0.0.1");

    //Connection of Client with Main Server
    if (connect(client_socket,(struct sockaddr*)&server_M,sizeof(server_M))==-1){
		cout<<"Connection Failed"<<endl;
		return 0;
	}

    cout<<"Client is up and running"<<endl;

    string user;

    int no_of_attempts=3;

    //This loop will iterate a maximum of 3 times. 
    while (no_of_attempts>0)
    {

    char username[100];
    char password[100];
    
    cout<<"Please enter username: ";
    bzero(username, strlen(username));
    cin>>username;
    write(client_socket, username,strlen(username));
    
    cout<<"Please enter password: ";
    bzero(password, strlen(password));
    cin>>password;

    //Send the authentication Request to ServerM who will send it to ServerC
    write(client_socket, password,strlen(password));
    cout<<username<<" sent an authentication request to the main server"<<endl;

    //Receive the Authentication Status from ServerM who gets a response ServerC
    char receive_auth_status[1];
    bzero(receive_auth_status, sizeof(receive_auth_status));
    read(client_socket,receive_auth_status,1);
    

    //ServerC returns a number for every authentication alert:
    //0-When Username does not exist; 1-When Password does not match; 2-When Authentication is successful
    if (receive_auth_status[0]=='2'){
        cout<<username<< " received the result of authentication using TCP over port "<< ServerM_TCP_Port<<" Authentication is successful"<<endl;
        cout<<endl;
        //To be used in the next loop. username has limited scope within the loop
        user=username;
        break;
    }
    else if (receive_auth_status[0]=='1'){
        cout<<username<< " received the result of authentication using TCP over port "<<ServerM_TCP_Port<<" Authentication failed: Password does not match"<<endl;
        no_of_attempts=no_of_attempts-1;
        cout<<"No of attempts remaining: "<<no_of_attempts<<endl;
        cout<<endl;
    }
    else if(receive_auth_status[0]=='0'){
        cout<<username<< " received the result of authentication using TCP over port "<<ServerM_TCP_Port<<" Authentication failed: Username does not exist"<<endl;
        no_of_attempts=no_of_attempts-1;
        cout<<"No of attempts remaining: "<<no_of_attempts<<endl;
        cout<<endl;
    }
    }

    if(no_of_attempts==0){
        cout<<"Authentication Failed for 3 attempts. Client will shut down"<<endl;
        exit(1);
    }

    //Phase 2(Client):After receiving auth status as success the user can now input query.
    //New Request Flag to check if the client is making his first request or not.
    int new_request_flag=0;
    int course_flag=1;

    while(true){
        char course_code[100];
        if(new_request_flag==0){
            cout<<"Please enter the course code to query:"<<endl;
            //After the first request is made new_request_flag is set to 1
            new_request_flag=1;
            }
        else{
            cout<<endl;
            cout<<"-----Start a new request----"<<endl;
            cout<<"Please enter the course code to query:"<<endl;
            }
        cin>>course_code;
        if(strlen(course_code)==5){
            char type[100];
            cout<<"Please enter the category (Credit / Professor / Days / CourseName):"<<endl;
            cin>>type;

            //Sending the query to ServerM who will then forward it to EE or CS Server.
            string query = string(course_code)+" "+string(type);
            char send_query[100];
            bzero(send_query,100);
            strncpy(send_query, query.c_str(), query.length());
            cout<<user<< " sent a request to the main server"<<endl;
            write(client_socket, send_query,strlen(send_query));

            //The Client receives a reply from ServerM about the course and information regarding it.
            char receive_reply[100];
            bzero(receive_reply,100);
            read(client_socket,receive_reply, 100);
            string response = string(receive_reply);
            cout<<"The client received the response from the Main server using TCP over port "<<ServerM_TCP_Port<<endl;
            if(response.find("Did")!= string::npos){
                cout<<response<<endl;
            }
            else{
            cout<<"The "<<type<<" of "<< course_code<<" is "<<response<<endl;
            }
        }
        //When the query is incomplete. Example: EE45, CS10, CS1
        else{
            cout<<"Didn't find course: "<<course_code<<endl;
            cout<<"The client received the response from the Main server using TCP over port "<<ServerM_TCP_Port<<endl;
            cout<<endl;
        }
    }

    return 0;
}