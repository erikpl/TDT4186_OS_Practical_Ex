#include<stdio.h>
#include<stdlib.h>
#include<string.h>	
#include<sys/socket.h>
#include<arpa/inet.h>	
#include<unistd.h>	
#include <errno.h>
#include <sys/stat.h>
#include <fcntl.h>


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
		char test[20];
		test[0] = buf[i];

		if (strstr(test, "/") != NULL) {
			read_start = 1;
			memset(test, 0, 20);
		}
		
		// TODO endre denne til å sjekke for "/" i stedet for 0. Da kan vi lese et filnavn. Bytte denne kodeblokken med // write to char array så blir det riktig

		// Stop reading when finding a space
		if (isalpha(buf[i]) == 0 && read_start == 1) {
			printf("i %d read start %d, char %d ", i, read_start, isalpha(buf[i]));
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
void working_socket(){
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

char * read_from_file(char *filename) {

	//Open the file to read from
	int in_file = open(filename, O_RDONLY);

    // Stat is a structure that is defined to store information about the file 
    struct stat sb;
    stat(filename, &sb);

    // Creates a read-only string with the same size of the file
    char *file_contents = malloc(sb.st_size);

	//Reads the file contents
	read(in_file, file_contents, sb.st_size);

	return file_contents;
}


int main(int argc , char *argv[]) {


	puts(read_from_file("index.html"));

}
	

