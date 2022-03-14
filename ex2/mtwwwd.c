#include <stdio.h>
#include <stdlib.h>
#include <string.h>	
#include <sys/socket.h>
#include <arpa/inet.h>	
#include <unistd.h>	
#include <errno.h>

#define BUFFER_SIZE 20000
#define SIZE 1024
#define CONNECTION_QUEUE_LIMIT 5

/* Called using ./mtwwwd www_path port #threads #bufferslots */
/* TODO: Hente ut request_path fra HTTP GET Request-en */
int main(int argc , char *argv[]) {
	/* Handling WWW_PATH from the command line */
	/* TODO: validering av path, spesielt mtp. trailing slash, burde legges til her.
	*  I tillegg er det viktig å huske at Unix og Windows bruker ulike formater 
	*/
	if (argv[1]) {
		char* WWW_PATH = (char*) NULL;
		int www_path_len = strlen(argv[1]);
		bzero(WWW_PATH, www_path_len + 1);
		strncpy(WWW_PATH, argv[1], www_path_len);
		printf("Serving the path %s\n", WWW_PATH);
	}
	else {
		fprintf(stderr, "Did not find WWW_PATH.\nTry calling again using ./mtwwwd www_path port #threads #bufferslots\n");
		exit(EXIT_FAILURE);
	}

	/* Handling PORT from the command_line. */
	int PORT;
	if (argv[2]) {
		// PORT should, admittedly, be an unsigned type, but this will suffice
		PORT = (int) strtol(argv[2], NULL, 10);
		printf("Accepting connection on port number %d", PORT);
	}
	else {
		fprintf(stderr, "Did not find PORT.\nTry calling again using ./mtwwwd www_path port #threads #bufferslots\n");
		exit(EXIT_FAILURE);
	}

	// Create the socket
	int socket_desc = socket(AF_INET, SOCK_STREAM, 0);
	// Checking if socket was successfully created
	if (socket_desc == -1) {
		printf("Could not create socket");
		exit(EXIT_FAILURE);
	}
	puts("Socket created");

	struct sockaddr_in server;

	// Initializing the sockaddr_in structure
	server.sin_family = AF_INET;
	server.sin_port = htons(PORT);
	server.sin_addr.s_addr = INADDR_ANY;

	// Assinging an address to the socket using bind
	if (bind(socket_desc, (struct sockaddr *)&server, sizeof(server)) < 0) {
		perror("Bind failed. Error");
		exit(EXIT_FAILURE);
	}

	puts("Bind successfull");

	// Prepares the socket for connection requests
	listen(socket_desc, CONNECTION_QUEUE_LIMIT);

	/* TODO: Legg til håndtering av #threads og #bufferslots */
	while (1) {
		int client_socket, read_size, n;
		struct sockaddr_in client;
		char client_message[BUFFER_SIZE];
		// Make sure the contents of client_message are empty before proceeding
		bzero(client_message, BUFFER_SIZE + 1);

		long numbytes;
		
		puts("Waiting for incoming connections...");
		int socket_size = sizeof(struct sockaddr_in);
		// Accept a connection from an incoming client
		client_socket = accept(socket_desc, (struct sockaddr *)&client, (socklen_t*)&socket_size);

		// Checks if client socket was created successfully
		if (client_socket < 0) {
			perror("Error on accept");
		}
		puts("Connection accepted");

		// TODO: Må vi free source siden den ikke er dynamisk allokert?
		char *source = NULL;
		FILE *file_pointer = fopen("/index.html", "r");

		/* TODO: Finne ut hvordan vi skal hente ut request_path */
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
					fprintf(stderr, "Error reading file");
				} 
				else {
					source[new_length + 1] = '\0'; /* Just to be safe. */
				}
			}
			fclose(file_pointer);
		}
		free(source);

		// Read data from client
		n = read(client_socket, client_message, sizeof(client_message)-1);

		if (n < 0) {
			puts("Error reading from client socket");
		}
		
		// Receive a message from client
		while ((read_size = recv(client_socket , client_message , 2000 , 0)) > 0 ) {
				n = write(client_socket, source, strlen(source));
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
	}
	return 0;
}