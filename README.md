---
title: "my_secmalloc"
description: "Outil répliquant le comportement de malloc de manière sécurisée"
date: N/A
tags:
  - Programming
  - C
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

## Ressources utilisées
- [malloc()](https://linux.die.net/man/3/malloc), [calloc()](https://linux.die.net/man/3/malloc), [realloc()](https://linux.die.net/man/3/malloc), et [free()](https://linux.die.net/man/3/malloc)
- [malloc internals](https://sourceware.org/glibc/wiki/MallocInternals)
