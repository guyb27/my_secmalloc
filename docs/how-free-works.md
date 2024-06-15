## Fonctionnement de FREE

La fonction free() libère l'espace mémoire pointé par ptr, qui doit avoir été retourné par un appel précédent à malloc(), calloc() ou realloc(). Dans le cas contraire, ou si free(ptr) a déjà été appelée auparavant, un comportement indéfini se produit. Si ptr est NULL, aucune opération n'est effectuée.

### Schématisation de l'allocation de mémoire avec free

(incoming)
