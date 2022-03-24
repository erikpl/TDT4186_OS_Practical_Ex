#include<stdio.h>
#include<stdlib.h>
#include<string.h>	
#include<sys/socket.h>
#include <arpa/inet.h>	
#include<unistd.h>	
#include <errno.h>
#include <sys/stat.h>
#include <ctype.h>
#include <fcntl.h>
#include <unistd.h>
#include "bbuffer.h"
#include <pthread.h>

#define BUFFER_SIZE 20000
#define SIZE 1024

char *WWW_PATH;
int PORT;
int BBUFFER_SLOTS;
int THREADS;



// Function that reads from a file and returns a string with the file contents
char * read_from_file(char *filename) {

	// Open the file to read from
	int in_file = open(filename, O_RDONLY);

    // Stat is a structure that is defined to store information about the file 
    struct stat sb;
    stat(filename, &sb);

    // Creates a read-only string with the same size of the file
    char *file_contents = malloc(sb.st_size);

	// Reads the file contents
	read(in_file, file_contents, sb.st_size);

	// Returns the file contents as a char * 
	return file_contents;

	close(in_file);
	
}

// Slice string and assign to buffer. From start to end.
void slice_str(const char * str, char * buffer, size_t start, size_t end) {
    size_t j = 0;
    for ( size_t i = start; i <= end; ++i ) {
        buffer[j++] = str[i];
    }
    buffer[j] = 0;
}

void* assign_request(void *data) {

	int *client_socket_ptr = data;
	int client_socket = *client_socket_ptr;

	puts("New transmission started.");

	// Recieve an incoming request
	char buf[1000];
	int recc = recv(client_socket, buf, sizeof buf, 0);

	// TODO Dette kan være en egen funksjon. Ikke prioritet nå.
	// Define the token, comparison string and the desired path end. The path end is after the "GET" part of the request, which we are searching for.
	char *token;
	char *comp_str = "GET";
	char *path_end = NULL;
	char *correct_line;
	char *tmp_token = NULL;
	char *correct_line_elem = NULL;
	int count = 0;

	// Split the request by line
	token = strtok(buf, "\n");
	while( token != NULL ) {
		
		// Check if the beginning of the line is "GET"
		char token_comp_str[strlen(comp_str)+1];
		slice_str(token, token_comp_str, 0, 2);

		// If "GET", search for the requests route.
		if (strncmp(token_comp_str, comp_str, strlen(token_comp_str)) == 0) {
			
			// Get copy. Must be done to get the value of current line instead of entire request.
			tmp_token = token;

			// Iterate through every "word" of the "GET" line of the request
			correct_line =  strtok(tmp_token, " ");
			while(correct_line != NULL) {
				
				// The requested file is always the second word.
				if(count == 1) {
					
					// Finally assign path end.
					path_end = correct_line;
				}
				// Continue iteration.
				correct_line = strtok(NULL, " ");
				count++;
			}
		}
		// ITerate through new line
		token = strtok(NULL, "\n");
	}
	// TODO Dette kan være en egen funksjon. Ikke prioritet nå.

	// Copy server path. 
	char full_path[200];
	strcpy(full_path, WWW_PATH);
	
	// Concat server and client path to get full path
	strcat(full_path, path_end);


	// Check if the file exists
	if( access( full_path, F_OK ) == 0 ) {

		// Print file served
		printf("Serving html file: %s\n", full_path);			
		//Open the file to read from
		char * file_contents = read_from_file(full_path);

		//Creates a buffer with the headers to be sent
		char msg[BUFFER_SIZE];
		snprintf(msg, sizeof(msg),
		"HTTP/1.0 200 OK\n"
		"Content-Type: text/html\n"
		"Content-Lenght: %ld\n\n%s", strlen(file_contents), file_contents); 
		write(client_socket, msg, strlen(msg));
		
	} else {
		puts("File does not exists.");
	}

	//Close the client socket
	close(client_socket);
	// Reset the request
	memset(buf, 0, sizeof(buf));
	puts("Socket connection closed.\n");
}


// TODO: Legge til håndtering og bruk av command line arguments
/* Called using ./mtwwwd www_path port #threads #bufferslots */
int main(int argc , char * argv[]) {
	int server_socket, client_socket, c, read_size, n;
	struct sockaddr_in server , client;
	char client_message[BUFFER_SIZE];
	long numbytes;
	char source[600];

	// Handling WWW_PATH from the command line 
	if (argv[1]) {
		WWW_PATH = argv[1];
		printf("Serving the path %s\n", WWW_PATH);
	}
	else {
		fprintf(stderr, "Did not find WWW_PATH.\nTry calling again using ./mtwwwd www_path port #threads #bufferslots\n");
		exit(EXIT_FAILURE);
	}

	// Handling PORT from the command_line. 
	if (argv[2]) {
		PORT = atoi(argv[2]);
		printf("Accepting connection on port number %d\n", PORT);
	}
	else {
		fprintf(stderr, "Did not find PORT.\nTry calling again using ./mtwwwd www_path port #threads #bufferslots\n");
		exit(EXIT_FAILURE);
	}

	if (argv[3]) {
		THREADS = atoi(argv[3]);
		printf("Running the server with %d threads.\n", THREADS);
	}

	else {
		fprintf(stderr, "Was not able to retrieve the desired number of threads.\nTry calling again using ./mtwwwd www_path port #threads #bufferslots\n");
		exit(EXIT_FAILURE);
	}

	if (argv[4]) {
		BBUFFER_SLOTS = atoi(argv[4]);
		printf("Running the connection with a queue size of %d.\n", BBUFFER_SLOTS);
	}
	else {
		fprintf(stderr, "Was not able to retrieve the desired number of buffer slots.\nTry calling again using ./mtwwwd www_path port #threads #bufferslots\n");
		exit(EXIT_FAILURE);
	}

	// Create the socket
	server_socket = socket(AF_INET, SOCK_STREAM, 0);

	// Checking if socket was successfully created
	if (server_socket == -1) {
		perror("Could not create socket");
		exit(EXIT_FAILURE);
	}

	// Resetting socket timeout, letting us reuse a port.
	int yes = 1;
	if (setsockopt(server_socket, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes))) {
		perror("setsockopt(SO_REUSEADDR) failed");
		exit(EXIT_FAILURE);
	}
		
	puts("Socket created");
	//Initializing the sockaddr_in structure
	server.sin_family = AF_INET;
	server.sin_port = htons(PORT);
	server.sin_addr.s_addr = INADDR_ANY;

	/* BBUFFER */
	struct BNDBUF *request_buffer = bb_init(BBUFFER_SLOTS);

	/* THREADS */
	pthread_t workers[THREADS];
	for (int i = 0; i < THREADS; i++) {
		pthread_create(&workers[i], NULL, assign_request, request_buffer);
	}

	// TODO Her er det vi ønsker å legge til multithreading
	// while (1) {

		// Assinging an address to the socket using bind		
		if(bind(server_socket, (struct sockaddr *)&server, sizeof(server)) < 0) {
			perror("Bind failed. Error");
			exit(EXIT_FAILURE);
		}

		puts("Bind successfull");
		
		// Prepares the socket for connection requests
		listen(server_socket, BBUFFER_SLOTS);
		
		printf("Running server with %d workers and %d buffer slots.\n\n",THREADS, BBUFFER_SLOTS );
		
		while (1) {
			
			puts("Waiting for incoming connections...\n");
			
			// Accept a connection from an incoming client. Does not proceeed from here before api-call.
			client_socket = accept(server_socket, (struct sockaddr *)&client, (socklen_t*)&c);
			
			// Pass client socket into ring buffer

			assign_request((void*) &client_socket);
		}
	// }
}

