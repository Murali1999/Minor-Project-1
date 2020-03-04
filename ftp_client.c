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
int choice, size, filehandle;
struct stat obj; //structure for storing data about folders and directories
char buf[100], filename[20], *f, buf2[100];
int netsocket, status; //variable for client socket for holding server data
netsocket = socket(AF_INET,SOCK_STREAM,0); //create socket descriptor
struct sockaddr_in server_address; //structure to hold information about client socket
server_address.sin_family = AF_INET; 
server_address.sin_port = htons(9002); //host to network conversion using htons()
server_address.sin_addr.s_addr = INADDR_ANY; 

int connect_status; 
connect_status = connect(netsocket,(struct sockaddr *) &server_address, sizeof(server_address));

if(connect_status==-1) //check if connection is created
{
printf("There was an error while making a connection"); //message to print if connection isn't formed
}
printf("\nWaiting to connect.... \n"); 
printf("Connected to server! \n"); //message to print if connection is formed

//Now the user logs in to the server which is validated at the server side and then acknowledgement is sent to the client side

char id[20],pass[20],login[40];
int flag=0;
printf("Enter username: ");
scanf("%s",id);
printf("Enter password: ");
scanf("%s",pass);
strcpy(login,id);
strcat(login,"-");
strcat(login,pass);
strcat(login,"\n");
printf("%s",login);
while(1)
{
strcpy(buf2,id);
send(netsocket, buf2, 100, 0);
FILE *file;
file= fopen ("Credentials.txt","r");

if (file != NULL) 
{
   char line [1000];
   while(fgets(line,sizeof line,file)!= NULL && flag!=1) 
   {
     if(strcmp(login,line)==0)
     {	
       flag=1; 
     }
     else
     {	
       flag=0;
     }
   } 
   fclose(file);
	
   if(flag==1)
   {
      if(login[0]=='5')
      {
         printf("\nEnter a choice: ");
         printf("\n1- Get notes file from server (get)");
         printf("\n2- Put assignment file in server (put)"); 
         printf("\n3- Quit \n");  
        printf("Client: ");
    scanf("%d", &choice);
    switch(choice)
    {    
	 case 1:
	 printf("Enter filename to get: ");
	 scanf("%s", filename);
	 strcpy(buf, "get ");
	 strcat(buf, filename);
	 send(netsocket, buf, 100, 0); //send response
	 recv(netsocket, &size, sizeof(int), 0); //recieve response from server
	 if(!size)
	 {
	    printf("No such file on the remote directory\n\n");
	    break;
	 }
	 f = malloc(size);
	 recv(netsocket, f, size, 0);
	 while(1)
	 {
	    filehandle = open(filename, O_CREAT | O_EXCL | O_WRONLY, 0666);
	    if(filehandle == -1)
	    {
		sprintf(filename + strlen(filename), "%d"); 
		//needed only if same directory is used for both server and client
	    }
	    else break;
	 }
	 write(filehandle, f, size);
	 close(filehandle);
	 strcpy(buf, "cat ");
	 strcat(buf, filename);
	 system(buf);
	 break;
	 case 2:
	 printf("Enter filename to put to server: ");
         scanf("%s", filename);
	 filehandle = open(filename, O_RDONLY);
         if(filehandle == -1)
         {
            printf("No such file on the local directory\n\n");
            break;
         }
         strcpy(buf, "put ");
	 strcat(buf, filename);
	 send(netsocket, buf, 100, 0);
	 stat(filename, &obj);
	 size = obj.st_size;
	 send(netsocket, &size, sizeof(int), 0);
	 sendfile(netsocket, filehandle, NULL, size);
	 recv(netsocket, &status, sizeof(int), 0);
	 if(status)
	   printf("File stored successfully\n");
	 else
	   printf("File failed to be stored to remote machine\n");
	 break;
	 case 3:
	 strcpy(buf, "quit");
         send(netsocket, buf, 100, 0);
         recv(netsocket, &status, 100, 0);
	 if(status)
	 {
	    printf("Server closed\n");
	    printf("%s Quitting....\n", buf2);
	    exit(0);
	 }
	 printf("Server failed to close connection\n");
	 break;
         default: printf("Wrong choice! Try again.");
   }
}
else
{
     printf("\nEnter a choice: ");
     printf("\n1- Get assignment file from server (get)");
     printf("\n2- Put notes file in server (put)");
     printf("\n3- Show all the files in the server (ls)");
     printf("\n4- Change directory (cd)");
     printf("\n5- Quit \n");
    printf("Client: ");
    scanf("%d", &choice);
    switch(choice)
    {    
	 case 1:
	 printf("Enter filename to get: ");
	 scanf("%s", filename);
	 strcpy(buf, "get ");
	 strcat(buf, filename);
	 send(netsocket, buf, 100, 0); //send response
	 recv(netsocket, &size, sizeof(int), 0); //recieve response from server
	 if(!size)
	 {
	    printf("No such file on the remote directory\n\n");
	    break;
	 }
	 f = malloc(size);
	 recv(netsocket, f, size, 0);
	 while(1)
	 {
	    filehandle = open(filename, O_CREAT | O_EXCL | O_WRONLY, 0666);
	    if(filehandle == -1)
	    {
		sprintf(filename + strlen(filename), "%d"); 
		//needed only if same directory is used for both server and client
	    }
	    else break;
	 }
	 write(filehandle, f, size);
	 close(filehandle);
	 strcpy(buf, "cat ");
	 strcat(buf, filename);
	 system(buf);
	 break;
	 case 2:
	 printf("Enter filename to put to server: ");
         scanf("%s", filename);
	 filehandle = open(filename, O_RDONLY);
         if(filehandle == -1)
         {
            printf("No such file on the local directory\n\n");
            break;
         }
         strcpy(buf, "put ");
	 strcat(buf, filename);
	 send(netsocket, buf, 100, 0);
	 stat(filename, &obj);
	 size = obj.st_size;
	 send(netsocket, &size, sizeof(int), 0);
	 sendfile(netsocket, filehandle, NULL, size);
	 recv(netsocket, &status, sizeof(int), 0);
	 if(status)
	   printf("File stored successfully\n");
	 else
	   printf("File failed to be stored to remote machine\n");
	 break;
	 case 3:
	 strcpy(buf, "ls");
         send(netsocket, buf, 100, 0);
	 recv(netsocket, &size, sizeof(int), 0);
         f = malloc(size);
         recv(netsocket, f, size, 0);
	 filehandle = creat("temp.txt", O_WRONLY);
	 write(filehandle, f, size);
	 close(filehandle);
         printf("The remote directory listing is as follows:\n");
	 system("cat temp.txt");
	 break;
         case 4:
	 strcpy(buf, "cd ");
	 printf("Enter the path to change the remote directory: ");
	 scanf("%s", buf + 3);
         send(netsocket, buf, 100, 0);
	 recv(netsocket, &status, sizeof(int), 0);
         if(status)
           printf("Remote directory successfully changed\n");
         else
           printf("Remote directory failed to change\n");
         break;
	 case 5:
	 strcpy(buf, "quit");
         send(netsocket, buf, 100, 0);
         recv(netsocket, &status, 100, 0);
	 if(status)
	 {
	    printf("Server closed\n");
	    printf("%s Quitting....\n", buf2);
	    exit(0);
	 }
	 printf("Server failed to close connection\n");
 	 break;
 	 default: printf("Wrong choice! Try again.");
      }
  }
}
else
printf("invalid");
}
else 
{
  perror("Invalid");
}
}
close(netsocket);
return 0;
}
