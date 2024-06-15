---
title: "my_secmalloc"
description: "Outil répliquant le comportement de malloc de manière sécurisée"
date: N/A
tags:
  - Programming
  - C
  - Linux
---

# my_secmalloc
Projet en C répliquant les fonctionnalités de malloc de manière sécurisée afin de détecter l'exploitation de certaines vulnérabilités de type double free, heap overflow ou encore memory leak. 

## Comment setup le projet
### Pré-requis
Il vous sera nécessaire de vérifier que chacun de ces outils sont installés sur votre appareil :
- [git](https://git-scm.com/download/win)
- [make](https://cmake.org/download/)
- [criterion](https://criterion.readthedocs.io/en/master/setup.html) build et installé dans le `/usr` avec ses dépendances
- [gef](https://github.com/hugsy/gef) et [valgrind](https://valgrind.org/downloads/current.html) pour le debugging et qui vous seront donc d'une grande aide si vous souhaitez contribuer au projet

### Etapes pour construire le projet et accéder à l'exécutable
#### Construction du projet
```bash
git clone git@github.com:guyb27/my_secmalloc.git
cd my_secmalloc/my_secmalloc
```
**Ensuite vous avez plusieurs options pour construire le projet :**
- ``make static`` qui vous donnera la librairie statique ``libmy_secmalloc.a`` dans le dossier ``./build/lib``
- ``make dynamic`` qui vous donnera la librairie dynamique ``libmy_secmalloc.so`` dans le dossier ``./build/lib``
- ``make test`` qui vous donnera le binaire de test ``test`` dans le dossier ``./build/test``, il fonctionne grâce à la lib statique
- ``make script`` qui vous donnera un script ``my_secmalloc.sh`` dans le dossier ``./build`` qui permet de lancer un programme avec l'allocateur sans besoin de spécifier ``LD_PRELOAD``
- ``make all`` qui permet de construire les deux librairies et exécute par la même occasion les tests en construisant le binaire de test
  *(actuellement le make all ne construit pas la librairie dynamique de la bonne manière, en effet si on fait un make all et regarde les symboles de
   la librairie dynamique, on voit que les fonctions de malloc, free, realloc et calloc ne sont pas présentes, il faudra donc faire un make dynamic 
   ou make clean dynamic (si la lib.so a déjà été build) pour avoir une librairie dynamique fonctionnelle, un correctif est en cours de développement)*

#### Comment utiliser l'allocateur avec un utilitaire/programme ?
Par défaut, le programme enregistre ses logs dans un fichier par défaut nommé ``default_log.txt``. Le fichier de log est déterminé grâce à la variable d'environnement ``MSM_OUTPUT`` que vous êtes libre de modifier.

Il existe deux principales options pour utiliser l'allocateur avec un utilitaire/programme :
- ``LD_PRELOAD`` qui permet de charger la librairie dynamique avant les autres librairies :
```bash
LD_PRELOAD=./build/lib/libmy_secmalloc.so sl
```

- Utiliser le script à ``./build/my_secmalloc.sh`` ayant été créé via un ``make all`` ou ``make script`` qui permet de lancer un programme avec l'allocateur :
```bash
./build/my_secmalloc.sh python3
```
Pour l'instant le script n'est pas optimisé.

## Ressources utilisées
- [malloc()](https://linux.die.net/man/3/malloc), [calloc()](https://linux.die.net/man/3/malloc), [realloc()](https://linux.die.net/man/3/malloc), et [free()](https://linux.die.net/man/3/malloc)
- [malloc internals](https://sourceware.org/glibc/wiki/MallocInternals)
