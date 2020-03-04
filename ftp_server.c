#include<stdio.h>
#include<stdlib.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<sys/time.h>
#include<netinet/in.h>
#include<unistd.h>
#include<netdb.h> 
#include<string.h>
#include<sys/sendfile.h>
#include<fcntl.h>
#include<sys/stat.h>
int main()
{
int size, i, filehandle, c;
struct stat obj;
char buf[100], filename[20], *f, command[5], buf2[100]; 
//buffer for responses, char array for filename and buffer for commands
char server_message[1024]; //message from server to client
int server_socket; //variable for server socket
server_socket = socket(AF_INET,SOCK_STREAM,0); //socket descriptor 
struct sockaddr_in server_address; //structure to hold information about server socket
server_address.sin_family = AF_INET; //internet family
server_address.sin_port = htons(9002); //port to network address conversion
server_address.sin_addr.s_addr = INADDR_ANY;

bind(server_socket, (struct sockaddr *) &server_address, sizeof(server_address)); //binding IP address and port for connection
listen(server_socket, 5); //attemp for connection is set to max. 5 times
printf("Waiting for a connection.... \n");

int client_socket;
client_socket = accept(server_socket, NULL, NULL); //accepting connection from client(s)

if(client_socket<0) //check if connection is created
{
printf("Failed to connect..... \n");
}
printf("Connected to client! \n");

recv(client_socket, buf2, 100, 0);
char l;
l = buf2[0];
if(l=='5'){
  printf("Welcome %s!", buf2);
}
else if(l=='4'){
  printf("Welcome %s!", buf2);
}
else{
  printf("Welcome %s!", buf2);
}

i = 1;
while(1) //infinite loop for accepting commands
{
    recv(client_socket, buf, 100, 0); //recieve command on the common buffer
    printf("\nServer: Command recieved. \n");
    sscanf(buf, "%s", command);
    if(!strcmp(command, "ls"))
    {
	system("ls >temps.txt");
	i = 0;
	stat("temps.txt",&obj);
	size = obj.st_size;
	send(client_socket, &size, sizeof(int),0);
        printf("All the files present on the server is displayed. \n");
	filehandle = open("temps.txt", O_RDONLY);
	sendfile(client_socket,filehandle,NULL,size);
    }
    else if(!strcmp(command,"get"))
    {
        sscanf(buf, "%s%s", filename, filename);
	stat(filename, &obj);
	filehandle = open(filename, O_RDONLY);
	size = obj.st_size;
	if(filehandle == -1)
        {
	   size = 0;
           printf("No file of the name specified present on the server. \n");
        }
	send(client_socket, &size, sizeof(int), 0);
	if(size)
        {
	sendfile(client_socket, filehandle, NULL, size);
        printf("Required file is sent. \n");
        }
    }
    else if(!strcmp(command, "put"))
    {
	int c = 0, len;
	char *f;
	sscanf(buf+strlen(command), "%s", filename);
	recv(client_socket, &size, sizeof(int), 0);
	i = 1;
	while(1)
	{
	    filehandle = open(filename, O_CREAT | O_EXCL | O_WRONLY, 0666);
	    if(filehandle == -1)
            {
	      sprintf(filename + strlen(filename), "%d", i);
	    }
	    else
	      break;
	 }
	 f = malloc(size);
	 recv(client_socket, f, size, 0);
	 c = write(filehandle, f, size);
	 close(filehandle);
         printf("File is successfully put in the server. \n");
	 send(client_socket, &c, sizeof(int), 0);
    }
    else if(!strcmp(command, "cd"))
    {
         if(chdir(buf+3) == 0)
         {
	   c = 1;
           printf("Directory changed. \n"); 
         }
	 else
         {
	   c = 0;
           printf("Failed to change directory. \n");
         }
         send(client_socket, &c, sizeof(int), 0);
    }
    else if(!strcmp(command, "bye") || !strcmp(command, "quit") || !strcmp(command, "exit"))
    {
	 printf("FTP server quitting.... \n");
	 i = 1;
	 send(client_socket, &i, sizeof(int), 0);
	 exit(0);
    }
}

close(client_socket);
return 0;
}
