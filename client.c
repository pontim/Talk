// Mark Ponti
// IPC Socket Talk
// This program connects to a server and then procees to
// initiate a chat protocol between the client and server.

#include <stdio.h>
#include <winsock2.h>
#include <Windows.h>
#include <string.h>
#include <conio.h>

int main(int argc, char *argv[])
{
	//variable declarations
	WORD wVersionRequested;
	WSADATA wsaData;
	int errornumber, result;
	unsigned long on = 1;
	char path[128] = "";
	char test[512] = "";
	char bucket[512]= "";
	LPHOSTENT host;
	SOCKADDR_IN theSrvr;
	SOCKET ConnSkt;
	

	wVersionRequested = MAKEWORD(2,2);    //version 2.0
	
	// initilizing winsock
	if(WSAStartup(wVersionRequested, &wsaData) != 0)
	{
		fprintf(stderr, "Process failed on WinSock startup \n");
		ExitProcess(0);
	}

	if(argc == 1)
	{
		printf("#######################################################\n");
		printf("TALK-CLIENT: Mark's Version\n");
		printf("#######################################################\n\n");
		printf("Usage: client <port no.> [<server_host>] \n(Must start Server first to get port no.)\n");
		WSACleanup();
		ExitProcess(0);
	}

	// if 3 arguments use user's server otherwise get localhost
	if(argc == 3)
	{
		strcpy(path,argv[2]);
	}
	else
	{
		gethostname(path,sizeof(path));
	}

	// gets host information
	host = gethostbyname(path);

	//declares the socket
	ConnSkt = socket(AF_INET, SOCK_STREAM, 0);

	// set socket attributes
	ZeroMemory(&theSrvr, sizeof(SOCKADDR_IN)); 
	theSrvr.sin_family = AF_INET; 
	theSrvr.sin_port = htons((atoi(argv[1]))); // port # 
	CopyMemory(&theSrvr.sin_addr, // Host 
	host->h_addr_list[0], host->h_length);

	// connect to specified port
	result = 
	connect(ConnSkt, (const SOCKADDR *) &theSrvr, sizeof(theSrvr));

	// if the connection fails exit the program
	if(result != 0)
	{
		int errornumber = GetLastError(); 
		printf("%d\n",errornumber);
		printf("ERROR!\n");
		WSACleanup();
		ExitProcess(0);
	}

	// IO mode of a socket
	ioctlsocket(ConnSkt, FIONBIO, &on );

	printf("#######################################################\n");
	printf("TALK-CLIENT: Mark's Version\n");
	printf("#######################################################\n");
	printf("Server host --------> ");
	printf("%s\n",path);
	printf("Server port number -> ");
	printf("%d\n",argv[1]);
	printf("Making connection request ... connected\n");
	printf(">");

	while(TRUE)
	{

		// if recieved connection drop any newlines
		if(recv(ConnSkt, test, 512, 0) > 0)
		{
			printf("\n%s","<");
			printf("%s",strtok(test,"\n"));
			printf("%s","\n\n>");
		}

		// if recieved message
		if(strcmp(test,"exit") == 0)
		{
			break;
		}

		// test for keyboard input
		if(_kbhit() != (0))
		{
			fgets(test,512,stdin);
			strcpy(bucket,test);

			// if user hits enter return to prompt
			if(strlen(test) <= 1)
			{
				printf(">");
			}
			else
			{
				// if input = exit send exit and break loop
				if(strcmp(strtok(test,"\n"),"exit") == 0)
				{
					send(ConnSkt, test, strlen(test),0);
					break;
				}

				send(ConnSkt, bucket, strlen(bucket),0);
				printf("%s",">");
			}
			
		}
		//clear array to prevent overwriting null terminator
		memset(test,'\0',512);
		Sleep(250);
	}


	closesocket(ConnSkt); 
	WSACleanup();

	return(0);
}