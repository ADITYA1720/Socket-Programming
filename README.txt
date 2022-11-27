#README for Web Registration System implemented using Socket Programming.

a) NAME : ADITYA DHANANJAY JADHAV

b) USC STUDENT ID : 

c) WORK DONE IN THE PROJECT:

- Implemented a web registration system using Socket Programming.The project consists of 5 Modules that 
  has 4 Servers and a client.
- Servers
- ServerM is the main server which takes requests from the clients and 
  queries it to ServerC, ServerCS or ServerEE depending on the request.
- The Client first has to be authorized to make a request to query 
  and has 3 attempts to make a successful login attempt to make a query.
- The client enters username and password and that is sent to the Credential
  Server- ServerC which checks the credentials and sends a response to the main 
  server which will send that response to the client.
- If Authorizarion is successful:
  The client can now make a query regarding the subject and can ask for four options
  a) Credit b)Professor c) Days and d)Course Name.
  Upon sending a relevant query the serverM will come back with a response.
- Case 1: Query : EExxx(xxx: subject code)
  The query will be sent from the client to serverM who will forward it to the
  EE departmental server and come back with a response.
- Case 2: Query : CSxxx(xxx: subject code)
  The query will be sent from the client to serverM who will forward it to the
  CS departmental server and come back with a response.
- The client can make infinite queries until it is manually stopped or serverM is manually 
  stopped.

d) Code files.

Makefile :
The makefile is used to compile and run all code files together using a single command.
It saves the hassle of compiling all files separately.

ServerM:
ServerM acts as mediator between the client and other servers. The client makes requests to the 
main server who then forwards it to the respective server.

ServerC:
Credential Server is used to authenticate the username and password coming from the main server.
The main server receives those from the client. The Credential servers reads the credentials from a text files
"cred.txt" which contains the encrypted usernames and passwords of authenticated users. The data read from the files 
is stored in a data structure(string array).

ServerCS:
After succesful authentication, the user queries for a course and its information. If the course is a CS course
the main server will forward the query to this server. The serverCS will read available course from a text file
"cs.txt" which has all course information and stores them in a data structure(string array). The serverCS will then
compare the incoming query with the information stored in the string array. If the information is found the serverCS 
sends the information to the main server who will forward it to the client. If the information is not found he will send
information not found.

ServerEE:
After succesful authentication, the user queries for a course and its information. If the course is a EE course
the main server will forward the query to this server. The serverEE will read available course from a text file
"ee.txt" which has all course information and stores them in a data structure(string array). The serverEe will then
compare the incoming query with the information stored in the string array. If the information is found the serverEE 
sends the information to the main server who will forward it to the client. If the information is not found he will send
information not found.

Client:
The client has to first authenticate to enter a course query. He gets three attempts for a successful try or the 
client shuts down. If authentication is successful the client can query a course. Eg; EE450 or CS100. If course information
is found the main server sends the info back else it asks the user to try for another course. The client can submit 
infinite requests if the main server is on.

e) Format of messages exchanged:

Client:

- variable name -> username 
- type -> character array
- description -> Accepts the username as input in a character array 

- variable name -> password 
- type -> character array
- description -> Accepts the password as input in a character array 

- variable name -> receive_auth_status 
- type -> character array
- description -> reads message from ServerM about authentication- 
0-When Username does not exist; 1-When Password does not match; 2-When Authentication is successful 

- variable name -> course_code 
- type -> character array
- description -> reads the course code entered by the client Eg EE450, CS100

- variable name -> receive_auth_status 
- type -> character array
- description -> reads message from ServerM about authentication- 

ServerM:
- variable name -> username
- type -> character array
- description -> receives username entered by client over TCP

- variable name -> password
- type -> character array
- description -> receives password entered by client over TCP

- variable name -> send_encrypted_username
- type -> character array
- description -> sends username entered by client over UDP to serverC 

- variable name -> send_encrypted_password 
- type -> character array
- description -> sends password entered by client over UDP to serverC 

- variable name -> receive_query
- type -> character array
- description -> receives the query sent by the client over UDP

- variable name -> receive_ee_reply
- type -> character array
- description -> Receives the reply from the EE server over UDP

- variable name -> send_ee_reply
- type -> character array
- description -> send the reply received from the EE server to the client over TCP

- variable name -> receive_cs_reply
- type -> character array
- description -> Receives the reply from the CS server over UDP

- variable name -> send_cs_reply
- type -> character array
- description -> send the reply received from the CS server to the client over TCP

ServerC:

- variable name -> receive_encrypted_username 
- type -> character array
- description -> Accepts the username as input in a character array in encrypted format from ServerM 

- variable name -> receive_encrypted_password 
- type -> character array
- description -> Accepts the password as input in a character array in encrypted format from serverM

- variable name -> signal 
- type -> character array
- description -> will send the authentication status to the main server about the status of authentication.


ServerCS:

- variable name -> read_cs_query 
- type -> character array
- description -> Read the query sent by the ServerM to check if it exists

- variable name -> course_index 
- type -> character array
- description -> Course index is set to -1. If the course is found the index of the course read from the file is stored in it.

- variable name -> send_cs_reply
- type -> character array
- description -> Send course info if the course is found or send did not find course information


ServerEE:

- variable name -> read_ee_query 
- type -> character array
- description -> Read the query sent by the ServerM to check if it exists

- variable name -> course_index 
- type -> character array
- description -> Course index is set to -1. If the course is found the index of the course read from the file is stored in it.

- variable name -> send_ee_reply
- type -> character array
- description -> Send course info if the course is found or send did not find course information


f) Idiosyncrasy of the project

Workflow-
1. Authentication at the client side. The client has to be authorized to make course query.
2. The authentication information will be checked at serverC and response sent to serverM who 
   forwards it to the client.
3. The client can now makes queries for course and its information.
4. The request is sent to ServerCS or ServerEE by serverM depending on the course queried by 
   the client.
5. If the info is found the CS or EE server sends back the response to the main server who sends it back
   to the client.
6. The client can now make a new query again and steps 3-5 are repeated.

- The project runs correctly on all test cases subject to the following constraints:
1. The Casing of the category should be correctly matched with the display messages.
For Example: When the client is asked for Credit. He should enter Credit.
The project will not get a correct output if he enters credit, CreDit or anything else.

2. The course code should also be entered in all caps.
For Example: The query for a course it should enter EE450 or CS100.
All other inputs such as ee450 or cs100 will be invalid.

3. Bind Error on serverM
It may happen that there was an error in bind after compiling. A waiting time of roughly 10s
is enough to get the code up and running. This might happen if the program is incorrectly terminated 
and the port is not free which causes a bind error.

g) Reused code

1. Code to read a file 
https://cplusplus.com/doc/tutorial/files/

2. Cipher the input - Caesar Ciper from GFG
https://www.geeksforgeeks.org/caesar-cipher-in-cryptography/

3. Split a string from a delimiter - 
//https://stackoverflow.com/questions/14265581/parse-split-a-string-in-c-using-string-delimiter-standard-c

4. Implementing socket connection and functionalities.
https://beej.us/guide/bgnet/
https://beej.us/guide/bgc/

Get the project running

#Using Make File commands

Compile the project using - makeall 

make clean - cleans all generated output files.

# Run the project

Step 1 - Start ServerM using : ./serverM
Step 2 - Start ServerC using : ./serverC
Step 3 - Start ServerCS using : ./serverCS
Step 4 - Start ServerEE using : ./serverEE
Step 5 - Start Client using : ./client
