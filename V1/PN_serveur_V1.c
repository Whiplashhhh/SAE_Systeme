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

	int lus; // nb d'octets lu si connexion fermée <0

	// Crée un socket de communication
	socketEcoute = socket(AF_INET, SOCK_STREAM, 0);
	// Teste la valeur renvoyée par l'appel système socket()
	if (socketEcoute < 0)
	{
		perror("socket");
		exit(-1);
	}
	printf("Socket créée avec succès ! (%d)\n", socketEcoute);

	// Remplissage de sockaddrDistant (structure sockaddr_in identifiant le point d'écoute local)
	longueurAdresse = sizeof(pointDeRencontreLocal);
	memset(&pointDeRencontreLocal, 0x00, longueurAdresse);
	pointDeRencontreLocal.sin_family = PF_INET;
	pointDeRencontreLocal.sin_addr.s_addr = htonl(INADDR_ANY);
	pointDeRencontreLocal.sin_port = htons(PORT);

	// On demande l'attachement local de la socket
	if ((bind(socketEcoute, (struct sockaddr *)&pointDeRencontreLocal, longueurAdresse)) < 0)
	{
		perror("bind");
		exit(-2);
	}
	printf("Socket attachée avec succès !\n");

	// On fixe la taille de la file d'attente à 5
	if (listen(socketEcoute, 5) < 0)
	{
		perror("listen");
		exit(-3);
	}
	printf("Socket placée en écoute passive ...\n");

	// boucle d'attente de connexion
	while (1)
	{
		printf("-En attente de 2 joueurs-\n\n");

		// Variables pour les 2 clients
		int socketClient1, socketClient2;
		struct sockaddr_in adresseClient1, adresseClient2;

		// Client 1
		printf("Attente du joueur 1...\n");
		socketClient1 = accept(socketEcoute, (struct sockaddr *)&adresseClient1, &longueurAdresse);
		if (socketClient1 < 0)
		{
			perror("accept client 1");
			continue;
		}
		printf("Joueur 1 connecté!\n");

		// Préparation du mot secret
		char mot_secret[] = "SYSTEME";
		int taille_mot = strlen(mot_secret);

		// Envoi "wait x" au client 1
		char message_wait[LG_MESSAGE];
		sprintf(message_wait, "wait %d", taille_mot);
		if (send(socketClient1, message_wait, strlen(message_wait), 0) == -1)
		{
			perror("Erreur envoi wait au client 1");
			close(socketClient1);
			continue;
		}
		printf("Envoyé '%s' au joueur 1. En attente du joueur 2...\n", message_wait);

		// Client 2
		printf("Attente du joueur 2...\n");
		socketClient2 = accept(socketEcoute, (struct sockaddr *)&adresseClient2, &longueurAdresse);
		if (socketClient2 < 0)
		{
			perror("accept client 2");
			close(socketClient1);
			continue;
		}
		printf("Joueur 2 connecté!\n");

		// Envoi "start x" au client 2
		char message_start[LG_MESSAGE];
		sprintf(message_start, "start %d", taille_mot);
		if (send(socketClient2, message_start, strlen(message_start), 0) == -1)
		{
			perror("Erreur envoi start au client 2");
			close(socketClient1);
			close(socketClient2);
			continue;
		}
		printf("Envoyé '%s' au joueur 2.\n", message_start);

		printf("\n=== Partie lancée! Mot: %s (%d lettres) ===\n\n", mot_secret, taille_mot);

		// Initialisation des 2 joueurs

		// Mots masqués
		char mot_masque1[taille_mot + 1];
		char mot_masque2[taille_mot + 1];
		for (int i = 0; i < taille_mot; i++)
		{
			mot_masque1[i] = '_';
			mot_masque2[i] = '_';
		}
		mot_masque1[taille_mot] = '\0';
		mot_masque2[taille_mot] = '\0';

		// Vies
		int vies1 = 10;
		int vies2 = 10;

		// Lettres jouées par les joueurs
		char lettres_jouees1[27] = "";
		char lettres_jouees2[27] = "";
		int nb_lettres_jouees1 = 0;
		int nb_lettres_jouees2 = 0;

		// État des joueurs (actif ou éliminé)
		int client1_actif = 1;
		int client2_actif = 1;

		// Tour actuel (2 = client 2 commence)
		int tour_actuel = 2;

		// délai pour que les clients traitent les messages start/wait correctement
		usleep(100000); // 100ms

		// Boucle de jeu
		int partie_finie = 0;
		while (!partie_finie)
		{
			// Déterminer le joueur actif et le joueur en attente
			int socketActif, socketAttente;
			int *viesActif;
			char *mot_masqueActif;
			char *lettres_joueesActif;
			int *nb_lettres_joueesActif;
			int *clientActif_actif;

			if (tour_actuel == 1)
			{
				socketActif = socketClient1;
				socketAttente = socketClient2;
				viesActif = &vies1;
				mot_masqueActif = mot_masque1;
				lettres_joueesActif = lettres_jouees1;
				nb_lettres_joueesActif = &nb_lettres_jouees1;
				clientActif_actif = &client1_actif;
			}
			else
			{
				socketActif = socketClient2;
				socketAttente = socketClient1;
				viesActif = &vies2;
				mot_masqueActif = mot_masque2;
				lettres_joueesActif = lettres_jouees2;
				nb_lettres_joueesActif = &nb_lettres_jouees2;
				clientActif_actif = &client2_actif;
			}

			// joueur actif ?
			if (!(*clientActif_actif))
			{
				// Passer au joueur suivant
				tour_actuel = (tour_actuel == 1) ? 2 : 1;

				// Vérifier si les deux joueurs sont éliminés
				if (!client1_actif && !client2_actif)
				{
					printf("Les deux joueurs ont perdu!\n");
					partie_finie = 1;
				}
				continue;
			}

			// Envoyer TURN au joueur actif
			if (send(socketActif, "TURN", 4, 0) == -1)
			{
				perror("Erreur envoi TURN");
				partie_finie = 1;
				break;
			}
			printf("Joueur %d: TURN envoyé\n", tour_actuel);

			// Envoyer WAIT au joueur en attente (s'il est encore actif)
			int autreJoueur = (tour_actuel == 1) ? 2 : 1;
			int autreActif = (tour_actuel == 1) ? client2_actif : client1_actif;
			if (autreActif)
			{
				if (send(socketAttente, "WAIT", 4, 0) == -1)
				{
					perror("Erreur envoi WAIT");
				}
				printf("Joueur %d: WAIT envoyé\n", autreJoueur);
			}

			// Réception de la lettre du joueur actif
			char lettre_recue;
			lus = recv(socketActif, &lettre_recue, 1, 0);
			if (lus <= 0)
			{
				printf("Le joueur %d a fermé la connexion.\n", tour_actuel);
				*clientActif_actif = 0;
				// Si l'un quitte l'autre doit finir la partie
				tour_actuel = (tour_actuel == 1) ? 2 : 1;
				continue;
			}

			// Conversion en majuscule
			if (lettre_recue >= 'a' && lettre_recue <= 'z')
			{
				lettre_recue = lettre_recue - 'a' + 'A';
			}
			printf("Joueur %d a envoyé: %c\n", tour_actuel, lettre_recue);

			// Vérifier si la lettre a déjà été jouée par CE joueur
			int deja_jouee = 0;
			for (int i = 0; i < *nb_lettres_joueesActif; i++)
			{
				if (lettres_joueesActif[i] == lettre_recue)
				{
					deja_jouee = 1;
					break;
				}
			}

			char reponse[LG_MESSAGE];
			char info[LG_MESSAGE];
			memset(reponse, 0, LG_MESSAGE);
			memset(info, 0, LG_MESSAGE);

			if (deja_jouee)
			{
				// Lettre déjà jouée
				sprintf(reponse, "ALREADY %s %d", mot_masqueActif, *viesActif);
				sprintf(info, "INFO ALREADY");
				printf("Joueur %d: Lettre déjà jouée\n", tour_actuel);
			}
			else
			{
				// Ajouter à la liste des lettres jouées
				lettres_joueesActif[*nb_lettres_joueesActif] = lettre_recue;
				(*nb_lettres_joueesActif)++;

				// Vérifier si la lettre est dans le mot
				int lettre_trouvee = 0;
				for (int i = 0; i < taille_mot; i++)
				{
					if (mot_secret[i] == lettre_recue)
					{
						mot_masqueActif[i] = lettre_recue;
						lettre_trouvee = 1;
					}
				}

				if (lettre_trouvee)
				{
					// Vérifier si le mot est complet -> GAGNE
					if (strcmp(mot_masqueActif, mot_secret) == 0)
					{
						sprintf(reponse, "WIN %s %d", mot_secret, *viesActif);
						sprintf(info, "OPPONENT_WIN");
						printf("Joueur %d a GAGNÉ!\n", tour_actuel);

						// Envoyer au joueur actif
						send(socketActif, reponse, strlen(reponse), 0);

						// Envoyer au joueur en attente (s'il est actif)
						if (autreActif)
						{
							send(socketAttente, info, strlen(info), 0);
						}

						partie_finie = 1;
						continue;
					}
					else
					{
						sprintf(reponse, "OK %s %d", mot_masqueActif, *viesActif);
						sprintf(info, "INFO OK");
						printf("Joueur %d: Lettre trouvée\n", tour_actuel);
					}
				}
				else
				{
					// Lettre pas dans le mot -> perte d'une vie
					(*viesActif)--;

					if (*viesActif <= 0)
					{
						// Le joueur a perdu
						sprintf(reponse, "LOSE %s %d", mot_secret, *viesActif);
						sprintf(info, "INFO LOSE");
						printf("Joueur %d a PERDU (plus de vies)!\n", tour_actuel);

						// Envoyer au joueur actif
						send(socketActif, reponse, strlen(reponse), 0);

						// Marquer comme éliminé
						*clientActif_actif = 0;

						// Envoyer info au joueur en attente
						if (autreActif)
						{
							send(socketAttente, info, strlen(info), 0);
							// Délai pour laisser le client traiter INFO LOSE avant de recevoir TURN
							usleep(100000); // 100ms
						}

						// Vérifier si les deux ont perdu
						if (!client1_actif && !client2_actif)
						{
							printf("Les deux joueurs ont perdu!\n");
							partie_finie = 1;
						}

						// Passer au tour suivant
						tour_actuel = (tour_actuel == 1) ? 2 : 1;
						continue;
					}
					else
					{
						sprintf(reponse, "NOK %s %d", mot_masqueActif, *viesActif);
						sprintf(info, "INFO NOK");
						printf("Joueur %d: Lettre non trouvée, %d vies restantes\n", tour_actuel, *viesActif);
					}
				}
			}

			// Envoyer la réponse au joueur actif
			if (send(socketActif, reponse, strlen(reponse), 0) == -1)
			{
				perror("Erreur envoi réponse");
			}

			// Envoyer l'info au joueur en attente (s'il est actif)
			if (autreActif)
			{
				if (send(socketAttente, info, strlen(info), 0) == -1)
				{
					perror("Erreur envoi info");
				}
			}

			// délai pour éviter la concaténation des messages TCP
			usleep(50000); // 50ms

			// Alterner le tour
			tour_actuel = (tour_actuel == 1) ? 2 : 1;
		}

		printf("Fin de la partie.\n");
		close(socketClient1);
		close(socketClient2);
	}
}
