# Protocole de Communication - Jeu du Pendu V0

## 1. Vue d'ensemble

```
┌─────────────────────────────────────────────────────────────────────────────┐
│                           JEU DU PENDU - PROTOCOLE                          │
├─────────────────────────────────────────────────────────────────────────────┤
│  CLIENT (Joueur)                              SERVEUR (Maître du jeu)       │
│  - Envoie des lettres                         - Détient le mot secret       │
│  - Affiche le résultat                        - Gère les vies               │
│  - Aucune logique de jeu                      - Toute la logique de jeu     │
└─────────────────────────────────────────────────────────────────────────────┘
```

---

## 2. Schéma de Communication

```
    CLIENT                                              SERVEUR
       │                                                   │
       │                  connect()                        │
       │ ─────────────────────────────────────────────────>│
       │                                                   │
       │                                            accept()
       │                                                   │
       │              "start 7"                            │
       │ <─────────────────────────────────────────────────│ send()
       │ recv()                                            │
       │                                                   │
       │  Affiche: "Mot à trouver: _ _ _ _ _ _ _"          │
       │                                                   │
       ├───────────────────────────────────────────────────┤
       │              BOUCLE DE JEU                        │
       ├───────────────────────────────────────────────────┤
       │                                                   │
       │  Saisie utilisateur: "E"                          │
       │                                                   │
       │              "E" (1 octet)                        │
       │ ─────────────────────────────────────────────────>│ recv()
       │ send()                                            │
       │                                                   │
       │                                     ┌─────────────┴─────────────┐
       │                                     │  Vérification lettre      │
       │                                     │  déjà jouée ?             │
       │                                     └─────────────┬─────────────┘
       │                                                   │
       │                                          ┌────────┴────────┐
       │                                          │                 │
       │                                         OUI               NON
       │                                          │                 │
       │                                          ▼                 ▼
       │                                      "ALREADY"   ┌─────────┴─────────┐
       │                                                  │ Lettre dans mot ? │
       │                                                  └─────────┬─────────┘
       │                                                            │
       │                                                   ┌────────┴────────┐
       │                                                   │                 │
       │                                                  OUI               NON
       │                                                   │                 │
       │                                                   ▼                 ▼
       │                                          Maj mot_masque           vies--
       │                                                   │                 │
       │                                          ┌────────┴────────┐        │
       │                                          │ Mot complet ?   │        │
       │                                          └────────┬────────┘        │
       │                                                   │                 │
       │                                          ┌────────┴────────┐   ┌────┴────┐
       │                                         OUI               NON  │ vies=0 ?│
       │                                          │                 │   └────┬────┘
       │                                          ▼                 ▼        │
       │                                       "WIN"             "OK"   ┌────┴────┐
       │                                                                │         │
       │                                                               OUI       NON
       │                                                                │         │
       │                                                                ▼         ▼
       │                                                             "LOSE"    "NOK"
       │                                                   │
       │         "CODE MOT VIES"                           │
       │ <─────────────────────────────────────────────────│ send()
       │ recv()                                            │
       │                                                   │
       │  Analyse du CODE reçu                             │
       │                                                   │
       │  ┌─────────────────────────────────────────────┐  │
       │  │ WIN    → Affiche "GAGNÉ!" + fin             │  │
       │  │ LOSE   → Affiche "PERDU!" + fin             │  │
       │  │ OK     → Affiche "Bien joué!" + continue    │  │
       │  │ NOK    → Affiche "Raté!" + continue         │  │
       │  │ ALREADY→ Affiche "Déjà joué!" + continue    │  │
       │  └─────────────────────────────────────────────┘  │
       │                                                   │
       │        (Si WIN ou LOSE: sortie de boucle)         │
       │        (Sinon: nouvelle itération)                │
       │                                                   │
       ├───────────────────────────────────────────────────┤
       │              FIN DE BOUCLE                        │
       ├───────────────────────────────────────────────────┤
       │                                                   │
       │                  close()                          │
       │ ─ ─ ─ ─ ─ ─ ─ ─ ─ ─ ─ ─ ─ ─ ─ ─ ─ ─ ─ ─ ─ ─ ─ ─ > │ close()
       │                                                   │
       │                                    Retour à accept() (attend
       │                                    un nouveau client)
       │                                                   │
       ▼                                                   ▼
```

---

## 3. Format des Messages

### 3.1 Message de début (Serveur → Client)

```
┌──────────────────────────────────────┐
│  Format: "start <taille_mot>"        │
│  Exemple: "start 7"                  │
│  Signification: Le mot a 7 lettres   │
└──────────────────────────────────────┘
```

### 3.2 Envoi d'une lettre (Client → Serveur)

```
┌──────────────────────────────────────┐
│  Format: 1 caractère (1 octet)       │
│  Exemple: "E" ou "e"                 │
│  Note: Converti en majuscule côté    │
│        serveur                       │
└──────────────────────────────────────┘
```

### 3.3 Réponse du serveur (Serveur → Client)

```
┌──────────────────────────────────────────────────────────┐
│  Format: "<CODE> <MOT_MASQUE> <VIES>"                    │
├──────────────────────────────────────────────────────────┤
│  CODE      │ Signification                               │
├────────────┼─────────────────────────────────────────────┤
│  WIN       │ Partie gagnée (mot complet trouvé)          │
│  LOSE      │ Partie perdue (plus de vies)                │
│  OK        │ Lettre trouvée dans le mot                  │
│  NOK       │ Lettre pas dans le mot (-1 vie)             │
│  ALREADY   │ Lettre déjà proposée auparavant             │
├──────────────────────────────────────────────────────────┤
│  Exemples:                                               │
│  "OK S_S____ 10"     → 'S' trouvé, 10 vies restantes     │
│  "NOK S_S____ 9"     → Lettre pas dans mot, 9 vies       │
│  "WIN SYSTEME 8"     → Gagné! Le mot était SYSTEME       │
│  "LOSE SYSTEME 0"    → Perdu! Le mot était SYSTEME       │
│  "ALREADY S_S____ 9" → Lettre déjà jouée                 │
└──────────────────────────────────────────────────────────┘
```

---

## 4. Description Textuelle du Protocole

### Phase 1 : Connexion

1. Le **CLIENT** établit une connexion TCP vers le **SERVEUR** (IP + Port 8000)
2. Le **SERVEUR** accepte la connexion (`accept()`)

### Phase 2 : Initialisation de la partie

3. Le **SERVEUR** initialise:
   - `mot_secret = "SYSTEME"`
   - `mot_masque = "_______"`
   - `vies = 10`
   - `lettres_jouees = []`
4. Le **SERVEUR** envoie `"start 7"` au client (`send()`)
5. Le **CLIENT** reçoit le message (`recv()`) et affiche le mot masqué

### Phase 3 : Boucle de jeu (répétée jusqu'à WIN ou LOSE)

#### Côté CLIENT:

6. Demande une lettre à l'utilisateur
7. Envoie la lettre au serveur (`send()`, 1 octet)
8. Attend la réponse (`recv()`)
9. Analyse le CODE reçu:
   - `WIN` ou `LOSE` → Affiche message de fin, sort de la boucle
   - `OK`, `NOK`, `ALREADY` → Affiche message + mot masqué, continue

#### Côté SERVEUR:

10. Reçoit la lettre (`recv()`)
11. Convertit en majuscule si nécessaire
12. **Test 1**: Lettre déjà jouée ?
    - OUI → Prépare réponse `"ALREADY mot_masque vies"`
    - NON → Continue
13. Ajoute la lettre à `lettres_jouees[]`
14. **Test 2**: Lettre dans `mot_secret` ?
    - OUI → Met à jour `mot_masque`
      - **Test 3**: `mot_masque == mot_secret` ?
        - OUI → Prépare réponse `"WIN mot_secret vies"`
        - NON → Prépare réponse `"OK mot_masque vies"`
    - NON → Décrémente `vies`
      - **Test 4**: `vies <= 0` ?
        - OUI → Prépare réponse `"LOSE mot_secret vies"`
        - NON → Prépare réponse `"NOK mot_masque vies"`
15. Envoie la réponse au client (`send()`)

### Phase 4 : Fin de partie

16. Le **CLIENT** ferme la connexion (`close()`)
17. Le **SERVEUR** ferme la socket de dialogue (`close()`)
18. Le **SERVEUR** retourne en attente d'un nouveau client (`accept()`)

---

## 5. Variables Clés

### Côté Serveur

| Variable         | Type     | Description                         |
| ---------------- | -------- | ----------------------------------- |
| `mot_secret`     | `char[]` | Le mot à deviner ("SYSTEME")        |
| `mot_masque`     | `char[]` | État actuel visible ("S_S\_\_\_\_") |
| `vies`           | `int`    | Nombre de vies restantes (init: 10) |
| `lettres_jouees` | `char[]` | Historique des lettres proposées    |
| `partie_finie`   | `int`    | Flag de fin de partie (0 ou 1)      |

### Côté Client

| Variable          | Type     | Description                       |
| ----------------- | -------- | --------------------------------- |
| `lettre`          | `char[]` | Buffer pour la saisie utilisateur |
| `messageServeur`  | `char[]` | Buffer de réception               |
| `partie_en_cours` | `int`    | Flag de continuation (1 ou 0)     |

---

## 6. Exemple de Partie Complète

```
SERVEUR                                          CLIENT
   │                                                │
   │◄───────────────── connect ─────────────────────│
   │                                                │
   │─────────── "start 7" ─────────────────────────►│ Affiche: _ _ _ _ _ _ _
   │                                                │
   │◄──────────────── "S" ──────────────────────────│ Utilisateur tape: S
   │─────────── "OK S_S____ 10" ───────────────────►│ Affiche: S _ S _ _ _ _
   │                                                │
   │◄──────────────── "Y" ──────────────────────────│ Utilisateur tape: Y
   │─────────── "OK SYS____ 10" ───────────────────►│ Affiche: S Y S _ _ _ _
   │                                                │
   │◄──────────────── "A" ──────────────────────────│ Utilisateur tape: A
   │─────────── "NOK SYS____ 9" ───────────────────►│ Affiche: Raté! (9 vies)
   │                                                │
   │◄──────────────── "T" ──────────────────────────│ Utilisateur tape: T
   │─────────── "OK SYST___ 9" ────────────────────►│ Affiche: S Y S T _ _ _
   │                                                │
   │◄──────────────── "E" ──────────────────────────│ Utilisateur tape: E
   │─────────── "OK SYSTE_E 9" ────────────────────►│ Affiche: S Y S T E _ E
   │                                                │
   │◄──────────────── "M" ──────────────────────────│ Utilisateur tape: M
   │─────────── "WIN SYSTEME 9" ───────────────────►│ Affiche: GAGNÉ!
   │                                                │
   │◄─────────────── close ─────────────────────────│
   │                                                │
```
