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

#define BUFFER_SIZE 20000
#define SIZE 1024
#define CONNECTION_QUEUE_LIMIT 5


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


// Function to parse and return the data from a request
const char* request_string(char *buf, int size) {
	int read_start = 0;
	int read_stop = 0;
	int data_index = 0;
	char data[512];
	int i = size;

	// Loop through request
	for (i; i > -1 ; i--) {
		
		// When first finding a char, start writing
		if (isalpha(buf[i])) {
			read_start = 1;
		}
		
		// TODO endre denne til å sjekke for "/" i stedet for 0. Da kan vi lese et filnavn. Bytte denne kodeblokken med // write to char array så blir det riktig
		// Stop reading when finding a space
		if (isalpha(buf[i]) == 0 && read_start == 1) {
			printf("i %d read start%d, char %d ", i, read_start, isalpha(buf[i]));
			read_stop = 1;
		}
		
		// write to char array
		if ((read_start == 1) && (read_stop == 0)) {
			data[data_index] = buf[i];
			data_index++;
		}

		// stop reading
		if ((read_start == 1) && (read_stop == 1)) {
			break;
		}
	}

	// Reverse string
	char string_done[500];
	for(i=0; i < strlen(data); ++i) {
			string_done[strlen(data) -i-1] = data[i];
		}
	
	// Letting our string be returned
	const char * res = string_done;
	return res;
}

// TODO: Legge til håndtering og bruk av command line arguments
/* Called using ./mtwwwd www_path port #threads #bufferslots */
int main(int argc , char * argv[]) {
	char *WWW_PATH;
	int server_socket, client_socket, c, read_size, n;
	struct sockaddr_in server , client;
	char client_message[BUFFER_SIZE];
	long numbytes;
	char source[600];

	// TODO test the path

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
	int PORT;
	if (argv[2]) {
		PORT = atoi(argv[2]);
		printf("Accepting connection on port number %d\n", PORT);
	}
	else {
		fprintf(stderr, "Did not find PORT.\nTry calling again using ./mtwwwd www_path port #threads #bufferslots\n");
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

	// TODO Her er det vi ønsker å legge til multithreading
	while (1) {

		//Initializing the sockaddr_in structure
		server.sin_family = AF_INET;
		server.sin_port = htons(PORT);
		server.sin_addr.s_addr = INADDR_ANY;

		// Assinging an address to the socket using bind		
		if(bind(server_socket, (struct sockaddr *)&server, sizeof(server)) < 0) {
			perror("Bind failed. Error");
			exit(EXIT_FAILURE);
		}

		puts("Bind successfull");
		
		// Prepares the socket for connection requests
		listen(server_socket, CONNECTION_QUEUE_LIMIT);
		
		puts("Waiting for incoming connections...");

		// Denne burde vi virkelig kalle noe annet
		c = sizeof(struct sockaddr_in);
		
		// Accept a connection from an incoming client. Does not proceeed from here before api-call.
		client_socket = accept(server_socket, (struct sockaddr *)&client, (socklen_t*)&c);
			
		puts("New transmission started.");

		// Recieve an incoming request
		char buf[512];
 	
    	int recc = recv(client_socket, buf, sizeof buf, 0);
		char *s;

 		s = strstr(buf, "index");      // search for string "hassasin" in buff
 		if (s != NULL)                     // if successful then s now points at "hassasin"
		{
     	printf("Found string at index = %ld\n", s - buf);
 		}                                  // index of "hassasin" in buff can be found by pointer subtraction
 		else{
     	printf("String not found\n");  // `strstr` returns NULL if search string not found
		}

		// TODO kommentert ut fordi det er en feil i pointers. Må fikses.

		// Parsing and fixing the request to get desired file
	 	const char* file_path = request_string(buf, sizeof buf);
			 
		char* total_path = strcat(WWW_PATH,  file_path);
		char* total_path_ext = strcat(total_path, ".html\n");
			 
		//Bare for å sjekke hva som printes
		puts("\n");
		printf("%s", buf);
		puts("\n");
		puts(file_path);
		puts("\n");
		puts(total_path);
		puts("\n");
		puts(total_path_ext);  
			

		//Open the file to read from
		char * file_contents = read_from_file(WWW_PATH);

		//Creates a buffer with the headers to be sent
		char msg[BUFFER_SIZE];
		snprintf(msg, sizeof(msg),
		"HTTP/1.0 200 OK\n"
		"Content-Type: text/html\n"
		"Content-Lenght: %ld\n\n%s", strlen(file_contents), file_contents); 
		write(client_socket, msg, strlen(msg));
		}
		
		//Close the client socket
		close(client_socket);

		puts("Transmission finished. Socket closed.");
		}


