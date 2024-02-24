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

/*Printea una linea de la tabla*/
void printLinea(int tabla[]){
	for(int i=0; i<5; i++){
		printf("%d  ", tabla[i]);
	}
	printf("\n");
}

int main(int argc, char *argv[]) {
    struct sockaddr_in echoserver;
    char buffer[BUFFSIZE];
    unsigned int echolen;
    int sock, result;
    int received = 0;

    /* Check input arguments */
    if (argc != 3) {
        fprintf(stderr, "Usage: %s <ip_server> <port>\n", argv[0]);
        exit(1);
    }

	bool ocupada=true, raya=false, full=false;

    /*Inicializar la tablas a 0*/
	for(int i=0; i<5; i++){
		for(int j=0; j<5; j++){
			tabla[i][j]=0;
		}
	}
    printf("\nYour are PLAYER 1 \nValue 0 -> Empty box    Value 1 -> Box of Player 1    Value 2 -> Box of Player 2\n");

    /*Imprimir tabla*/
    printf("\n");
    for(int i=0; i<5; i++){
		printLinea(tabla[i]);
	}
    printf("\n");

    while(!raya && !full){

        /* Try to create TCP socket */
        sock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
        if (sock < 0) {
            err_sys("Error socket");
        }

        /* Set information for sockaddr_in */
        memset(&echoserver, 0, sizeof(echoserver));       /* Reset memory */
        echoserver.sin_family = AF_INET;                  /* Internet/IP */
        echoserver.sin_addr.s_addr = inet_addr(argv[1]);  /* Server address */
        echoserver.sin_port = htons(atoi(argv[2]));       /* Server port */

        /* Try to have a connection with the server */
        result = connect(sock, (struct sockaddr *) &echoserver, sizeof(echoserver));
        if (result < 0) {
            err_sys("Error connect");
        }

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
                tabla[fil-1][col-1]=1;

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
            printf("The winner is: PLAYER 1\n You win :)\n");
            break;
        }
        
        if(llena()){
            printf("Tie. There is no empty box\n");
            exit(0);
        }
        else printf("Turn of Player 2\n");

        /*Respuesta server*/
        char coordserv[4];
        read(sock, coordserv, strlen(coordserv));

        int fil, col;
        fil=atoi(&coordserv[0]);
        col=atoi(&coordserv[2]);

        tabla[fil-1][col-1]=2;
        /*Imprimir tabla*/
        printf("\n");
        for(int i=0; i<5; i++){
            printLinea(tabla[i]);
        }
        printf("\n");

        if(coordserv[3]=='Y' || coordserv[3]=='y'){
            raya=true;
            printf("The winner is: PLAYER 2\n YOU LOSE :(\n");
        }

        if(llena()){
            printf("Tie. There is no empty box\n");
            exit(0);
        }
    }
    
    /* Close socket */
    close(sock);
    exit(0);
}