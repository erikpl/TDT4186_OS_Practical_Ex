#include<stdio.h>
#include<stdlib.h>
#include<string.h>	
#include<sys/socket.h>
#include<arpa/inet.h>	
#include<unistd.h>	
#include <errno.h>
#include <sys/stat.h>

#define BUFFER_SIZE 20000
#define SIZE 1024
#define CONNECTION_QUEUE_LIMIT 5

// TODO: Legge til håndtering og bruk av command line arguments
/* Called using ./mtwwwd www_path port #threads #bufferslots */
int main(int argc , char *argv[]) {
	char *WWW_PATH;
	int socket_desc, client_socket, c, read_size, n;
	struct sockaddr_in server , client;
	char client_message[BUFFER_SIZE];
	long numbytes;
	char source[500];

	/* Handling WWW_PATH from the command line */
	if (argv[1]) {
		WWW_PATH = argv[1];
		printf("Serving the path %s\n", WWW_PATH);
	}
	else {
		fprintf(stderr, "Did not find WWW_PATH.\nTry calling again using ./mtwwwd www_path port #threads #bufferslots\n");
		exit(EXIT_FAILURE);
	}

	/* Handling PORT from the command_line. */

	int PORT;
	if (argv[2]) {
		PORT = atoi(argv[2]);
		printf("Accepting connection on port number %d", PORT);
	}
	else {
		fprintf(stderr, "Did not find PORT.\nTry calling again using ./mtwwwd www_path port #threads #bufferslots\n");
		exit(EXIT_FAILURE);
	}

	while (1) {
	

		//Open the file to read from
		FILE *in_file = fopen("index.html", "r");

		// Stat is a structure that is defined to store information about the file 
		struct stat sb;
		stat("index.html", &sb);

		// Creates a read-only string with the same size of the file
		char *file_contents = malloc(sb.st_size);

	
		// Create the socket
		socket_desc = socket(AF_INET, SOCK_STREAM, 0);

		// Checking if socket was successfully created
		if (socket_desc == -1) {
			printf("Could not create socket");
			exit(EXIT_FAILURE);
		}

		puts("Socket created");

		//Initializing the sockaddr_in structure
		server.sin_family = AF_INET;
		server.sin_port = htons(8182);
		server.sin_addr.s_addr = INADDR_ANY;
		
		//Assinging an address to the socket using bind
		if(bind(socket_desc, (struct sockaddr *)&server, sizeof(server)) < 0) {
			perror("Bind failed. Error");
			exit(EXIT_FAILURE);
		}

		puts("Bind successfull");
		
		//Prepares the socket for connection requests
		listen(socket_desc, CONNECTION_QUEUE_LIMIT);
		
		puts("Waiting for incoming connections...");

		// Denne burde vi virkelig kalle noe annet
		c = sizeof(struct sockaddr_in);
		
		// Accept a connection from an incoming client
		client_socket = accept(socket_desc, (struct sockaddr *)&client, (socklen_t*)&c);

		// While the file is not empty, write every line to the socket
		while (fgets(file_contents, sb.st_size, in_file)!=NULL) {
		write(client_socket, file_contents, strlen(file_contents));
		}

		// Close the file after use
		fclose(in_file);
			}

		// Checks if client socket was created successfully
		if (client_socket < 0) {
			perror("Error on accept");
		}
		puts("Connection accepted");


		// Read data from client
		n = read (client_socket, client_message, sizeof(client_message)-1);

		if (n < 0) {
			puts("Error reading from client socket");
		}
		
		//Receive a message from client
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

