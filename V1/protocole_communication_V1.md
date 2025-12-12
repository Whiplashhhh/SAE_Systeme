# Protocole de Communication - Jeu du Pendu V1 (2 Joueurs)

## 1. Vue d'ensemble

```
┌─────────────────────────────────────────────────────────────────────────────┐
│                      JEU DU PENDU V1 - 2 JOUEURS                            │
├─────────────────────────────────────────────────────────────────────────────┤
│  CLIENT 1 (1er connecté)     SERVEUR            CLIENT 2 (2ème connecté)    │
│  - Reçoit "wait x"           - Gère 2 clients   - Reçoit "start x"          │
│  - Joue en 2ème              - Alterne tours    - Joue en 1er               │
│  - 10 vies propres           - Mot secret       - 10 vies propres           │
└─────────────────────────────────────────────────────────────────────────────┘
```

### Règles du jeu V1

- **2 joueurs** jouent en alternance
- Le **Client 2** (dernier connecté) joue en **premier**
- Chaque joueur a ses **propres vies** (10) et son **propre état du mot masqué**
- Le **premier qui trouve le mot GAGNE**
- Si un joueur **perd toutes ses vies**, l'autre **continue seul**
- Si les **deux perdent**, personne ne gagne

---

## 2. Organigramme Complet (Client 1 | Serveur | Client 2)

```
     CLIENT 1                              SERVEUR                              CLIENT 2
        │                                     │                                     │
        │                          ┌──────────┴──────────┐                          │
        │                          │       DÉBUT         │                          │
        │                          │   Serveur démarre   │                          │
        │                          └──────────┬──────────┘                          │
        │                                     │                                     │
        │                          ┌──────────┴──────────┐                          │
        │                          │  socket()           │                          │
        │                          │  bind(port 8000)    │                          │
        │                          │  listen()           │                          │
        │                          └──────────┬──────────┘                          │
        │                                     │                                     │
        │                          ╔══════════╧══════════╗                          │
        │                          ║  BOUCLE PARTIES     ║                          │
        │                          ║  (infinie)          ║                          │
        │                          ╚══════════╤══════════╝                          │
        │                                     │                                     │
┌───────┴───────┐                             │                                     │
│    DÉBUT      │                             │                                     │
│ Client 1      │                             │                                     │
│ démarre       │                             │                                     │
└───────┬───────┘                             │                                     │
        │                                     │                                     │
┌───────┴───────┐              ┌──────────────┴──────────────┐                      │
│  socket()     │              │                             │                      │
│  connect()    │─────────────>│  accept() ──> socketClient1 │                      │
└───────┬───────┘              └──────────────┬──────────────┘                      │
        │                                     │                                     │
        │                      ┌──────────────┴──────────────┐                      │
        │                      │  Envoyer "wait 7"           │                      │
        │       "wait 7"       │  au Client 1                │                      │
        │<─────────────────────│                             │                      │
        │                      └──────────────┬──────────────┘                      │
        │                                     │                                     │
┌───────┴───────┐                             │                      ┌──────────────┴──────────────┐
│ Affiche:      │                             │                      │           DÉBUT             │
│ "En attente   │                             │                      │        Client 2             │
│ d'un          │                             │                      │         démarre             │
│ adversaire"   │                             │                      └──────────────┬──────────────┘
└───────┬───────┘                             │                                     │
        │                                     │                      ┌──────────────┴──────────────┐
        │                      ┌──────────────┴──────────────┐       │  socket()                   │
        │                      │                             │<──────│  connect()                  │
        │                      │  accept() ──> socketClient2 │       └──────────────┬──────────────┘
        │                      └──────────────┬──────────────┘                      │
        │                                     │                                     │
        │                      ┌──────────────┴──────────────┐                      │
        │                      │  Envoyer "start 7"          │       "start 7"      │
        │                      │  au Client 2                │─────────────────────>│
        │                      └──────────────┬──────────────┘                      │
        │                                     │                      ┌──────────────┴──────────────┐
        │                                     │                      │ Affiche:                    │
        │                                     │                      │ "Partie lancée!             │
        │                                     │                      │ Mot de 7 lettres"           │
        │                                     │                      └──────────────┬──────────────┘
        │                                     │                                     │
        │                      ┌──────────────┴──────────────┐                      │
        │                      │      INITIALISATION         │                      │
        │                      │  mot_secret = "SYSTEME"     │                      │
        │                      │  mot_masque1 = "_______"    │                      │
        │                      │  mot_masque2 = "_______"    │                      │
        │                      │  vies1 = 10, vies2 = 10     │                      │
        │                      │  client1_actif = 1          │                      │
        │                      │  client2_actif = 1          │                      │
        │                      │  tour_actuel = 2            │                      │
        │                      └──────────────┬──────────────┘                      │
        │                                     │                                     │
        │                      ╔══════════════╧══════════════╗                      │
        │                      ║     BOUCLE DE JEU           ║                      │
        │                      ║  (tant que !partie_finie)   ║                      │
        │                      ╚══════════════╤══════════════╝                      │
        │                                     │                                     │
        │                      ┌──────────────┴──────────────┐                      │
        │                      │  Joueur actuel éliminé ?    │                      │
        │                      │  (clientX_actif == 0)       │                      │
        │                      └──────────────┬──────────────┘                      │
        │                                     │                                     │
        │                           ┌─────────┴─────────┐                           │
        │                          OUI                 NON                          │
        │                           │                   │                           │
        │                           ▼                   │                           │
        │                      ┌─────────┐              │                           │
        │                      │ Changer │              │                           │
        │                      │ de tour │              │                           │
        │                      │continue │              │                           │
        │                      └────┬────┘              │                           │
        │                           │                   │                           │
        │                           └─────────┬─────────┘                           │
        │                                     │                                     │
        │                                     ▼                                     │
        │                      ┌──────────────────────────────┐                     │
        │                      │ tour_actuel == 1 ou 2 ?      │                     │
        │                      └──────────────┬───────────────┘                     │
        │                                     │                                     │
        │              ┌──────────────────────┼──────────────────────┐              │
        │         tour == 1                   │                 tour == 2          │
        │              │                      │                      │              │
        │              ▼                      │                      ▼              │
        │       ┌─────────────┐               │               ┌─────────────┐       │
        │  TURN │ Envoyer     │               │               │ Envoyer     │ TURN  │
        │<──────│ "TURN" à C1 │               │               │ "TURN" à C2 │──────>│
        │       │ "WAIT" à C2 │               │               │ "WAIT" à C1 │       │
        │       └──────┬──────┘  WAIT         │         WAIT  └──────┬──────┘       │
        │              │─────────────────────────────────────────────│              │
        │              │                      │                      │              │
        │              └──────────────────────┼──────────────────────┘              │
        │                                     │                                     │
        ▼                                     │                                     ▼
┌───────────────┐                             │                      ┌───────────────┐
│ Si TURN:      │                             │                      │ Si TURN:      │
│ "C'est votre  │                             │                      │ "C'est votre  │
│ tour!"        │                             │                      │ tour!"        │
│               │                             │                      │               │
│ Si WAIT:      │                             │                      │ Si WAIT:      │
│ "Tour de      │                             │                      │ "Tour de      │
│ l'adversaire" │                             │                      │ l'adversaire" │
└───────┬───────┘                             │                      └───────┬───────┘
        │                                     │                              │
        │                                     │                              │
┌───────┴───────┐                             │                      ┌───────┴───────┐
│ Si TURN:      │                             │                      │ Si TURN:      │
│ Saisir lettre │                             │                      │ Saisir lettre │
│ send(lettre)  │                             │                      │ send(lettre)  │
└───────┬───────┘                             │                      └───────┬───────┘
        │                                     │                              │
        │              ┌──────────────────────┼──────────────────────┐       │
        │              │                      │                      │       │
        │         (si tour==1)                │                (si tour==2)  │
        │              │                      │                      │       │
        │   lettre     │                      │                      │ lettre│
        └──────────────┼─────────────────────>│<─────────────────────┼───────┘
                       │                      │                      │
                       │       ┌──────────────┴──────────────┐       │
                       │       │  recv() lettre              │       │
                       │       │  Convertir MAJUSCULE        │       │
                       │       └──────────────┬──────────────┘       │
                       │                      │                      │
                       │       ┌──────────────┴──────────────┐       │
                       │       │  Lettre déjà jouée ?        │       │
                       │       └──────────────┬──────────────┘       │
                       │                      │                      │
                       │            ┌─────────┴─────────┐            │
                       │           OUI                 NON           │
                       │            │                   │            │
                       │            ▼                   ▼            │
                       │    ┌─────────────┐   ┌─────────────────┐    │
                       │    │ ALREADY     │   │ Ajouter lettre  │    │
                       │    │ info=       │   │ à lettres_jouees│    │
                       │    │"INFO ALREADY"│   └────────┬────────┘    │
                       │    └──────┬──────┘            │            │
                       │           │                   │            │
                       │           │        ┌──────────┴──────────┐  │
                       │           │        │ Lettre dans mot ?   │  │
                       │           │        └──────────┬──────────┘  │
                       │           │                   │             │
                       │           │         ┌─────────┴─────────┐   │
                       │           │        OUI                 NON  │
                       │           │         │                   │   │
                       │           │         ▼                   ▼   │
                       │           │  ┌─────────────┐   ┌─────────────┐
                       │           │  │ Maj         │   │  vies--     │
                       │           │  │ mot_masque  │   └──────┬──────┘
                       │           │  └──────┬──────┘          │      │
                       │           │         │                 │      │
                       │           │         ▼                 ▼      │
                       │           │  ┌─────────────┐   ┌─────────────┐
                       │           │  │ mot_masque  │   │ vies <= 0 ? │
                       │           │  │==mot_secret?│   └──────┬──────┘
                       │           │  └──────┬──────┘          │      │
                       │           │         │          ┌──────┴──────┐
                       │           │    ┌────┴────┐    OUI           NON
                       │           │   OUI       NON    │             │
                       │           │    │         │     │             │
                       │           │    ▼         ▼     ▼             ▼
                       │           │ ┌─────┐ ┌─────┐ ┌─────┐     ┌─────┐
                       │           │ │ WIN │ │ OK  │ │LOSE │     │ NOK │
                       │           │ │     │ │info=│ │actif│     │info=│
                       │           │ │info=│ │INFO │ │ =0  │     │INFO │
                       │           │ │OPPO-│ │ OK  │ │info=│     │ NOK │
                       │           │ │NENT_│ │     │ │INFO │     │     │
                       │           │ │WIN  │ │     │ │LOSE │     │     │
                       │           │ └──┬──┘ └──┬──┘ └──┬──┘     └──┬──┘
                       │           │    │      │       │           │
                       │           └────┴──────┴───────┴───────────┘
                       │                      │
                       │       ┌──────────────┴──────────────┐
                       │       │  Envoyer reponse au joueur  │
                       │       │  actif (OK/NOK/WIN/LOSE/    │
                       │       │  ALREADY + mot + vies)      │
                       │       └──────────────┬──────────────┘
                       │                      │
        ┌──────────────┼──────────────────────┼──────────────────────┼──────────────┐
        │              │                      │                      │              │
        │         (si tour==1)                │                (si tour==2)         │
        │              │                      │                      │              │
        │   reponse    │                      │                      │   reponse    │
        │<─────────────┼──────────────────────│──────────────────────┼─────────────>│
        │              │                      │                      │              │
        ▼              │                      │                      │              ▼
┌───────────────┐      │                      │                      │      ┌───────────────┐
│ Si WIN:       │      │                      │                      │      │ Si WIN:       │
│ "GAGNÉ!"      │      │                      │                      │      │ "GAGNÉ!"      │
│ partie_en_    │      │                      │                      │      │ partie_en_    │
│ cours=0       │      │                      │                      │      │ cours=0       │
│               │      │                      │                      │      │               │
│ Si LOSE:      │      │                      │                      │      │ Si LOSE:      │
│ "PERDU!"      │      │                      │                      │      │ "PERDU!"      │
│ partie_en_    │      │                      │                      │      │ partie_en_    │
│ cours=0       │      │                      │                      │      │ cours=0       │
│               │      │                      │                      │      │               │
│ Si OK/NOK/    │      │                      │                      │      │ Si OK/NOK/    │
│ ALREADY:      │      │                      │                      │      │ ALREADY:      │
│ Affiche mot   │      │                      │                      │      │ Affiche mot   │
│ et vies       │      │                      │                      │      │ et vies       │
└───────┬───────┘      │                      │                      │      └───────┬───────┘
        │              │                      │                      │              │
        │              │       ┌──────────────┴──────────────┐       │              │
        │              │       │  L'autre joueur est actif ? │       │              │
        │              │       └──────────────┬──────────────┘       │              │
        │              │                      │                      │              │
        │              │            ┌─────────┴─────────┐            │              │
        │              │           OUI                 NON           │              │
        │              │            │                   │            │              │
        │              │            ▼                   │            │              │
        │              │    ┌──────────────┐            │            │              │
        │              │    │ Envoyer info │            │            │              │
        │              │    │ à l'autre    │            │            │              │
        │              │    │ (INFO OK/NOK/│            │            │              │
        │              │    │ LOSE/ALREADY/│            │            │              │
        │              │    │ OPPONENT_WIN)│            │            │              │
        │              │    └──────┬───────┘            │            │              │
        │              │           │                    │            │              │
        │              └───────────┼────────────────────┘            │              │
        │                          │                                 │              │
        │ ┌────────────────────────┼─────────────────────────────────┼────────────┐ │
        │ │                        │                                 │            │ │
        │ │   (si tour==2)         │                    (si tour==1) │            │ │
        │ │        │               │                         │       │            │ │
        │ │   info │               │                         │ info  │            │ │
        │ │<───────┼───────────────│─────────────────────────┼──────>│            │ │
        │ │        │               │                         │       │            │ │
        ▼ ▼        │               │                         │       ▼            ▼ │
┌───────────────┐  │               │                         │  ┌───────────────┐   │
│ Si WAIT:      │  │               │                         │  │ Si WAIT:      │   │
│               │  │               │                         │  │               │   │
│ INFO OK:      │  │               │                         │  │ INFO OK:      │   │
│ "Adversaire   │  │               │                         │  │ "Adversaire   │   │
│ trouvé lettre"│  │               │                         │  │ trouvé lettre"│   │
│               │  │               │                         │  │               │   │
│ INFO NOK:     │  │               │                         │  │ INFO NOK:     │   │
│ "Adversaire   │  │               │                         │  │ "Adversaire   │   │
│ perdu vie"    │  │               │                         │  │ perdu vie"    │   │
│               │  │               │                         │  │               │   │
│ INFO LOSE:    │  │               │                         │  │ INFO LOSE:    │   │
│ "Adversaire   │  │               │                         │  │ "Adversaire   │   │
│ éliminé"      │  │               │                         │  │ éliminé"      │   │
│               │  │               │                         │  │               │   │
│ OPPONENT_WIN: │  │               │                         │  │ OPPONENT_WIN: │   │
│ "PERDU!       │  │               │                         │  │ "PERDU!       │   │
│ Adversaire    │  │               │                         │  │ Adversaire    │   │
│ a gagné"      │  │               │                         │  │ a gagné"      │   │
│ partie_en_    │  │               │                         │  │ partie_en_    │   │
│ cours=0       │  │               │                         │  │ cours=0       │   │
└───────┬───────┘  │               │                         │  └───────┬───────┘   │
        │          │               │                         │          │           │
        └──────────┴───────────────┼─────────────────────────┴──────────┘           │
                                   │                                                │
                    ┌──────────────┴──────────────┐                                 │
                    │  Si LOSE du joueur actif:   │                                 │
                    │  clientX_actif = 0          │                                 │
                    │  Changer tour_actuel        │                                 │
                    └──────────────┬──────────────┘                                 │
                                   │                                                │
                    ┌──────────────┴──────────────┐                                 │
                    │  Si WIN: partie_finie = 1   │                                 │
                    │  Si 2 éliminés:             │                                 │
                    │     partie_finie = 1        │                                 │
                    └──────────────┬──────────────┘                                 │
                                   │                                                │
                    ┌──────────────┴──────────────┐                                 │
                    │  Changer de tour            │                                 │
                    │  tour = (tour==1) ? 2 : 1   │                                 │
                    └──────────────┬──────────────┘                                 │
                                   │                                                │
                    ┌──────────────┴──────────────┐                                 │
                    │     partie_finie == 1 ?     │                                 │
                    └──────────────┬──────────────┘                                 │
                                   │                                                │
                         ┌─────────┴─────────┐                                      │
                        NON                 OUI                                     │
                         │                   │                                      │
                         │                   ▼                                      │
                         │    ┌──────────────────────────────┐                      │
                         │    │  close(socketClient1)        │                      │
                         │    │  close(socketClient2)        │                      │
                         │    └──────────────┬───────────────┘                      │
                         │                   │                                      │
        │                │                   │                                      │
        ▼                │                   │                                      ▼
┌───────────────┐        │                   │                      ┌───────────────┐
│ Si partie_en_ │        │                   │                      │ Si partie_en_ │
│ cours == 0:   │        │                   │                      │ cours == 0:   │
│   close()     │        │                   │                      │   close()     │
│   FIN CLIENT  │        │                   │                      │   FIN CLIENT  │
│               │        │                   │                      │               │
│ Sinon:        │        │                   │                      │ Sinon:        │
│   Reboucler   │        │                   │                      │   Reboucler   │
│   recv()      │        │                   │                      │   recv()      │
└───────────────┘        │                   │                      └───────────────┘
                         │                   │
                         │                   ▼
                         │    ┌──────────────────────────────┐
                         │    │  Retour BOUCLE PARTIES       │
                         │    │  (nouvelle partie)           │
                         │    └──────────────────────────────┘
                         │                   │
                         └───────────────────┘
                                (continuer BOUCLE DE JEU)
```

---

## 3. Phase de Connexion

```
    CLIENT 1                        SERVEUR                        CLIENT 2
       │                               │                               │
       │         connect()             │                               │
       │ ─────────────────────────────>│                               │
       │                               │ accept() [socket1]            │
       │                               │                               │
       │        "wait 7"               │                               │
       │ <─────────────────────────────│                               │
       │                               │                               │
       │  Affiche: "En attente         │         connect()             │
       │  d'un adversaire..."          │ <─────────────────────────────│
       │                               │ accept() [socket2]            │
       │                               │                               │
       │                               │        "start 7"              │
       │                               │ ─────────────────────────────>│
       │                               │                               │
       │                               │  Affiche: "Partie lancée!     │
       │                               │  Mot de 7 lettres"            │
       │                               │                               │
```

---

## 4. Boucle de Jeu (Alternance)

```
    CLIENT 1                        SERVEUR                        CLIENT 2
       │                               │                               │
       ├───────────────────────────────┼───────────────────────────────┤
       │            TOUR DU CLIENT 2 (premier tour)                    │
       ├───────────────────────────────┼───────────────────────────────┤
       │                               │                               │
       │         "WAIT"                │          "TURN"               │
       │ <─────────────────────────────│──────────────────────────────>│
       │                               │                               │
       │  Affiche: "Tour de            │      Saisie: "E"              │
       │  l'adversaire..."             │                               │
       │                               │           "E"                 │
       │                               │ <─────────────────────────────│
       │                               │                               │
       │                               │  [Vérifie lettre]             │
       │                               │                               │
       │       "INFO OK"               │    "OK S_S____ 10"            │
       │ <─────────────────────────────│──────────────────────────────>│
       │                               │                               │
       │  Affiche: "L'adversaire       │  Affiche: "Bien joué!         │
       │  a trouvé une lettre"         │  Mot: S_S____ Vies: 10"       │
       │                               │                               │
       ├───────────────────────────────┼───────────────────────────────┤
       │            TOUR DU CLIENT 1                                   │
       ├───────────────────────────────┼───────────────────────────────┤
       │                               │                               │
       │         "TURN"                │          "WAIT"               │
       │ <─────────────────────────────│──────────────────────────────>│
       │                               │                               │
       │      Saisie: "A"              │  Affiche: "Tour de            │
       │                               │  l'adversaire..."             │
       │           "A"                 │                               │
       │ ─────────────────────────────>│                               │
       │                               │                               │
       │                               │  [Vérifie lettre]             │
       │                               │                               │
       │    "NOK _______ 9"            │       "INFO NOK"              │
       │ <─────────────────────────────│──────────────────────────────>│
       │                               │                               │
       │  Affiche: "Raté!              │  Affiche: "L'adversaire       │
       │  Mot: _______ Vies: 9"        │  a perdu une vie"             │
       │                               │                               │
       └───────────────────────────────┴───────────────────────────────┘
                            (Répète alternance...)
```

---

## 5. Scénarios de Fin de Partie

### 4.1 Un client GAGNE (trouve le mot)

```
    CLIENT 1                        SERVEUR                        CLIENT 2
       │                               │                               │
       │         "TURN"                │          "WAIT"               │
       │ <─────────────────────────────│──────────────────────────────>│
       │                               │                               │
       │           "M"                 │                               │
       │ ─────────────────────────────>│                               │
       │                               │                               │
       │                               │  [Mot complet!]               │
       │                               │                               │
       │    "WIN SYSTEME 8"            │    "OPPONENT_WIN"             │
       │ <─────────────────────────────│──────────────────────────────>│
       │                               │                               │
       │  Affiche: "GAGNÉ!             │  Affiche: "L'adversaire       │
       │  Le mot: SYSTEME"             │  a gagné. PERDU!"             │
       │                               │                               │
       │         close()               │         close()               │
       │ ─ ─ ─ ─ ─ ─ ─ ─ ─ ─ ─ ─ ─ ─ ─>│<─ ─ ─ ─ ─ ─ ─ ─ ─ ─ ─ ─ ─ ─ ─│
       │                               │                               │
```

### 4.2 Un client PERD (plus de vies) - L'autre continue

```
    CLIENT 1                        SERVEUR                        CLIENT 2
       │                               │                               │
       │         "TURN"                │          "WAIT"               │
       │ <─────────────────────────────│──────────────────────────────>│
       │                               │                               │
       │           "Z"                 │                               │
       │ ─────────────────────────────>│                               │
       │                               │                               │
       │                               │  [vies1 = 0]                  │
       │                               │                               │
       │   "LOSE SYSTEME 0"            │      "INFO LOSE"              │
       │ <─────────────────────────────│──────────────────────────────>│
       │                               │                               │
       │  Affiche: "PERDU!             │  Affiche: "L'adversaire       │
       │  Le mot: SYSTEME"             │  a perdu (plus de vies)"      │
       │                               │                               │
       │         close()               │                               │
       │ ─ ─ ─ ─ ─ ─ ─ ─ ─ ─ ─ ─ ─ ─ ─>│                               │
       │                               │                               │
       │                               │  [Client 2 joue seul]         │
       │                               │                               │
       │                               │          "TURN"               │
       │                               │──────────────────────────────>│
       │                               │                               │
```

### 4.3 Les DEUX clients perdent

```
    CLIENT 1                        SERVEUR                        CLIENT 2
       │                               │                               │
       │  (déjà éliminé)               │          "TURN"               │
       │                               │──────────────────────────────>│
       │                               │                               │
       │                               │           "X"                 │
       │                               │ <─────────────────────────────│
       │                               │                               │
       │                               │  [vies2 = 0]                  │
       │                               │                               │
       │                               │   "LOSE SYSTEME 0"            │
       │                               │──────────────────────────────>│
       │                               │                               │
       │                               │  Affiche: "PERDU!             │
       │                               │  Le mot: SYSTEME"             │
       │                               │                               │
       │                               │         close()               │
       │                               │ <─ ─ ─ ─ ─ ─ ─ ─ ─ ─ ─ ─ ─ ─ ─│
       │                               │                               │
       │                               │  [Partie terminée,            │
       │                               │   aucun gagnant]              │
```

---

## 6. Format des Messages

### 5.1 Messages de connexion (Serveur → Client)

| Message     | Destinataire | Signification                                     |
| ----------- | ------------ | ------------------------------------------------- |
| `wait <x>`  | Client 1     | Patiente, mot de x lettres, attends un adversaire |
| `start <x>` | Client 2     | La partie commence, mot de x lettres              |

### 5.2 Messages de tour (Serveur → Client)

| Message | Signification                          |
| ------- | -------------------------------------- |
| `TURN`  | C'est ton tour, entre une lettre       |
| `WAIT`  | Attends, c'est le tour de l'adversaire |

### 5.3 Messages de résultat (Serveur → Client qui joue)

| Message   | Format                        | Signification              |
| --------- | ----------------------------- | -------------------------- |
| `OK`      | `OK <mot_masque> <vies>`      | Lettre trouvée             |
| `NOK`     | `NOK <mot_masque> <vies>`     | Lettre pas dans le mot     |
| `ALREADY` | `ALREADY <mot_masque> <vies>` | Lettre déjà jouée          |
| `WIN`     | `WIN <mot_complet> <vies>`    | Tu as gagné!               |
| `LOSE`    | `LOSE <mot_complet> <vies>`   | Tu as perdu (plus de vies) |

### 5.4 Messages d'info (Serveur → Client en attente)

| Message        | Signification                               |
| -------------- | ------------------------------------------- |
| `INFO OK`      | L'adversaire a trouvé une lettre            |
| `INFO NOK`     | L'adversaire a perdu une vie                |
| `INFO ALREADY` | L'adversaire a rejoué une lettre déjà jouée |
| `INFO LOSE`    | L'adversaire a perdu (plus de vies)         |
| `OPPONENT_WIN` | L'adversaire a gagné, tu as perdu           |

---

## 7. Variables du Serveur

| Variable          | Type     | Description                          |
| ----------------- | -------- | ------------------------------------ |
| `socketClient1`   | `int`    | Socket du 1er client connecté        |
| `socketClient2`   | `int`    | Socket du 2ème client connecté       |
| `mot_secret`      | `char[]` | Le mot à deviner ("SYSTEME")         |
| `mot_masque1`     | `char[]` | État du mot pour Client 1            |
| `mot_masque2`     | `char[]` | État du mot pour Client 2            |
| `vies1`           | `int`    | Vies du Client 1 (init: 10)          |
| `vies2`           | `int`    | Vies du Client 2 (init: 10)          |
| `lettres_jouees1` | `char[]` | Lettres jouées par Client 1          |
| `lettres_jouees2` | `char[]` | Lettres jouées par Client 2          |
| `client1_actif`   | `int`    | 1 si Client 1 encore en jeu, 0 sinon |
| `client2_actif`   | `int`    | 1 si Client 2 encore en jeu, 0 sinon |
| `tour_actuel`     | `int`    | 1 ou 2 selon le client qui joue      |

---

## 8. Variables du Client

| Variable            | Type     | Description                       |
| ------------------- | -------- | --------------------------------- |
| `descripteurSocket` | `int`    | Socket de connexion au serveur    |
| `messageServeur`    | `char[]` | Buffer de réception des messages  |
| `lettre`            | `char[]` | Buffer pour la saisie utilisateur |
| `partie_en_cours`   | `int`    | Flag de continuation (1 ou 0)     |
| `taille_mot`        | `int`    | Taille du mot à deviner           |

---

## 9. Description Textuelle du Protocole

### Phase 1 : Connexion des clients

1. Le **SERVEUR** crée une socket, bind sur le port 8000, et écoute (`listen()`)
2. Le **CLIENT 1** établit une connexion TCP vers le **SERVEUR**
3. Le **SERVEUR** accepte la connexion (`accept()`) → obtient `socketClient1`
4. Le **SERVEUR** envoie `"wait 7"` au **CLIENT 1** (attend un adversaire)
5. Le **CLIENT 1** affiche "En attente d'un adversaire..."
6. Le **CLIENT 2** établit une connexion TCP vers le **SERVEUR**
7. Le **SERVEUR** accepte la connexion (`accept()`) → obtient `socketClient2`
8. Le **SERVEUR** envoie `"start 7"` au **CLIENT 2** (la partie commence)

### Phase 2 : Initialisation de la partie

9. Le **SERVEUR** initialise :
   - `mot_secret = "SYSTEME"`
   - `mot_masque1 = "_______"` et `mot_masque2 = "_______"`
   - `vies1 = 10` et `vies2 = 10`
   - `lettres_jouees1 = []` et `lettres_jouees2 = []`
   - `client1_actif = 1` et `client2_actif = 1`
   - `tour_actuel = 2` (Client 2 joue en premier)

### Phase 3 : Boucle de jeu (répétée jusqu'à WIN ou tous éliminés)

#### Côté SERVEUR (à chaque tour) :

10. **Vérifier** si le joueur actuel est éliminé (`clientX_actif == 0`)
    - OUI → Changer de tour, `continue`
    - NON → Continuer
11. **Envoyer** `"TURN"` au joueur actif
12. **Envoyer** `"WAIT"` à l'autre joueur (s'il est encore actif)
13. **Recevoir** la lettre du joueur actif (`recv()`)
14. **Convertir** en majuscule si nécessaire
15. **Test 1** : Lettre déjà jouée ?
    - OUI → Prépare `"ALREADY mot_masque vies"` et `"INFO ALREADY"`
    - NON → Continue
16. **Ajouter** la lettre à `lettres_joueesX[]`
17. **Test 2** : Lettre dans `mot_secret` ?
    - OUI → Met à jour `mot_masqueX`
      - **Test 3** : `mot_masqueX == mot_secret` ?
        - OUI → Prépare `"WIN mot_secret vies"` et `"OPPONENT_WIN"`, `partie_finie = 1`
        - NON → Prépare `"OK mot_masque vies"` et `"INFO OK"`
    - NON → Décrémente `viesX`
      - **Test 4** : `viesX <= 0` ?
        - OUI → Prépare `"LOSE mot_secret 0"` et `"INFO LOSE"`, marque joueur éliminé
        - NON → Prépare `"NOK mot_masque vies"` et `"INFO NOK"`
18. **Envoyer** la réponse au joueur actif
19. **Envoyer** le message INFO à l'autre joueur (s'il est actif)
20. **Changer** de tour : `tour_actuel = (tour_actuel == 1) ? 2 : 1`
21. **Vérifier** si les deux sont éliminés → `partie_finie = 1`

#### Côté CLIENT (à chaque tour) :

22. **Recevoir** message du serveur (`recv()`)
23. **Si** `"TURN"` :
    - Demande une lettre à l'utilisateur
    - Envoie la lettre au serveur (`send()`)
    - Reçoit la réponse (`recv()`)
    - Analyse le CODE : `WIN`, `LOSE`, `OK`, `NOK`, `ALREADY`
    - Si `WIN` ou `LOSE` → sort de la boucle
24. **Si** `"WAIT"` :
    - Affiche "Tour de l'adversaire..."
    - Attend le message INFO (`recv()`)
    - Affiche le résultat de l'adversaire
    - Si `OPPONENT_WIN` → sort de la boucle

### Phase 4 : Fin de partie

25. Les **CLIENTS** ferment leur connexion (`close()`)
26. Le **SERVEUR** ferme les deux sockets de dialogue (`close()`)
27. Le **SERVEUR** retourne en attente de nouveaux clients (`accept()`)

---

## 10. Exemple de Partie Complète

```
SERVEUR                     CLIENT 1                    CLIENT 2
   │                           │                           │
   │◄───────── connect ────────│                           │
   │                           │                           │
   │─────── "wait 7" ─────────>│                           │
   │                           │ "En attente..."           │
   │                           │                           │
   │◄──────────────────────────┼──────── connect ──────────│
   │                           │                           │
   │─────────────────────────────────── "start 7" ────────>│
   │                           │                           │
   │══════════ TOUR 1 (Client 2) ══════════════════════════│
   │                           │                           │
   │─────── "WAIT" ───────────>│─────── "TURN" ───────────>│
   │                           │                           │
   │◄──────────────────────────┼─────────── "S" ───────────│
   │                           │                           │
   │──── "INFO OK" ───────────>│── "OK S_S____ 10" ───────>│
   │                           │                           │
   │══════════ TOUR 2 (Client 1) ══════════════════════════│
   │                           │                           │
   │─────── "TURN" ───────────>│─────── "WAIT" ───────────>│
   │                           │                           │
   │◄─────────── "A" ──────────│                           │
   │                           │                           │
   │── "NOK _______ 9" ───────>│──── "INFO NOK" ──────────>│
   │                           │                           │
   │══════════ ... (alternance) ... ═══════════════════════│
   │                           │                           │
   │══════════ TOUR FINAL ═════════════════════════════════│
   │                           │                           │
   │─────── "TURN" ───────────>│─────── "WAIT" ───────────>│
   │                           │                           │
   │◄─────────── "E" ──────────│                           │
   │                           │                           │
   │── "WIN SYSTEME 7" ───────>│── "OPPONENT_WIN" ────────>│
   │                           │                           │
   │◄─────── close ────────────│◄─────── close ────────────│
```
