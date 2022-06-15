#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define PORT 8080

void main()
{
    int socketDescriptor, recieveMessageSize, sendStatus;
    struct sockaddr_in socketAddress;
    int option = 1;
    char rcvdServerMessage[1024] = {0};
    char *clientMessage;
    int socketAddressLength = sizeof(socketAddress);
    int portToJoin;

    if ((socketDescriptor = socket(AF_INET, SOCK_STREAM, 0)) <= 0)
    {
        perror("Socket creation failed at server");
        exit(EXIT_FAILURE);
    }

    printf("(+) Socket Created in Client\n");

    socketAddress.sin_family = AF_INET;
    socketAddress.sin_port = htons(PORT);

    if (inet_pton(AF_INET, "127.0.0.1", &socketAddress.sin_addr) <= 0)
    {
        perror("\nInvalid address/ Address not supported \n");
        exit(EXIT_FAILURE);
    }

    if (connect(socketDescriptor, (struct sockaddr *)&socketAddress, socketAddressLength) < 0)
    {
        printf("\nConnection Failed at client \n");
        exit(EXIT_FAILURE);
    }

    printf("(+) Client Connected to Main server....\n");

    // SEND FIRST MESSAGE (CLIENT 1 HERE)
    gets(clientMessage);

    // Sending the Message to server using send method
    sendStatus = send(socketDescriptor, clientMessage, strlen(clientMessage), 0);
    // checking if message sent successfully
    if (sendStatus < 0)
    {
        // Displaying error and exiting the code
        perror("Could not send message to server");
        exit(EXIT_FAILURE);
    }

    // GETTING THE GROUP LIST

    recieveMessageSize = read(socketDescriptor, rcvdServerMessage, 1024);
    if (recieveMessageSize < 0)
    {
        perror("Could not recieve message from server");
        exit(EXIT_FAILURE);
    }
    printf("\nServer: %s\n", rcvdServerMessage);

    memset(rcvdServerMessage, 0, 1024);

    // SENDING THE GROUP WHICH CLIENT WANT TO JOIN

    gets(clientMessage);
    sendStatus = send(socketDescriptor, clientMessage, strlen(clientMessage), 0);
    if (sendStatus < 0)
    {
        perror("Could not send message to server");
        exit(EXIT_FAILURE);
    }

    // WHEN NEW CLIENT CONNECTED IT IS STUCK AT THIS POINT

    printf("READING PORT FROM SERVER...\n");

    // RECEIVING THE PORT FROM THE MAIN SERVER
    if (read(socketDescriptor, &portToJoin, sizeof(portToJoin)) < 0)
    {
        perror("ERROR:reading group port");
        exit(EXIT_FAILURE);
    }
    printf("Port Received:%d\n", portToJoin);

    close(socketDescriptor);
    printf("(-) Client disconnected with Main server\n");

    socketDescriptor = socket(AF_INET, SOCK_STREAM, 0);

    // RESETING THE ADDRESS
    socketAddress.sin_family = AF_INET;
    socketAddress.sin_port = htons(portToJoin);
    if (inet_pton(AF_INET, "127.0.0.1", &socketAddress.sin_addr) < 0)
    {
        perror("\nInvalid address/ Address not supported \n");
        exit(EXIT_FAILURE);
    }

    if (connect(socketDescriptor, (struct sockaddr *)&socketAddress, socketAddressLength) < 0)
    {
        printf("\nConnection Failed at client 8081\n");
        exit(EXIT_FAILURE);
    }

    printf("(+) Client Connected to Group 1 Server\n");

    // RECEIVING BUFFER IF ANY OTHERWISE WELCOME MESSAGE

    recieveMessageSize = read(socketDescriptor, rcvdServerMessage, 1024);
    if (recieveMessageSize < 0)
    {
        perror("Could not recieve message from server");
        exit(EXIT_FAILURE);
    }

    printf("\nServer: %s\n", rcvdServerMessage);

    memset(rcvdServerMessage, 0, 1024);

    while (1)
    {
        printf("INTO WHILE LOOP TO SEND MESSAGES TO SERVER 1\n");
        gets(clientMessage);

        if (!strcmp(clientMessage, "Exit"))
        {
            sendStatus = send(socketDescriptor, clientMessage, strlen(clientMessage), 0);

            printf("(-) Client disconnected with Group server\n");
            close(socketDescriptor);
            break;
            // exit(EXIT_SUCCESS);
        }

        sendStatus = send(socketDescriptor, clientMessage, strlen(clientMessage), 0);

        if (sendStatus < 0)
        {

            perror("Could not send message to server");
            exit(EXIT_FAILURE);
        }
    }
}
