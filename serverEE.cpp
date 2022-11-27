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

#define ServerEE_UDP_Port 23697

//Reference: https://www.geeksforgeeks.org/find-the-nth-occurrence-of-a-character-in-the-given-string/
string findNthOccur(string str,char ch, int n)
{
    str = str+",";
    std::string delimiter = ",";
    string return_value[100];
    size_t pos = 0;
    std::string token;
    int a=0;
    while ((pos = str.find(delimiter)) != std::string::npos) {
        token = str.substr(0, pos);
        return_value[a]=token;
        str.erase(0, pos + delimiter.length());
        a=a+1;
    }

    if(n==1){
        return return_value[1];
    }
    else if(n==2){
        return return_value[2];
    }
    else if(n==3){
        return return_value[3];
    }
    else if(n==4){
        return return_value[4];
}
else{
    return "Invalid Entry";
}
}

int main(){

    //Reference from:https://cplusplus.com/doc/tutorial/files/
    ifstream input("ee.txt");
    string line;

    string ee_file_read[100];
    int i=0;
    while(getline( input, line) ) {
        ee_file_read[i]=line;
        i=i+1;
    }
    input.close();

    //Create a UDP Socket at the ServerEE
    int serverEE_UDP_socket = socket(AF_INET,SOCK_DGRAM,0);
    if (serverEE_UDP_socket==-1){
        cout<<"The Server CS was unable to startup"<<endl;
        return 0;
    }

    struct sockaddr_in serverEE_udp;
    memset(&serverEE_udp,0,sizeof(serverEE_udp));
    serverEE_udp.sin_family = AF_INET;
	serverEE_udp.sin_port = htons(ServerEE_UDP_Port);
	serverEE_udp.sin_addr.s_addr =inet_addr("127.0.0.1");

    if(::bind(serverEE_UDP_socket,(struct sockaddr*)&serverEE_udp,sizeof(serverEE_udp))!=-1){
    cout<<"The Server EE is up and running using UDP on port 23697"<<endl;
    }
    else{
    cout<<"The Server EE was unable to startup"<<endl;
    return 0;
    }

    while(true){
    struct sockaddr_in serverM_udp;
    char read_ee_query[100];
    socklen_t serverM_udp_address;
    serverM_udp_address=sizeof(serverM_udp);
    recvfrom(serverEE_UDP_socket, read_ee_query, sizeof(read_ee_query),0, (struct sockaddr*)&serverM_udp, &serverM_udp_address);
    string ee_query = string(read_ee_query);

    int find_space=ee_query.find(" ");
    string course_code = ee_query.substr(0, find_space);
    string category = ee_query.substr(find_space+1, ee_query.length());
    cout<<"The Server EE received a request from the Main Server about the "<< category <<" of "<<course_code<<endl;

    int course_index=-1;

    if(ee_query.substr(0,2)=="EE"){
        for(int j=0; j<i; j++)
        {
            int comma_occurence = ee_file_read[j].find(',');
            string check_string = ee_file_read[j].substr(0,comma_occurence);
            if (ee_query.find(check_string)!= std::string::npos){
                course_index=j;
                break;
            }
        }
    }

    if(course_index==-1){
        cout<<"Didn’t find the course: "<<course_code<<endl;
        string reply="Didn’t find the course: "+course_code;
        char send_reply[100];
        bzero(send_reply,100);
        strncpy(send_reply, reply.c_str(), reply.length());
        sendto(serverEE_UDP_socket, send_reply, sizeof(send_reply),0,(const struct sockaddr*)&serverM_udp, sizeof(serverM_udp));
        }
    else{
        if(ee_query.find("Credit")!= std::string::npos){
            char ch = ',';
            string credit=findNthOccur(ee_file_read[course_index], ch, 1);
            cout<<"The course information has been found: The Credit of "<<course_code<<" is "<<credit<<endl;
            //Send a reply to the main server
            char send_reply[100];
            bzero(send_reply,100);
            strncpy(send_reply, credit.c_str(), credit.length());
            sendto(serverEE_UDP_socket, send_reply, sizeof(send_reply),0,(const struct sockaddr*)&serverM_udp, sizeof(serverM_udp)); 
        }
        else if(ee_query.find("Professor")!= std::string::npos){
            char ch = ',';
            string professor = findNthOccur(ee_file_read[course_index], ch, 2);
            cout<<"The course information has been found: The Professor of "<<course_code<<" is "<<professor<<endl;
            //Send a reply to the main Server
            char send_reply[100];
            bzero(send_reply,100);
            strncpy(send_reply, professor.c_str(), professor.length());
            sendto(serverEE_UDP_socket, send_reply, sizeof(send_reply),0,(const struct sockaddr*)&serverM_udp, sizeof(serverM_udp));
        }
        else if(ee_query.find("Days")!= std::string::npos){
            char ch = ',';
            string days = findNthOccur(ee_file_read[course_index], ch, 3);
            cout<<"The course information has been found: The Days of "<<course_code<<" is "<<days<<endl;
            //Send this to the main Server
            char send_reply[100];
            bzero(send_reply,100);
            strncpy(send_reply, days.c_str(), days.length());
            sendto(serverEE_UDP_socket, send_reply, sizeof(send_reply),0,(const struct sockaddr*)&serverM_udp, sizeof(serverM_udp));
        }
        else if(ee_query.find("CourseName")!= std::string::npos){
            char ch = ',';
            string course_name = findNthOccur(ee_file_read[course_index], ch, 4);
            cout<<"The course information has been found: The Course Name of "<<course_code<<" is "<<course_name<<endl;
            //Send this to the main Server
            char send_reply[100];
            bzero(send_reply,100);
            strncpy(send_reply, course_name.c_str(), course_name.length());
            sendto(serverEE_UDP_socket, send_reply, sizeof(send_reply),0,(const struct sockaddr*)&serverM_udp, sizeof(serverM_udp));
        }
    }
   
    cout<<"The Server EE finished sending the response to the Main Server"<<endl;

    }
    return 0;
}