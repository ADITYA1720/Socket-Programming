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

#define ServerCS_UDP_Port 22697


//Reference: https://www.geeksforgeeks.org/find-the-nth-occurrence-of-a-character-in-the-given-string/
//This returns the Nth occurence of a "," in this scenario
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
        // std::cout << token << std::endl;
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
return "Invalid Entry";
}

int main(){

    //Reference from:https://cplusplus.com/doc/tutorial/files/
    ifstream input("cs.txt");
    string line;

    string cs_file_read[100];
    int i=0;
    while(getline( input, line) ) {
        cs_file_read[i]=line;
        i=i+1;
    }
    input.close();

    //Create a UDP Socket at the Server Side
    int serverCS_UDP_socket = socket(AF_INET,SOCK_DGRAM,0);
    if (serverCS_UDP_socket==-1){
        cout<<"The Server CS was unable to startup"<<endl;
        return 0;
    }

    struct sockaddr_in serverCS_udp;
    memset(&serverCS_udp,0,sizeof(serverCS_udp));
    serverCS_udp.sin_family = AF_INET;
	serverCS_udp.sin_port = htons(ServerCS_UDP_Port);
	serverCS_udp.sin_addr.s_addr =inet_addr("127.0.0.1");

    if(::bind(serverCS_UDP_socket,(struct sockaddr*)&serverCS_udp,sizeof(serverCS_udp))!=-1){
    cout<<"The Server CS is up and running using UDP on port 22697"<<endl;
    }
    else{
    cout<<"The Server CS was unable to startup"<<endl;
    return 0;
    }

    while(true){
    struct sockaddr_in serverM_udp;

    char read_cs_query[100];
    socklen_t serverM_udp_address;
    serverM_udp_address=sizeof(serverM_udp);
    recvfrom(serverCS_UDP_socket, read_cs_query, sizeof(read_cs_query),0, (struct sockaddr*)&serverM_udp, &serverM_udp_address);
    string cs_query = string(read_cs_query);

    int find_space=cs_query.find(" ");
    string course_code = cs_query.substr(0, find_space);
    string category = cs_query.substr(find_space+1, cs_query.length());
    cout<<"The Server CS received a request from the Main Server about the "<< category <<" of "<<course_code<<endl;

    //Course Index=-1 specifies the course is not present. After iterating through the file if the value
    // of the variable does not change that means it is absent 
    int course_index=-1;

    if(cs_query.substr(0,2)=="CS"){

        for(int j=0; j<i;j++){
            int comma_occurence = cs_file_read[j].find(',');
            string check_string = cs_file_read[j].substr(0,comma_occurence);
            if (cs_query.find(check_string)!= std::string::npos){
                course_index=j;
                break;
            }
        }
    

        if(course_index==-1){
            cout<<"Didn’t find the course: "<<course_code<<endl;
            string reply="Didn’t find the course: "+course_code;
            char send_reply[100];
            bzero(send_reply,100);
            strncpy(send_reply, reply.c_str(), reply.length());
            sendto(serverCS_UDP_socket, send_reply, sizeof(send_reply),0,(const struct sockaddr*)&serverM_udp, sizeof(serverM_udp));
        }
        else{
        if(cs_query.find("Credit")!= std::string::npos){
            char ch = ',';
            string credit=findNthOccur(cs_file_read[course_index], ch, 1);
            cout<<"The course information has been found: The Credit of "<<course_code<<" is "<<credit<<endl;
            //Send a reply to the main server
            char send_reply[100];
            bzero(send_reply,100);
            strncpy(send_reply, credit.c_str(), credit.length());
            sendto(serverCS_UDP_socket, send_reply, sizeof(send_reply),0,(const struct sockaddr*)&serverM_udp, sizeof(serverM_udp));
        }
        else if(cs_query.find("Professor")!= std::string::npos){
            char ch = ',';
            string professor = findNthOccur(cs_file_read[course_index], ch, 2);
            
            cout<<"The course information has been found: The Professor of "<<course_code<<" is "<<professor<<endl;
            //Send this to the main Server
            char send_reply[100];
            bzero(send_reply,100);
            strncpy(send_reply, professor.c_str(), professor.length());
            sendto(serverCS_UDP_socket, send_reply, sizeof(send_reply),0,(const struct sockaddr*)&serverM_udp, sizeof(serverM_udp));
            
        }
        else if(cs_query.find("Days")!= std::string::npos){
            char ch = ',';
            string days = findNthOccur(cs_file_read[course_index], ch, 3);
            cout<<"The course information has been found: The Days of "<<course_code<<" is "<<days<<endl;
            //Send this to the main Server
            char send_reply[100];
            bzero(send_reply,100);
            strncpy(send_reply, days.c_str(), days.length());
            sendto(serverCS_UDP_socket, send_reply, sizeof(send_reply),0,(const struct sockaddr*)&serverM_udp, sizeof(serverM_udp));
            
        }
        else if(cs_query.find("CourseName")!= std::string::npos){
            char ch = ',';
            string course_name = findNthOccur(cs_file_read[course_index], ch, 4);
            cout<<"The course information has been found: The Course Name of "<<course_code<<" is "<<course_name<<endl;
            //Send this to the main Server
            char send_reply[100];
            bzero(send_reply,100);
            strncpy(send_reply, course_name.c_str(), course_name.length());
            sendto(serverCS_UDP_socket, send_reply, sizeof(send_reply),0,(const struct sockaddr*)&serverM_udp, sizeof(serverM_udp));
            
        }

    }
    }
    
    cout<<"The Server CS finished sending the response to the Main Server"<<endl;
    }
    return 0;
}