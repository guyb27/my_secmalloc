## Fonctionnement de CALLOC
La fonction calloc() alloue de la mémoire pour un tableau de nmemb éléments de taille octets chacun et renvoie un pointeur sur la mémoire allouée. La mémoire est mise à zéro. Si nmemb ou size est égal à 0, calloc() renvoie soit NULL, soit un pointeur unique qui peut être passé avec succès à free().
Elle alloue de la mémoire pour un tableau d'éléments nmemb de taille octets chacun et renvoie un pointeur sur la mémoire allouée. La mémoire est mise à zéro. Si nmemb ou size est 0, calloc() renvoie soit NULL, soit une valeur de pointeur unique qui peut ensuite être passée avec succès à free().

### Schématisation de l'allocation de mémoire avec calloc
(incoming)
