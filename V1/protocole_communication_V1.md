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

---

## 2. Phase de Connexion

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

## 3. Boucle de Jeu (Alternance)

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

## 4. Scénarios de Fin de Partie

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

## 5. Format des Messages

### 5.1 Messages de connexion (Serveur → Client)

| Message | Destinataire | Signification |
|---------|--------------|---------------|
| `wait <x>` | Client 1 | Patiente, mot de x lettres, attends un adversaire |
| `start <x>` | Client 2 | La partie commence, mot de x lettres |

### 5.2 Messages de tour (Serveur → Client)

| Message | Signification |
|---------|---------------|
| `TURN` | C'est ton tour, entre une lettre |
| `WAIT` | Attends, c'est le tour de l'adversaire |

### 5.3 Messages de résultat (Serveur → Client qui joue)

| Message | Format | Signification |
|---------|--------|---------------|
| `OK` | `OK <mot_masque> <vies>` | Lettre trouvée |
| `NOK` | `NOK <mot_masque> <vies>` | Lettre pas dans le mot |
| `ALREADY` | `ALREADY <mot_masque> <vies>` | Lettre déjà jouée |
| `WIN` | `WIN <mot_complet> <vies>` | Tu as gagné! |
| `LOSE` | `LOSE <mot_complet> <vies>` | Tu as perdu (plus de vies) |

### 5.4 Messages d'info (Serveur → Client en attente)

| Message | Signification |
|---------|---------------|
| `INFO OK` | L'adversaire a trouvé une lettre |
| `INFO NOK` | L'adversaire a perdu une vie |
| `INFO ALREADY` | L'adversaire a rejoué une lettre déjà jouée |
| `INFO LOSE` | L'adversaire a perdu (plus de vies) |
| `OPPONENT_WIN` | L'adversaire a gagné, tu as perdu |

---

## 6. Variables du Serveur

| Variable | Type | Description |
|----------|------|-------------|
| `socketClient1` | `int` | Socket du 1er client connecté |
| `socketClient2` | `int` | Socket du 2ème client connecté |
| `mot_secret` | `char[]` | Le mot à deviner ("SYSTEME") |
| `mot_masque1` | `char[]` | État du mot pour Client 1 |
| `mot_masque2` | `char[]` | État du mot pour Client 2 |
| `vies1` | `int` | Vies du Client 1 (init: 10) |
| `vies2` | `int` | Vies du Client 2 (init: 10) |
| `lettres_jouees1` | `char[]` | Lettres jouées par Client 1 |
| `lettres_jouees2` | `char[]` | Lettres jouées par Client 2 |
| `client1_actif` | `int` | 1 si Client 1 encore en jeu, 0 sinon |
| `client2_actif` | `int` | 1 si Client 2 encore en jeu, 0 sinon |
| `tour_actuel` | `int` | 1 ou 2 selon le client qui joue |

---

## 7. Exemple de Partie Complète

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
