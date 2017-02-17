# Highway to jail - part. 1

- Auteur : Vahlkar
- Type : Forensic

## Challenge
### Nom de production
Highway to jail - part. 1

### Descripion du challenge

Ce challenge est une sous partie du challenge
misc-toffanvahlkar-highwaytojail-1.  Le dépot permet de générer un cryptolocker
(malware qui chiffre les données de l'utilisateur en échange d'une rançon).
Le but du challenge est d'effectuer une analyse forensique de la machine infectée
afin de retrouver la clé utilisée pour chiffre les données et donc les
déchiffrer.

### Description participant

Voir misc-toffanvahlkar-highwaytojail-1.

### Fichiers fournis

**ATTENTION** : le binaire cryptolock est destiné à chiffrer le home de
l'utilisateur avec une clé générée aléatoirement, ne le lancez pas en dehors
d'une VM dédiée.

- `export/cryptolock` : programme malveillant utilisé dans
  misc-toffanvahlkar-highwaytojail-1, ne pas le mettre directement à
  disposition des participants.

#### Remarques

Le programme malveillant est conçu pour ne se lancer que dans certaines
conditions sur la VM.  La clé utilisée pour chiffrer les données est envoyée
une fois lors d'une connexion à la VM sur le port `54321` (partie gérée par
misc-toffanvahlkar-highwaytojail-1). La clé n'est bien évidemment pas à fournir
aux participants (le but du challenge étant de la retrouver) mais celle-ci peut
servir à des fins de debug / test du challenge.

### Changement de flag

Le flag est défini dans le dépot misc-toffanvahlkar-highwaytojail-1, ce dépot
n'en étant qu'une sous partie. Pour plus d'informations voir
misc-toffanvahlkar-highwaytojail-1.

### Usage

Dépendances du projet : **docker**, **qemu-img**, **qemu-system-x86\_64**.

`make export` pour compiler le binaire. Celui-ci est utilisé par
misc-toffanvahlkar-highwaytojail-1.

### Nettoyer

- `make clean` : supprime les fichiers fournis (dossier `export`).
- `make clean-all` : supprime l'image (servant à la comilation du programme malveillant).

### Situation

| Relecture | Construction | Test | Déploiement |
| --- | --- | --- | --- |
| toffan() | toffan() | toffan() | |
| Vahlkar(version 46e539f) | Vahlkar() | Vahlkar () | |
| | | | |

### Tests

Des scripts de solution semi-automatiques sont disponibles dans les dossiers
`solution` de chaque dépot.
