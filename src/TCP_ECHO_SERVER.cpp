/*
TCP Echo server example in winsock
Live Server on port 8888
*/
#include "stdafx.h"
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include<stdio.h>
#include<winsock2.h>

#pragma comment(lib, "ws2_32.lib") //Winsock Library

#define n 16
//sarah 0322 begin
typedef struct device {
	char ID; //0~15 device; 254 register ;253 hostID
	IN_ADDR IP;
	char action;//on/off/status
	int the_s;
};
//sarah 0322 end
struct device scratch[n];
struct device devices[n];
int scratch_num = 0;
int device_num = 0;


// ¨ú±o sockaddr¡AIPv4 ©Î IPv6¡G
void *get_in_addr(struct sockaddr *sa)
{
	if (sa->sa_family == AF_INET) {
		return &(((struct sockaddr_in*)sa)->sin_addr);
	}

	//return & (((struct sockaddr_in6*)sa)->sin6_addr);
}

int main(int argc, char *argv[])
{
	WSADATA wsa;
	SOCKET master, new_socket, client_socket[30], s, x;//sarah 0404 «Å§i?
	struct sockaddr_in server, address;
	int max_clients = 30, activity, addrlen, i, valread, client_num = 0;
	//char *message = "ECHO Daemon v1.0 \r\n";

	//size of our receive buffer, this is string length.
	int MAXRECV = 1024;
	//set of socket descriptors
	fd_set readfds;
	//1 extra for null character, string termination
	char const *msgack = "ack";
	char *buffer;
	char put[1024] = "";
	buffer = (char*)malloc((MAXRECV + 1) * sizeof(char));

	int input_ptr = 0;
	int output_ptr = 0;
	char output[1024] = "";


	for (i = 0; i < 30; i++)
	{
		client_socket[i] = 0;
	}

	printf("\nInitialising Winsock...");
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
	{
		printf("Failed. Error Code : %d", WSAGetLastError());
		exit(EXIT_FAILURE);
	}

	printf("Initialised.\n");

	//Create a socket
	if ((master = socket(AF_INET, SOCK_STREAM, 0)) == INVALID_SOCKET)
	{
		printf("Could not create socket : %d", WSAGetLastError());
		exit(EXIT_FAILURE);
	}

	printf("Socket created.\n");

	//Prepare the sockaddr_in structure
	server.sin_family = AF_INET;
	server.sin_addr.s_addr = inet_addr("192.168.180.1"); //INADDR_ANY;
	server.sin_port = htons(8888);

	//Bind
	if (bind(master, (struct sockaddr *)&server, sizeof(server)) == SOCKET_ERROR)
	{
		printf("Bind failed with error code : %d", WSAGetLastError());
		exit(EXIT_FAILURE);
	}

	puts("Bind done");

	//Listen to incoming connections
	listen(master, 3);// SOCKET,backlog

					  //Accept and incoming connection
	puts("Waiting for incoming connections...");

	addrlen = sizeof(struct sockaddr_in);

	while (TRUE)
	{
		//clear the socket fd set
		FD_ZERO(&readfds);

		//add master socket to fd set
		FD_SET(master, &readfds);

		//add child sockets to fd set
		for (i = 0; i < max_clients; i++)
		{
			s = client_socket[i];
			if (s > 0)
			{
				FD_SET(s, &readfds);
			}
		}

		//wait for an activity on any of the sockets, timeout is NULL , so wait indefinitely
		//until a file descriptor have been¡]ready
		activity = select(0, &readfds, NULL, NULL, NULL);
		if (activity == SOCKET_ERROR)
		{
			printf("select call failed with error code : %d", WSAGetLastError());
			exit(EXIT_FAILURE);
		}

		//If something happened on the master socket , then its an incoming connection
		if (FD_ISSET(master, &readfds))
		{
			if ((new_socket = accept(master, (struct sockaddr *)&address, (int *)&addrlen))<0)
			{
				perror("accept");
				exit(EXIT_FAILURE);
			}

			//inform user of socket number - used in send and receive commands
			printf("New connection , socket fd is %d , ip is : %s , port : %d \n", new_socket, inet_ntoa(address.sin_addr), ntohs(address.sin_port));

			//add new socket to array of sockets

			for (i = 0; i < max_clients; i++)
			{
				if (client_socket[i] == 0)
				{

					client_socket[i] = new_socket;
					printf("Adding to list of sockets at index %d \n", i);
					client_num++;
					bool device_recon = false;
					bool scratch_recon = false;
					int j = 0;
					for (j; j < device_num; j++) {

						if (devices[j].IP.S_un.S_addr == address.sin_addr.S_un.S_addr)
						{
							printf("reconnected ID: %c\n", devices[j].ID);
							int dcs = devices[j].the_s;
							client_socket[dcs] = 0;// delete old socket
							client_num--;
							device_recon = true;
							devices[j].the_s = i;//device connect to (new) socket
							break;
						}
					}
					if (!device_recon) {
						int k;
						for (k = 0; k < scratch_num; k++) {
							if (scratch[k].IP.S_un.S_addr == address.sin_addr.S_un.S_addr)
							{
								printf("reconnected SCRATCH ID: %c\n", scratch[k].ID);
								int dcs = scratch[k].the_s;
								client_socket[dcs] = 0;// delete old socket
								client_num--;
								scratch_recon = true;
								scratch[k].the_s = i;//device connect to (new) socket
								break;
							}
						}
						if (!scratch_recon) { //the new client
							devices[j].IP = address.sin_addr;//sarah 0404
							devices[j].the_s = i;//device connect to (new) socket
							device_num++;
							printf("device count %d   ", device_num);
						}
					}
					break;
				}
			}
		}//if (FD_ISSET(master, &readfds))

		for (i = 0; i < max_clients; i++)
		{
			s = client_socket[i];
			//if client presend in read sockets             
			if (FD_ISSET(s, &readfds))
			{
				//get details of the client
				getpeername(s, (struct sockaddr*)&address, (int*)&addrlen);


				//Check if it was for closing , and also read the incoming message
				//recv does not place a null terminator at the end of the string (whilst printf %s assumes there is one).
				valread = recv(s, buffer, MAXRECV, 0);
				buffer[MAXRECV] = '\0';
				if (valread == SOCKET_ERROR)
				{
					int error_code = WSAGetLastError();
					if (error_code == WSAECONNRESET)
					{
						//Somebody disconnected , get his details and print
						printf("Host disconnected unexpectedly , ip %s , port %d \n", inet_ntoa(address.sin_addr), ntohs(address.sin_port));

						//Close the socket and mark as 0 in list for reuse
						closesocket(s);
						client_socket[i] = 0;
						client_num--;
						for (int j = 0; j < device_num; j++)
						{
							if (devices[j].the_s == i)
								devices[j].the_s = 16;
						}
					}
					else
					{
						printf("recv failed with error code : %d", error_code);
						break;
					}
				}
				if (valread == 0)
				{
					//Somebody disconnected , get his details and print
					printf("Host disconnected , ip %s , port %d \n", inet_ntoa(address.sin_addr), ntohs(address.sin_port));

					//Close the socket and mark as 0 in list for reuse
					closesocket(s);
					client_socket[i] = 0;
					client_num--;
				}

				//Echo back the message that came in
				else
				{
					//add null character, if you want to use with printf/puts or other string handling functions
					buffer[valread] = '\0'; //convert to string noted by sarah delete 
					printf("\n %s:%d - %s \n", inet_ntoa(address.sin_addr), ntohs(address.sin_port), buffer);

					if (buffer[0] == 'X') {//scratch register 
						for (int sj = 0; sj < device_num; sj++) {
							if (devices[sj].IP.S_un.S_addr == address.sin_addr.S_un.S_addr) { //there is an ip in device means it's new and not classify to scratch
								scratch[scratch_num].IP.S_un.S_addr = address.sin_addr.S_un.S_addr;
								scratch[scratch_num].ID = buffer[1];
								scratch[scratch_num].the_s = i;
								//*put = buffer[1];
								printf("%c", buffer[1]);//1.2....65.66...
								scratch_num++;
								device_num--;
								devices[sj] = { 0 };
								break;
							}
						}
					}

					else if (buffer[0] == 'L') {
						bool dev_exist = false;
						char const *d_id;
						buffer++;
						for (int j = 0; j < device_num; j++) {
							//if (buffer[1] == devices[j].ID)
							d_id = &(devices[j].ID);
							if (_strnicmp(buffer, d_id, 1) == 0)//buffer
							{
								dev_exist = true;
								devices[j].action = buffer[1];
								put[0] = devices[j].action;
								s = client_socket[devices[j].the_s];
								int sback = send(s, put, strlen(put), 0);
								if (sback != strlen(put)) {
									perror("send fail");
								}
							}
						}
						if (dev_exist == false) {
							printf("the device doesn't exist.");
						}
						break;
					}
					else { // from client
						for (int j = 0; j < device_num; j++) {
							if (devices[j].IP.S_un.S_addr == address.sin_addr.S_un.S_addr) //msg from client
							{
								if (buffer[0] == 'R')//set ID
								{
									char const* str1;
									str1 = &(devices[j].ID);
									if (strcmp(str1, "\0") == 0) {//actually this condition is redundant because it must true
										devices[j].ID = buffer[1];
										printf("the ID:");
										printf("%c", buffer[1]);//1.2....65.66...
									}

									break;

								}
								else {//report the action of itself send to scratch
									devices[j].action = buffer[1];
									printf("ID: %c , action= %c", devices[j].ID, devices[j].action);
									output[input_ptr] = devices[j].ID;
									output[input_ptr + 1] = devices[j].action;
									input_ptr += 2;

									//client send msg to server
									x = client_socket[scratch[0].the_s];//send to scratches
									put[0] = output[output_ptr];//char *message
									put[1] = output[output_ptr + 1];
									send(x, put, strlen(put), 0);
								}
							}
						}// for device				
					}
				}
			}//if(FD_ISET)
		}
		if (output_ptr != input_ptr) {
			while (output_ptr < input_ptr) {
				char const* the_id;
				the_id = &(scratch[0].ID);
				if (strcmp(the_id, "") == 1) {
					x = client_socket[scratch[0].the_s];//send to scratches
					put[0] = output[output_ptr];//char *message
					put[1] = output[output_ptr + 1];
					output_ptr += 2;
					send(x, put, strlen(put), 0);
				}
				else {
					break;
				}
			}
		}

	}// while(1)

	closesocket(s);
	WSACleanup();
	return 0;
}
