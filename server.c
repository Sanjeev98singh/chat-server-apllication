#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define PORT 8080
#define PORT1 8081
#define PORT2 8082

void main()
{
    int subProcess1, subProcess2;
    int pipe1[2], pipe2[2];

    int socketDescriptor, clientSocket, recieveMessageSize, sendStatus, groupPort, groupPort1, groupPort2;
    int option = 1;
    struct sockaddr_in socketAddress;
    char rcvdClientMessage[1024] = {0};
    char *serverMessage;
    char *groupNameList = "\nEnter 1.Group 1\nEnter 2.Group 2\nWhich group do you want to join?";
    char *groupNameInput;
    int socketAddressLength = sizeof(socketAddress);

    if (pipe(pipe1) < 0)
    {
        perror("Error: Pipe 1 failed");
        exit(EXIT_FAILURE);
    }
    if (pipe(pipe2) < 0)
    {
        perror("Error: Pipe 2 failed");
        exit(EXIT_FAILURE);
    }
    subProcess1 = fork();
    if (subProcess1 > 0)
    {
        subProcess2 = fork();
        if (subProcess2 > 0)
        {

            // MAIN SERVER/PARENT HERE

            if ((socketDescriptor = socket(AF_INET, SOCK_STREAM, 0)) < 0)
            {
                perror("Socket creation failed at server");
                exit(EXIT_FAILURE);
            }

            printf("(+) Socket Created in Main Server\n");

            if (setsockopt(socketDescriptor, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &option, sizeof(option)))
            {
                perror("setsockopt error at server");
                exit(EXIT_FAILURE);
            }

            socketAddress.sin_family = AF_INET;
            socketAddress.sin_addr.s_addr = INADDR_ANY;
            socketAddress.sin_port = htons(PORT);

            if (bind(socketDescriptor, (struct sockaddr *)&socketAddress, socketAddressLength) < 0)
            {
                perror("bind failed at server");
                exit(EXIT_FAILURE);
            }

            printf("(+) Bind Successfull in Main Server\n");

            if (listen(socketDescriptor, 5) < 0)
            {
                perror("listening failed at server");
                exit(EXIT_FAILURE);
            }
            printf("(+) Listening in Main Server....\n");

            while (1)
            {

                // RECEIVING PORT FROM CHILD 1
                close(pipe1[1]);
                read(pipe1[0], &groupPort1, sizeof(groupPort1));
                close(pipe1[0]);

                // RECEIVING PORT FROM CHILD 2
                close(pipe2[1]);
                read(pipe2[0], &groupPort2, sizeof(groupPort2));
                close(pipe2[0]);

                printf("PORT RECEIVED FROM CHILD:%d\n", groupPort1);
                printf("PORT RECEIVED FROM CHILD:%d\n", groupPort2);

                if ((clientSocket = accept(socketDescriptor, (struct sockaddr *)&socketAddress, &socketAddressLength)) < 0)
                {
                    perror("cant accept at server");
                    exit(EXIT_FAILURE);
                }

                printf("(+) Accepted in Main Server....\n");

                // FIRST TIME READ (CLIENT 1 HERE)

                recieveMessageSize = read(clientSocket, rcvdClientMessage, 1024);
                if (recieveMessageSize < 0)
                {
                    perror("Could not recieve message");
                    exit(EXIT_FAILURE);
                }

                printf("\nClient: %s\n", rcvdClientMessage);
                memset(rcvdClientMessage, 0, 1024);

                // SENDING THE GROUP LIST

                sendStatus = send(clientSocket, groupNameList, strlen(groupNameList), 0);
                if (sendStatus < 0)
                {
                    perror("Could not send message to client");
                    exit(EXIT_FAILURE);
                }

                // READING WHICH GROUP TO JOIN

                recieveMessageSize = read(clientSocket, rcvdClientMessage, 1024);
                if (recieveMessageSize < 0)
                {
                    perror("Could not recieve message");
                    exit(EXIT_FAILURE);
                }

                // COMPARING THE GROUP

                if (strcmp(rcvdClientMessage, "Group 1") == 0)
                {
                    groupPort = groupPort1;
                    groupNameInput = "Group 1";
                    printf("\nClient: %s\n", rcvdClientMessage);
                }
                else if (strcmp(rcvdClientMessage, "Group 2") == 0)
                {
                    groupPort = groupPort2;
                    groupNameInput = "Group 2";
                    printf("\nClient: %s\n", rcvdClientMessage);
                }
                else
                {
                    printf("Please enter valid group\n");
                }

                // SENDING THE PORT OF CHILD TO CLIENT
                if (write(clientSocket, &groupPort, sizeof(groupPort)) < 0)
                {
                    perror("Error:Coud not send port to client");
                }
                printf("PORT SENT TO CLIENT:%d\n", groupPort);
            }
        }
        else if (subProcess2 == 0)
        {
            // CHILD 2 | SERVER 2 | GROUP 2 HERE

            int child2SocketDescriptor, child2ClientSocket;
            struct sockaddr_in child2SocketAddress;
            int child2SocketAddressLength = sizeof(child2SocketAddress);
            int group2Port = 8082;
            int recieveMessageSize, sendStatus;
            char rcvdClientMessage[1024] = {0};
            char *serverMessage;

            char *group2Buffer = "";

            group2Buffer = malloc(5000);

            if ((child2SocketDescriptor = socket(AF_INET, SOCK_STREAM, 0)) < 0)
            {
                perror("Socket creation failed at group 2 server");
                exit(EXIT_FAILURE);
            }

            printf("(+) Socket Created in Group 2 server\n");

            if (setsockopt(child2SocketDescriptor, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &option, sizeof(option)))
            {
                perror("setsockopt error at group 2 server");
                exit(EXIT_FAILURE);
            }

            child2SocketAddress.sin_family = AF_INET;
            child2SocketAddress.sin_addr.s_addr = INADDR_ANY;
            child2SocketAddress.sin_port = htons(PORT2);

            if (bind(child2SocketDescriptor, (struct sockaddr *)&child2SocketAddress, child2SocketAddressLength) < 0)
            {
                perror("bind failed at group 2 server");
                exit(EXIT_FAILURE);
            }

            printf("(+) Bind Successfull in Group 2 server\n");

            if (listen(child2SocketDescriptor, 5) < 0)
            {
                perror("listening failed at group 2 server");
                exit(EXIT_FAILURE);
            }

            printf("(+) Listening in Group 2 server....\n");

            while (1)
            {

                // SENDING THE PORT NUMBER TO PARENT/ MAIN SERVER
                close(pipe2[0]);
                write(pipe2[1], &group2Port, sizeof(group2Port));
                close(pipe2[1]);
                printf("CHILD MATHI MUKALYU: %d\n", group2Port);

                // printf("INTO GROUP 2 ACCEPT WHILE LOOP\n");
                if ((child2ClientSocket = accept(child2SocketDescriptor, (struct sockaddr *)&child2SocketAddress, &child2SocketAddressLength)) < 0)
                {
                    perror("cant accept at group 2 server");
                    exit(EXIT_FAILURE);
                }

                printf("(+) Accepted in Group 2 server....\n");

                // WORKING FINE UPTO THIS LINE

                // CHECK IF BUFFER IS EMPTY, IF SO SEND WELCOME OTHERWISE SEND THE BUFFER
                if (strlen(group2Buffer) == 0)
                {
                    // NO BUFFER TO DISPLAY
                    printf("NO MESSAGE IN BUFFER!\n");

                    // SEND WELCOME MESSAGE TO CLIENT

                    sendStatus = send(child2ClientSocket, "WELCOME TO GROUP 2", 20, 0);

                    if (sendStatus < 0)
                    {
                        perror("Could not send message to Client");
                        // exit(EXIT_FAILURE);
                    }
                }

                else
                {
                    printf("Group 2 Buffer:\n%s\n", group2Buffer);
                    sendStatus = send(child2ClientSocket, group2Buffer, strlen(group2Buffer), 0);

                    if (sendStatus < 0)
                    {
                        perror("Could not send message to Client");
                        // exit(EXIT_FAILURE);
                    }
                }

                // NOW THE CLIENT WILL SEND THE MESSAGES, SO WE READ ALL
                while (1)
                {

                    recieveMessageSize = read(child2ClientSocket, rcvdClientMessage, 1024);

                    if (recieveMessageSize < 0)
                    {

                        perror("Could not recieve message in group  2");
                        exit(EXIT_FAILURE);
                    }

                    if (!strcmp(rcvdClientMessage, "Exit"))
                    {

                        printf("(-) Client disconnected with Group 2 server\n");
                        break;
                    }

                    printf("\nClient: %s\n", rcvdClientMessage);

                    // CREATING TIME STAMP
                    time_t t;
                    struct tm *times;
                    time(&t);
                    times = localtime(&t);
                    char *timestamp = asctime(times);

                    // printf("TIME STAMP:%s",timestamp);

                    // CONCATINATING TIME STAMP TO BUFFER
                    strcat(group2Buffer, timestamp);

                    // CONCATINATING RECEIVED MESSAGE FROM CLIENT TO BUFFER
                    strcat(group2Buffer, rcvdClientMessage);

                    printf("TOTAL BUFFER:\n%s\n", group2Buffer);

                    memset(rcvdClientMessage, 0, 1024);
                }
            }
        }
    }
    else if (subProcess1 == 0)
    {
        // CHILD 1 | SERVER 1 | GROUP 1 HERE

        int child1SocketDescriptor, child1ClientSocket;
        struct sockaddr_in child1SocketAddress;
        int child1SocketAddressLength = sizeof(child1SocketAddress);
        int group1Port = 8081;
        int recieveMessageSize, sendStatus;
        char rcvdClientMessage[1024] = {0};
        char *serverMessage;

        char *group1Buffer = "";

        group1Buffer = malloc(5000);

        if ((child1SocketDescriptor = socket(AF_INET, SOCK_STREAM, 0)) < 0)
        {
            perror("Socket creation failed at group 1 server");
            exit(EXIT_FAILURE);
        }

        printf("(+) Socket Created in Group 1 server\n");

        if (setsockopt(child1SocketDescriptor, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &option, sizeof(option)))
        {
            perror("setsockopt error at group 1 server");
            exit(EXIT_FAILURE);
        }

        child1SocketAddress.sin_family = AF_INET;
        child1SocketAddress.sin_addr.s_addr = INADDR_ANY;
        child1SocketAddress.sin_port = htons(PORT1);

        if (bind(child1SocketDescriptor, (struct sockaddr *)&child1SocketAddress, child1SocketAddressLength) < 0)
        {
            perror("bind failed at group 1 server");
            exit(EXIT_FAILURE);
        }

        printf("(+) Bind Successfull in Group 1 server\n");

        if (listen(child1SocketDescriptor, 5) < 0)
        {
            perror("listening failed at group 1 server");
            exit(EXIT_FAILURE);
        }

        printf("(+) Listening in Group 1 server....\n");

        while (1)
        {

            // SENDING THE PORT NUMBER TO PARENT/ MAIN SERVER
            close(pipe1[0]);
            write(pipe1[1], &group1Port, sizeof(group1Port));
            close(pipe1[1]);
            printf("CHILD MATHI MUKALYU: %d\n", group1Port);

            // printf("INTO GROUP 1 ACCEPT WHILE LOOP\n");
            if ((child1ClientSocket = accept(child1SocketDescriptor, (struct sockaddr *)&child1SocketAddress, &child1SocketAddressLength)) < 0)
            {
                perror("cant accept at group 1 server");
                exit(EXIT_FAILURE);
            }

            printf("(+) Accepted in Group 1 server....\n");

            // WORKING FINE UPTO THIS LINE

            // CHECK IF BUFFER IS EMPTY, IF SO SEND WELCOME OTHERWISE SEND THE BUFFER
            if (strlen(group1Buffer) == 0)
            {
                // NO BUFFER TO DISPLAY
                printf("NO MESSAGE IN BUFFER!\n");

                // SEND WELCOME MESSAGE TO CLIENT

                sendStatus = send(child1ClientSocket, "WELCOME TO GROUP 1", 20, 0);

                if (sendStatus < 0)
                {
                    perror("Could not send message to Client");
                    // exit(EXIT_FAILURE);
                }
            }

            else
            {
                printf("Group 1 Buffer:\n%s\n", group1Buffer);
                sendStatus = send(child1ClientSocket, group1Buffer, strlen(group1Buffer), 0);

                if (sendStatus < 0)
                {
                    perror("Could not send message to Client");
                    // exit(EXIT_FAILURE);
                }
            }

            // NOW THE CLIENT WILL SEND THE MESSAGES, SO WE READ ALL
            while (1)
            {

                recieveMessageSize = read(child1ClientSocket, rcvdClientMessage, 1024);

                if (recieveMessageSize < 0)
                {

                    perror("Could not recieve message in group  1");
                    exit(EXIT_FAILURE);
                }

                if (!strcmp(rcvdClientMessage, "Exit"))
                {

                    printf("(-) Client disconnected with Group 1 server\n");
                    break;
                }

                printf("\nClient: %s\n", rcvdClientMessage);

                // CREATING TIME STAMP
                time_t t;
                struct tm *times;
                time(&t);
                times = localtime(&t);
                char *timestamp = asctime(times);

                // printf("TIME STAMP:%s",timestamp);

                // CONCATINATING TIME STAMP TO BUFFER
                strcat(group1Buffer, timestamp);

                // CONCATINATING RECEIVED MESSAGE FROM CLIENT TO BUFFER
                strcat(group1Buffer, rcvdClientMessage);

                printf("TOTAL BUFFER:\n%s\n", group1Buffer);

                memset(rcvdClientMessage, 0, 1024);
            }
        }
    }
}
