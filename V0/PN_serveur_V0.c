//Willem VANBAELINGHEM--DEZITTER
//Thomas TEITEN
//Alex FRANCOIS
//Romain THEOBALD

//V0

#include <stdio.h>
#include <stdlib.h> /* pour exit */
#include <unistd.h> /* pour read, write, close, sleep */
#include <sys/types.h>
#include <sys/socket.h>
#include <string.h>		/* pour memset */
#include <netinet/in.h> /* pour struct sockaddr_in */
#include <arpa/inet.h>	/* pour htons et inet_aton */

#define PORT 8000
#define LG_MESSAGE 256

int main()
{
	int socketEcoute;

	struct sockaddr_in pointDeRencontreLocal;
	socklen_t longueurAdresse;

	int socketDialogue;
	struct sockaddr_in pointDeRencontreDistant;
	char messageRecu[LG_MESSAGE];
	int lus; //nb d'octets lu si connexion fermée <0

	// Crée un socket de communication
	socketEcoute = socket(AF_INET, SOCK_STREAM, 0);
	// Teste la valeur renvoyée par l’appel système socket()
	if (socketEcoute < 0)
	{
		perror("socket");
		exit(-1);		  // On sort en indiquant un code erreur
	}
	printf("Socket créée avec succès ! (%d)\n", socketEcoute); // On prépare l’adresse d’attachement locale
	// setsockopt()

	// Remplissage de sockaddrDistant (structure sockaddr_in identifiant le point d'écoute local)
	longueurAdresse = sizeof(pointDeRencontreLocal);
	// memset sert à faire une copie d'un octet n fois à partir d'une adresse mémoire donnée
	// ici l'octet 0 est recopié longueurAdresse fois à partir de l'adresse &pointDeRencontreLocal
	memset(&pointDeRencontreLocal, 0x00, longueurAdresse);
	pointDeRencontreLocal.sin_family = PF_INET;
	pointDeRencontreLocal.sin_addr.s_addr = htonl(INADDR_ANY); // attaché à toutes les interfaces locales disponibles
	pointDeRencontreLocal.sin_port = htons(PORT);			   // = 5000 ou plus

	// On demande l’attachement local de la socket
	if ((bind(socketEcoute, (struct sockaddr *)&pointDeRencontreLocal, longueurAdresse)) < 0)
	{
		perror("bind");
		exit(-2);
	}
	printf("Socket attachée avec succès !\n");

	// On fixe la taille de la file d’attente à 5 (pour les demandes de connexion non encore traitées)
	if (listen(socketEcoute, 5) < 0)
	{
		perror("listen");
		exit(-3);
	}
	printf("Socket placée en écoute passive ...\n");

	// boucle d’attente de connexion : en théorie, un serveur attend indéfiniment !
	while (1)
	{
		memset(messageRecu, 'a', LG_MESSAGE * sizeof(char));
		printf("Attente d’une demande de connexion (quitter avec Ctrl-C)\n\n");

		// c’est un appel bloquant
		socketDialogue = accept(socketEcoute, (struct sockaddr *)&pointDeRencontreDistant, &longueurAdresse);
		if (socketDialogue < 0)
		{
			perror("accept");
			close(socketDialogue);
			close(socketEcoute);
			exit(-4);
		}

		//Début de la partie
		char mot_secret[] = "SYSTEME"; // Le mot à deviner
		int taille_mot = strlen(mot_secret);
		char mot_masque[taille_mot + 1]; // Le mot masqué envoyé au client
		int vies = 10;					 // Nombre de vies

		// Initialisation du mot masqué avec des tirets
		for (int i = 0; i < taille_mot; i++)
		{
			mot_masque[i] = '_';
		}
		mot_masque[taille_mot] = '\0';

		// Préparation du message "start x"
		char message_start[LG_MESSAGE];
		sprintf(message_start, "start %d", taille_mot);

		// Envoi du message
		if (send(socketDialogue, message_start, strlen(message_start), 0) == -1)
		{
			perror("Erreur lors de l'envoi du start");
			close(socketDialogue);
			continue; // Client suivant si erreur
		}
		printf("Nouvelle partie démarrée. Mot : %s (%d lettres). Envoi de '%s'\n", mot_secret, taille_mot, message_start);

		// Liste des lettres déjà jouées
		char lettres_jouees[27] = "";
		int nb_lettres_jouees = 0;

		// Boucle de jeu pour ce client
		int partie_finie = 0;
		while (!partie_finie)
		{
			// Réception de la lettre envoyé par le client
			char lettre_recue;
			lus = recv(socketDialogue, &lettre_recue, 1, 0);
			if (lus <= 0) {
				printf("Le client a fermé la connexion.\n");
				break;
			}

			//Convertiion en majuscule pour la comparaison
			if (lettre_recue >= 'a' && lettre_recue <= 'z') {
				lettre_recue = lettre_recue - 'a' + 'A';
			}
			printf("Lettre reçue : %c\n", lettre_recue);

			//Vérifier si la lettre a déjà été jouée
			int deja_jouee = 0;
			for (int i = 0; i < nb_lettres_jouees; i++) {
				if (lettres_jouees[i] == lettre_recue) {
					deja_jouee = 1;
					break;
				}
			}

			char reponse[LG_MESSAGE];
			memset(reponse, 0, LG_MESSAGE);

			if (deja_jouee) {
				//Si lettre déjà jouée
				sprintf(reponse, "ALREADY %s %d", mot_masque, vies);
				printf("Lettre déjà jouée. Réponse : %s\n", reponse);
			} else {
				//Ajout à la liste des lettres jouées
				lettres_jouees[nb_lettres_jouees] = lettre_recue;
				nb_lettres_jouees++;

				//Vérifier si la lettre est dans le mot
				int lettre_trouvee = 0;
				for (int i = 0; i < taille_mot; i++) {
					if (mot_secret[i] == lettre_recue) {
						mot_masque[i] = lettre_recue;
						lettre_trouvee = 1;
					}
				}

				if (lettre_trouvee) {
					// Vérifier si le mot est complet
					if (strcmp(mot_masque, mot_secret) == 0) {
						sprintf(reponse, "WIN %s %d", mot_secret, vies);
						partie_finie = 1;
						printf("GAGNÉ ! Réponse : %s\n", reponse);
					} else {
						sprintf(reponse, "OK %s %d", mot_masque, vies);
						printf("Lettre trouvée. Réponse : %s\n", reponse);
					}
				} else {
					// Lettre pas dans le mot, perte d'une vie
					vies--;
					if (vies <= 0) {
						sprintf(reponse, "LOSE %s %d", mot_secret, vies);
						partie_finie = 1;
						printf("PERDU ! Réponse : %s\n", reponse);
					} else {
						sprintf(reponse, "NOK %s %d", mot_masque, vies);
						printf("Lettre non trouvée. Réponse : %s\n", reponse);
					}
				}
			}

			// Envoie de la réponse au client
			if (send(socketDialogue, reponse, strlen(reponse), 0) == -1) {
				perror("Erreur envoi réponse");
				break;
			}
		}

		printf("Fin de la partie pour ce client.\n");
		close(socketDialogue);
	}
}
