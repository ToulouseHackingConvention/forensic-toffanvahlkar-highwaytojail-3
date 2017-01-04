# forensic-toffanvahlkar-allnightlong-3

- Auteur : Vahlkar
- Type : Forensic

## Description

### Fichiers fournis

#### Liste des fichiers fournis (au candidat) :

- export/guest\_dump : dump de la mémoire d'une VM infectée.
- export/deb64-noflag.qcow2 : VM saine sans flag servant à utiliser le programme malveillant en environnement protégé.
- export/id_rsa.pub : Identité SSH (clé publique) à utiliser pour se connecter sur la VM sous l'utilisateur `pigeon`.
- export/id_rsa : Identité SSH (clé privée) à utiliser pour se connecter sur la VM sous l'utilisateur `pigeon`.

#### Également disponible à destination des testeurs :

**ATTENTION** : le binaire cryptolock est destiné à chiffrer le home de l'utilisateur avec une clé générée aléatoirement, ne le lancez pas en dehors d'une VM dédiée.

- build/cryptolock : Le programme malveillant, conçu pour ne se lancer que dans certaines conditions sur la VM.
- build/key : La clé utilisée pour chiffrer les données lors de la dernière exécution du programme malveillant.

## Utilisation

Dépendances du projet : **docker**, **qemu-img**, **qemu-system-x86\_64**.

Pré-requis : Télécharger la VM sur le cloud et placer l'image disque décompressée dans **vm/deb64.qcow2**.

- `make build` : construire l'image (compiler le programme malveillant, cacher le flag dans la VM saine).
- `make export` : obtenir les fichiers fournis (infecter la VM avec le programme malveillant et réaliser un dump mémoire).

### Modifier le flag

Le dossier import est transféré sur le home de la VM avec la commande `make build`.
Ce dossier est la cible de l'attaque, déclenchée avec la commande `make export`.

Le flag est dans la vidéo import/kiwi.mp4.

### Nettoyer

- `make clean` : supprime les fichiers fournis (dossier export & build).
- `make clean-all` : supprime l'image (servant à la comilation du programme malveillant) et restaure l'état de la VM à son état d'origine (sans flag).

## État

À tester (version 46e539f)

### Relecture

- vahlkar (version 46e539f)

### Test

### Déploiement

### Relecture Déploiement

### Test Déploiement

### Docker-compose

