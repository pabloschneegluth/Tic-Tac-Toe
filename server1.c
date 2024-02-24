#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <netinet/in.h>
#include <stdbool.h>

#define BUFFSIZE 255 

void err_sys(char *mess) { perror(mess); exit(1); }

int tabla [5][5];
int fil, col;
bool ocupada=true, raya=false, full=false;

/*Printea una linea de la tabla*/
void printLinea(int tabla[]){
	for(int i=0; i<5; i++){
		printf("%d  ", tabla[i]);
	}
	printf("\n");
}

/*Verifica las coordenadas*/
char verificar (){
    int c;
    scanf("%d", &c);
    while(c<1 || c>5){
        printf("Incorrect value, try again (1-5): ");
        scanf("%d", &c);
    }
    char a=c+'0';
    return a;
}

/*Mira si el tablero esta lleno*/
bool llena(){
    bool hayHueco=false;
    for(int i=0; i<5; i++){
        for(int j=0; j<5; j++){
            if(tabla[i][j]==0) hayHueco=true;
        }
    }
    if(hayHueco) return false;
    else return true;
}

bool handle_client(int sock) {
    char buffer[BUFFSIZE];
    bool ret=false;

    /* Read from socket */
    read(sock, &buffer[0], BUFFSIZE);

    fil=atoi(&buffer[0]);
    col=atoi(&buffer[2]);

    tabla[fil-1][col-1]=1;
    /*Imprimir tabla*/
    printf("\n");
    for(int i=0; i<5; i++){
		printLinea(tabla[i]);
	}
    printf("\n");

    if(buffer[3]=='Y' || buffer[3]=='y'){
        ret=true;
    }

    if(llena()){
        printf("Tie. There is no empty box\n");
        full=true;
    }

    if(!full){
        if(!ret){
            char respuesta;
            while(ocupada){
                char coord[4];
                for(int i=0; i<strlen(coord); i++){
                    coord[i]=' ';
                }

                /*Pedir coordenadas*/
                printf("Enter the row (1-5): ");
                coord[0]=verificar();
                coord[1]=',';
                printf("Enter the column (1-5): ");
                coord[2]=verificar();

                /*Pasar de char a int*/
                int fil, col;
                fil=atoi(&coord[0]);
                col=atoi(&coord[2]);

                /*Si la casilla seleccionada esta vacía...*/
                if(tabla[fil-1][col-1]==0){
                    ocupada=false;
                    tabla[fil-1][col-1]=2;

                    /*Preguntar si hará 3 en ralla*/
                    printf("\nAre you going to make 3 in a row? Y(yes) or N(no): ");
                    scanf("%s", &respuesta);
                    while(respuesta!='Y' && respuesta!='N' && respuesta!='y' && respuesta!='n'){
                        printf("\nCharacter invalid. Write Y or N: ");
                        scanf("%s", &respuesta);
                    }
                    coord[3]=respuesta;

                    /* Write to socket */
                    write(sock, coord, strlen(coord) + 1);
                }
                else{
                    printf("\nThe box selected has already a value, select another one.\n");
                }
            }
            ocupada=true;

            /*Imprimir tabla*/
            printf("\n");
            for(int i=0; i<5; i++){
                printLinea(tabla[i]);
            }
            printf("\n");

            if(respuesta=='Y' || respuesta=='y'){
                raya=true;
                printf("The winner is: PLAYER 2\n YOU WIN :)\n");
                exit(0);
            }
            
            if(llena()){
                printf("Tie. There is no empty box\n");
                full=true;
            }
            else printf("Turn of Player 1\n");
        }
    }

    /* Close socket */
    close(sock);
    return ret;
}

int main(int argc, char *argv[]) {
    struct sockaddr_in echoserver, echoclient;
    int serversock, clientsock;
    int result;

    /* Check input arguments */
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <port>\n", argv[0]);
        exit(1);
    }

    /* Create TCP socket */
    serversock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (serversock < 0) {
        err_sys("Error socket");
    }

    /* Set information for sockaddr_in structure */
    memset(&echoserver, 0, sizeof(echoserver));       /* Reset memory */
    echoserver.sin_family = AF_INET;                  /* Internet/IP */
    echoserver.sin_addr.s_addr = htonl(INADDR_ANY);   /* Any address */
    echoserver.sin_port = htons(atoi(argv[1]));       /* Server port */

    /* Bind socket */
    result = bind(serversock, (struct sockaddr *) &echoserver, sizeof(echoserver));
    if (result < 0) {
        err_sys("Error bind");
    }

    /* Listen socket */
    result = listen(serversock, 1);
    if (result < 0) {
        err_sys("Error listen");
    }

    /*Inicializar la tablas a 0*/
    for(int i=0; i<5; i++){
        for(int j=0; j<5; j++){
            tabla[i][j]=0;
        }
    }
    
    /* As a server we are in an infinite loop, waiting forever */
    int l=0;
    while (1) {
        unsigned int clientlen = sizeof(echoclient);

        /* Wait for a connection from a client */
        clientsock = accept(serversock, (struct sockaddr *) &echoclient, &clientlen);
        if (clientsock < 0) {
            err_sys("Error accept");
        }

        if(l==0){
            printf("\nYou are PLAYER 2 \nValue 0 -> Empty box    Value 1 -> Box of Player 1    Value 2 -> Box of Player 2\n");
            /*Imprimir tabla*/
            printf("\n");
            for(int i=0; i<5; i++){
                printLinea(tabla[i]);
            }
            printf("\n");
            printf("Turn of Player 1\n");
        }

        /* Call function to handle socket */
        bool acaba=handle_client(clientsock);
        if(acaba){
            printf("The winner is Player 1\n YOU LOSE :(\n");
            exit(0);
        }
        if(full){
            exit(0);
        }
        l=l+1;
    }
}