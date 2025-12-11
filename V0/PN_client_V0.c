//Willem VANBAELINGHEM--DEZITTER
//Thomas TEITEN
//Alex FRANCOIS
//Romain THEOBALD

#include <stdio.h>
#include <stdlib.h> /* pour exit */
#include <unistd.h> /* pour read, write, close, sleep */
#include <sys/types.h>
#include <sys/socket.h>
#include <string.h> /* pour memset */
#include <netinet/in.h> /* pour struct sockaddr_in */
#include <arpa/inet.h> /* pour htons et inet_aton */

int main(int argc, char *argv[]){
	int descripteurSocket;
	struct sockaddr_in sockaddrDistant;
	socklen_t longueurAdresse;

	char ip_dest[16];
	int  port_dest;

	// exemple de connexion à la socket :
	// ./PN_client_V0.exe 127.0.0.1 8000
	if (argc>2) { // si il y a au moins 2 arguments passés en ligne de commande
		strncpy(ip_dest,argv[1],16);
		sscanf(argv[2],"%d",&port_dest);
	}else{
		printf("USAGE : %s ip port message\n",argv[0]);
		exit(-1);
	}

	// Crée un socket de communication
	descripteurSocket = socket(AF_INET, SOCK_STREAM, 0);
	// Teste la valeur renvoyée par l’appel système socket()
	if(descripteurSocket < 0){
		perror("Erreur en création de la socket..."); // Affiche le message d’erreur
		exit(-1); // On sort en indiquant un code erreur
	}
	printf("Socket créée! (%d)\n", descripteurSocket);


	// Remplissage de sockaddrDistant (structure sockaddr_in identifiant la machine distante)
	// Obtient la longueur en octets de la structure sockaddr_in
	longueurAdresse = sizeof(sockaddrDistant);
	// Initialise à 0 la structure sockaddr_in
	// memset sert à faire une copie d'un octet n fois à partir d'une adresse mémoire donnée
	// ici l'octet 0 est recopié longueurAdresse fois à partir de l'adresse &sockaddrDistant
	memset(&sockaddrDistant, 0x00, longueurAdresse);
	// Renseigne la structure sockaddr_in avec les informations du serveur distant
	sockaddrDistant.sin_family = AF_INET;
	// On choisit le numéro de port d’écoute du serveur
	sockaddrDistant.sin_port = htons(port_dest);
	// On choisit l’adresse IPv4 du serveur
	inet_aton(ip_dest, &sockaddrDistant.sin_addr);




	// Débute la connexion vers le processus serveur distant
	if((connect(descripteurSocket, (struct sockaddr *)&sockaddrDistant,longueurAdresse)) == -1){
		perror("Erreur de connection avec le serveur distant...");
		close(descripteurSocket);
		exit(-2); // On sort en indiquant un code erreur
	}
	printf("Connexion au serveur %s:%d réussie!\n",ip_dest,port_dest);


	//réception du "start x"
	char messageServeur[256];
	memset(messageServeur, 0, 256);
	int start = recv(descripteurSocket, messageServeur, sizeof(messageServeur)-1, 0);
	if (start > 0) {
		messageServeur[start] = '\0';
		printf("Message du serveur : %s\n", messageServeur);
	} else {
		perror("Erreur réception start ou connexion fermée");
		close(descripteurSocket);
		exit(-1);
	}

	// Boucle de jeu
	int partie_en_cours = 1;
	char lettre[10]; // Buffer pour lire l'entrée utilisateur

	while (partie_en_cours) {
		//Saisie de la lettre
		printf("\nEntrez une lettre : ");
		scanf("%s", lettre);

		//Envoi de la lettre au serveur
		if (send(descripteurSocket, lettre, 1, 0) == -1) {
			perror("Erreur envoi lettre");
			break;
		}

		//Réception de la réponse du serveur
		memset(messageServeur, 0, 256);
		int lus = recv(descripteurSocket, messageServeur, sizeof(messageServeur)-1, 0);
		if (lus <= 0) {
			printf("Le serveur a fermé la connexion.\n");
			break;
		}
		messageServeur[lus] = '\0';

		// Format attendu : CODE MOT_MASQUE VIES
		// Ex: WIN SYSTEME 10
		// Ex: OK S_S____ 10

		
		char code[20], mot[100];
		int vies;
		sscanf(messageServeur, "%s %s %d", code, mot, &vies);

		if (strcmp(code, "WIN") == 0) {
			printf("\n>>> GAGNÉ ! Le mot était : %s\n", mot);
			partie_en_cours = 0;
		} else if (strcmp(code, "LOSE") == 0) {
			printf("\n>>> PERDU... Le mot était : %s\n", mot);
			partie_en_cours = 0;
		} else if (strcmp(code, "OK") == 0) {
			printf("Bien joué ! Mot : %s | Vies : %d\n", mot, vies);
		} else if (strcmp(code, "NOK") == 0) {
			printf("Raté ! Mot : %s | Vies : %d\n", mot, vies);
		} else if (strcmp(code, "ALREADY") == 0) {
			printf("Déjà joué ! Mot : %s | Vies : %d\n", mot, vies);
		} else {
			printf("Message serveur inconnu : %s\n", messageServeur);
		}
	}

	// On ferme la ressource avant de quitter
	close(descripteurSocket);

	return 0;
}
