
#include "../headers/SDL2/SDL.h"
#include "../headers/SDL2/SDL_net.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <signal.h>

volatile sig_atomic_t stop;

typedef struct
{
    int id;
    char nick[30];
    char country[40];
    SDL_Color color;
    int imgID;
    int bestTime;
    int averageTime;
    int firstLog;
    int defeats;
    int wins;

} profile;

typedef struct
{
    char instruction[20];
    int ret;
    profile client;
    profile bestClients[11];
    int received;
    int infos;
    int canSend;

} netPacket;

static profile client;

static int threadProcessing = 0;

int getThreadStatut() {

    return threadProcessing;
}

void setPacketThreadTimeOut(netPacket *packet,int timeOut) {

    int prev = SDL_GetTicks();

    while ((SDL_GetTicks() - prev) < timeOut && getThreadStatut());

    packet->canSend = 0;
}

void inthand(int signum) {
    stop = 1;
}

void sendAndRecieve(IPaddress* ip, netPacket *packet);

int checkString(char *str) {

    int i = 0;

    do {

        if ((str[i] > 48 && str[i] < 57) || (str[i] > 65 && str[i] < 95) || (str[i] > 97 && str[i] < 122)) {}
        else {

            return -1;
        }

        i++;
    } while (str[i] != '\0');

    return 0;
}

void encrypt(char *str) {

    int key = 112211;

    for (int i = 0; i < 20; ++i)
    {
        str[i] = str[i] + key;
    }

}

void decrypt(char *str) {

    int key = 112211;

    for (int i = 0; i < 20; ++i)
    {
        str[i] = str[i] - key;
    }
}

void saveClientInfo() {

    FILE *file;

    file = fopen("../client.info", "w");

    fprintf(file, "CLIENT\n");

    encrypt(client.nick);

    fprintf(file, "%s\n", client.nick);

    char str[20];
    sprintf(str, "%d", client.id);

    encrypt(str);

    fprintf(file, "%s\n", str);

    fclose(file);
}

int checkClientInfo() {

    profile customClient;

    customClient = client;

    if (strlen(customClient.nick) > 30 || strlen(customClient.nick) < 3)
    {
        return -1;
    }

    if (customClient.id < 1000 || customClient.id > 9999)
    {
        return -2;
    }

    if (customClient.bestTime < 0)
    {
        return -3;
    }

    return 0;
}

int netManage( netPacket *packet )
{
    IPaddress serverIP;

    SDLNet_ResolveHost(&serverIP, "176.138.70.122", 1234);

    sendAndRecieve(&serverIP, packet);

    return 0;
}

void sendAndRecieve(IPaddress* ip, netPacket *packet) {

    TCPsocket server = SDLNet_TCP_Open(ip);

    if (server == NULL) {
        printf("Erreur de connexion au serveur > %s\n", SDLNet_GetError());

    }
    else if(packet->canSend == 1){

        SDLNet_TCP_Send(server, packet, sizeof(*packet));

        if (packet->ret == 1)
        {
            SDLNet_TCP_Recv(server, packet, sizeof(*packet));
        }

        SDLNet_TCP_Close(server);
    }

    threadProcessing = 0;
}

void getClientInfo() {

    client.id = 0;
    client.bestTime = 0;
    strcpy(client.nick, "");

    FILE *file;

    file = fopen("../client.info", "r");

    char temp[40];

    for (int i = 0; i < 2; ++i)
    {
        fgets(client.nick, 40, file);
    }
    client.nick[strcspn(client.nick, "\n")] = '\0';

    decrypt(client.nick);

    fgets(temp, 40, file);
    fgets(temp, 40, file);

    decrypt(temp);

    client.id = atoi(temp);

    fclose(file);
}

profile getClientProfile() {

    return client;
}

void setClientProfile(profile tempProfile) {

    client = tempProfile;
}

profile initBlankProfile(){

    profile element;

    element.id = -1;
    element.imgID = -1;
    element.bestTime = 0;
    element.averageTime = 0;
    element.firstLog = 0;
    element.wins = 0;
    element.defeats = 0;

    element.color.r = 0;
    element.color.g = 0;
    element.color.b = 0;
    element.color.a = 255;

    strcpy(element.nick,"");
    strcpy(element.country,"");

    return element;
}

void netRequest(void *tempPacket) {

    netPacket *packet = (netPacket*) tempPacket;

    threadProcessing = 1;

    int tempValue;

    packet->received = 0;

    if (strstr(packet->instruction, "getList"))
    {
        packet->client = client;
        packet->ret = 1;
        packet->canSend = 1;
    }
    else if (!strcmp(packet->instruction, "createProfile"))
    {
        packet->ret = 1;
        packet->canSend = 1;
    }
    else if (!strcmp(packet->instruction,"updateProfile"))
    {
        packet->canSend = 1;
        packet->ret = 1;
        tempValue = packet->client.bestTime;
        packet->client = client;
        packet->client.bestTime = tempValue;
    }

    netManage(packet);
}
