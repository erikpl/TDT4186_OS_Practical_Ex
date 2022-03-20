#include<stdio.h>
#include<stdlib.h>
#include<string.h>	
#include<sys/socket.h>
#include<arpa/inet.h>	
#include<unistd.h>	
#include <errno.h>
#include <sys/stat.h>


int main(int argc , char *argv[]) {

FILE* fp;
const char* filename = "index.html";
int socket_desc , client_sock , c , read_size;
struct sockaddr_in server , client;


//Create socket
	socket_desc = socket(AF_INET , SOCK_STREAM , 0);
	if (socket_desc == -1)
	{
		printf("Could not create socket");
	}
	puts("Socket created");
	
	//Prepare the sockaddr_in structure
	server.sin_family = AF_INET;
	server.sin_addr.s_addr = INADDR_ANY;
	server.sin_port = htons( 8182 );
	
	//Bind
	if( bind(socket_desc,(struct sockaddr *)&server , sizeof(server)) < 0)
	{
		//print the error message
		perror("bind failed. Error");

	}
	puts("bind done");
	
	//Listen
	listen(socket_desc , 3);
	
	//Accept and incoming connection
	puts("Waiting for incoming connections...");
	c = sizeof(struct sockaddr_in);
	
	//accept connection from an incoming client
	client_sock = accept(socket_desc, (struct sockaddr *)&client, (socklen_t*)&c);
	if (client_sock < 0)
	{
		perror("accept failed");
	
	}
	puts("Connection accepted");


    //Open the file to read from
	FILE *in_file = fopen(filename, "r");

    // Stat is a structure that is defined to store information about the file 
    struct stat sb;
    stat(filename, &sb);

    // Creates a read-only string with the same size of the file
    char *file_contents = malloc(sb.st_size);

    // While the file is not empty, write every line to the socket
	while (fgets(file_contents, sb.st_size, in_file)!=NULL) {
       write(client_sock, file_contents, strlen(file_contents));
    }

    // Close the file after use
    fclose(in_file);

	
	if(read_size == 0)
	{
		puts("Client disconnected");
		fflush(stdout);
	}
	else if(read_size == -1)
	{
		perror("recv failed");
	}
	

}
	

