// Mark Ponti
// IPC Socket Talk
// This program listens for a connection over the port it is bound to and then 
// initiates a chat protocol between the client and server.

#include <stdio.h>
#include <winsock2.h>
#include <Windows.h>
#include <string.h>
#include <conio.h>

int main(int argc, char *argv[])
{
	// variable declaration
	WORD wVersionRequested;
	WSADATA wsaData;
	int errornumber, result, len, ClientLen;
	unsigned long on = 1;
	u_short port = 0;
	char path[128] = "";
	char test[512] = "";
	char bucket[512] = "";
	LPHOSTENT host;
	SOCKADDR_IN theSrvr;
	SOCKADDR_IN Client;
	SOCKET ConnSkt;
	

	wVersionRequested = MAKEWORD(2,2);    //version 2.0

	// initilizing winsock
	if(WSAStartup(wVersionRequested, &wsaData) != 0)
	{
		fprintf(stderr, "Process failed on WinSock startup \n");
		ExitProcess(0);
	}

	// if 2 arguments use user's server otherwise get localhost
	if(argc == 2)
	{
		strcpy(path,argv[1]);
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
	theSrvr.sin_port = htons((u_short) port); // port # 
	CopyMemory(&theSrvr.sin_addr, // Host 
	host->h_addr_list[0], host->h_length);

	// bind to an open port
	result = 
	bind(ConnSkt, (const SOCKADDR *) &theSrvr, sizeof(theSrvr));

	// if bind fails kick back the error and exit
	if(result != 0)
	{
		int errornumber = GetLastError(); 
		printf("Connection error: ");
		printf("%d\n",errornumber);
		WSACleanup();
		closesocket(ConnSkt);
		ExitProcess(0);
	}


	len = sizeof(theSrvr);
	// retrieves the local name for the socket
	getsockname(ConnSkt, (SOCKADDR *) &theSrvr, &len ); 

	// converts u_short to TCP/IP network byte order to host byte order
	port = ntohs(theSrvr.sin_port);

	// server information
	printf("#######################################################\n");
	printf("TALK-SERVER: Mark's Version\n");
	printf("#######################################################\n");
	printf("Server host --------> ");
	printf("%s\n",path);
	printf("Server port number -> ");
	printf("%d\n",port);
	printf("Listening...");
	listen(ConnSkt, SOMAXCONN); //listen for connection

	ClientLen = sizeof(SOCKADDR_IN);
	ConnSkt = accept(ConnSkt, (SOCKADDR *) &Client, &ClientLen); // accept incoming connection
	printf(" accepted connection request\n");

	// IO mode of a socket
	ioctlsocket(ConnSkt, FIONBIO, &on );

	
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

	// close socket and cleanup
	closesocket(ConnSkt);
	WSACleanup();

	return(0);
}