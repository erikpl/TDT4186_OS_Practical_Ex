#include<stdio.h>
#include<stdlib.h>
#include<string.h>	
#include<sys/socket.h>
#include<arpa/inet.h>	
#include<unistd.h>	
#include <errno.h>

#define BUFFER_SIZE 2000
#define SIZE 1024
#define PORT 8111

// TODO: Legge til connection loop
int main(int argc , char *argv[])
{
	int socket_desc, client_socket, c, read_size, n;
	struct sockaddr_in server , client;
	char client_message[BUFFER_SIZE];

	long numbytes;

	char *source = NULL;
	FILE *file_pointer = fopen("/index.html", "r");

	if (file_pointer != NULL) {
		/* Go to the end of the file. */
		if (fseek(file_pointer, 0L, SEEK_END) == 0) {
			/* Get the size of the file. */
			long bufsize = ftell(file_pointer);
			if (bufsize == -1) { /* Error */ }

			/* Allocate our buffer to that size. */
			source = malloc(sizeof(char) * (bufsize + 1));

			/* Go back to the start of the file. */
			if (fseek(file_pointer, 0L, SEEK_SET) != 0) { /* Error */ }

			/* Read the entire file into memory. */
			size_t new_length = fread(source, sizeof(char), bufsize, file_pointer);

			if (ferror(file_pointer) != 0) {
				file_pointeruts("Error reading file", stderr);
			} 
			else {
				source[new_length++] = '\0'; /* Just to be safe. */
			}
		}
		fclose(file_pointer);
	}
	free(source);

	//Create the socket
	socket_desc = socket(AF_INET, SOCK_STREAM, 0);

	//Checking if socket was successfully created
	if (socket_desc == -1) {
		printf("Could not create socket");
	}

	puts("Socket created");

	//Initializing the sockaddr_in structure
	server.sin_family = AF_INET;
	server.sin_port = htons(PORT);
	server.sin_addr.s_addr = INADDR_ANY;
	
	//Assinging an address to the socket using bind
	if(bind(socket_desc, (struct sockaddr *)&server, sizeof(server)) < 0) {
		perror("Bind failed. Error");
		exit(EXIT_FAILURE);
	}

	puts("Bind successfull");
	
	//Prepares the socket for connection requests
	listen(socket_desc, 5);
	
	puts("Waiting for incoming connections...");


	c = sizeof(struct sockaddr_in);
	
	//Accept a connection from an incoming client
	client_socket = accept(socket_desc, (struct sockaddr *)&client, (socklen_t*)&c);

	//Checks if client socket was created successfully
	if (client_socket < 0) {
		perror("Error on accept");
		exit(EXIT_FAILURE);
	}
	puts("Connection accepted");


	//Read data from client
	n = read (client_socket, client_message, sizeof(client_message)-1);

	if (n < 0) {
		puts("Error reading from client socket");
	}
	
	//Receive a message from client
	while ((read_size = recv(client_socket , client_message , 2000 , 0)) > 0 ) {
			 n = write (client_socket, source, strlen(source));
	}
	
	if (read_size == 0) {
		puts("Client disconnected");
		fflush(stdout);
	}

	else if  (read_size == -1){
		perror("recv failed");
	}
	
	//Closes the client socket
	close(client_socket);

	// Loopen burde vel slutte sånn ca. her
	exit(EXIT_SUCCESS);
}