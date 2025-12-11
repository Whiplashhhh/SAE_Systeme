# Jeu du Pendu - V0

Jeu du pendu en réseau avec sockets TCP.

## Auteurs
- Willem VANBAELINGHEM--DEZITTER
- Thomas TEITEN
- Alex FRANCOIS
- Romain THEOBALD

## Compilation

```bash
gcc PN_serveur_V0.c -o PN_serveur_V0.exe
gcc PN_client_V0.c -o PN_client_V0.exe
```

## Utilisation

1. Lancer le serveur :
```bash
./PN_serveur_V0.exe
```

2. Lancer le client :
```bash
./PN_client_V0.exe 127.0.0.1 8000
```

## Règles
- 10 vies pour deviner le mot
- Le mot est "SYSTEME" (fixe dans cette version)
