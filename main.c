#include <stdio.h>
#include <winsock2.h>
#include <stdint.h>

int main()
{
    WSADATA lpWSAData;

    WORD version = MAKEWORD(2,2);

    int startupResult = WSAStartup(version, &lpWSAData);

    if (startupResult != 0)
    {
        printf("Failed to initialize windows socket. Error code: %d", startupResult);
        return 1;
    }
    else
    {
        printf("Initialized WSA\n");
    }

    SOCKET listenSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

    u_long mode;
    
    if (listenSocket == INVALID_SOCKET)
    {
        printf("Failed to create socket: %d\n", WSAGetLastError());
        return 1;
    }
    else
    {
        printf("Created socket\n");
    }

    int success = ioctlsocket(listenSocket, FIONBIO, &mode); 

    if (success != 0)
    {
        printf("Could not switch socket mode. :(\n%d\n", WSAGetLastError());
    }
    struct sockaddr_in address;
    address.sin_family = AF_INET;
    address.sin_port = htons(8000);
    address.sin_addr.s_addr = inet_addr("0.0.0.0\0");
    int bound = bind(listenSocket, (struct sockaddr*) &address, sizeof(address));

    if (bound == SOCKET_ERROR)
    {
        printf("Sorry m8, failed to bind...\n");
        printf("%d", WSAGetLastError());
        return 1;
    }
    else
    {
        printf("Bound socket\n");
    }

    printf("socket looks good to me kek\n");

    if (listen(listenSocket,SOMAXCONN) == SOCKET_ERROR)
    {
        printf("Failed to listen: %s\n", WSAGetLastError());
        closesocket(listenSocket);
    }
    else{
        printf("Listening\n");
    }


    SOCKET clientSocket;
    int socketPopulated = 0;
    while (1)
    {
        if (!socketPopulated)
        {
            clientSocket = accept(listenSocket, NULL, NULL);
            if (clientSocket != INVALID_SOCKET)
            {
                int cSuccess = ioctlsocket(clientSocket, FIONBIO, &mode); 

                if (cSuccess != 0)
                {
                    printf("Could not switch socket mode. :(\n%d\n", WSAGetLastError());
                    break;
                }

                socketPopulated = 1;
            }
            // else
            // {
            //     printf("Could not connect with client: %d\n", WSAGetLastError());
            //     break;
            // }
        }
        else
        {
            char recvbuf[5000];
            int iResult = recv(clientSocket, recvbuf, 5000, 0);

            if (iResult > 0)
            {
                printf("received %d bytes:\n", iResult);
                printf("%s\n", recvbuf);
            }
            else if (iResult == SOCKET_ERROR)
            {
                int error = WSAGetLastError();

                if (error != 10035)
                {
                    closesocket(clientSocket);
                    printf("UH OH UH OH\n%d\n", error);
                    break;
                }
            }
        }
    }
    
    closesocket(listenSocket);
    WSACleanup();
    return 0;
}
