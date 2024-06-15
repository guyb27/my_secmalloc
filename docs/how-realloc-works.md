## Fonctionnement de REALLOC

La fonction realloc() modifie la taille du bloc de mémoire indiqué par ptr en fonction de la taille des octets. Le contenu sera inchangé entre le début de la région et le minimum de l'ancienne et de la nouvelle taille. Si la nouvelle taille est supérieure à l'ancienne, la mémoire ajoutée ne sera pas initialisée. Si ptr est NULL, l'appel est équivalent à malloc(size), pour toutes les valeurs de size ; si size est égal à zéro, et que ptr n'est pas NULL, l'appel est équivalent à free(ptr). À moins que ptr ne soit NULL, il doit avoir été retourné par un appel antérieur à malloc(), calloc() ou realloc(). Si la zone pointée a été déplacée, un free(ptr) est effectué.

### Schématisation de l'allocation de mémoire avec realloc

(incoming)
