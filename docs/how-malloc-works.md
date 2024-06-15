## Fonctionnement de MALLOC

La fonction malloc() alloue une taille d'octets et renvoie un pointeur sur la mémoire allouée. La mémoire n'est pas initialisée. Si la taille est égale à 0, malloc() renvoie soit NULL, soit une valeur de pointeur unique qui peut ensuite être passée avec succès à free().

### Schématisation de l'allocation de mémoire avec malloc

(incoming)
