## Pseudo-fonctionnement de la heap lors d’appels de certaines fonctions (mmap, munmap, mremap, calloc, malloc, free)

```cpp
[MD] = Méta-données
[DA] = Données allouées
[ES] = Espace libre

Initialisation de la Heap avec mmap :
+------------------------------------------------------------------------------------+
| [MD] 0x00002000 à 0x0000200F: Méta-données: Flag=LIBRE(FREE), Taille=8192          |
| [ES] 0x00002010 à 0x00002FFF: Espace Libre (8192 octets initialement alloués)      |
+------------------------------------------------------------------------------------+

Allocation avec malloc (demande de taille 1024 octets, puis 2048 octets) :
+------------------------------------------------------------------------------------+
| [MD] 0x00002010 à 0x0000201B: Méta-données: Flag=OCCUPÉ(BUSY), Taille=1024         |
| [DA] 0x0000201C à 0x0000205B: Données Allouées (1024 octets)                       |
+------------------------------------------------------------------------------------+
| [MD] 0x0000205C à 0x00002067: Méta-données: Flag=OCCUPÉ(BUSY), Taille=2048         |
| [DA] 0x00002068 à 0x000020E7: Données Allouées (2048 octets)                       |
+------------------------------------------------------------------------------------+
| [MD] 0x000020E8 à 0x000020F3: Méta-données: Flag=LIBRE(FREE), Taille=5120          |
| [ES] 0x000020F4 à 0x00002FFF: Espace Libre Restant (5120 octets)                   |
+------------------------------------------------------------------------------------+

Demande de nouvelle allocation qui nécessite plus d'espace (demande de 6000 octets) :
+------------------------------------------------------------------------------------+
| [MD] 0x000020E8 à 0x000020F3: Méta-données: Flag=LIBRE(FREE), Taille=5120          |
| [ES] 0x000020F4 à 0x00002FFF: Espace Libre Insuffisant pour nouvelle demande       |
+------------------------------------------------------------------------------------+

Application de mremap pour agrandir la mémoire (lastHeapSizeGeneratedByMmap*2) :
+------------------------------------------------------------------------------------+
| [MD] 0x00002010 à 0x0000201B: Méta-données: Flag=OCCUPÉ(BUSY), Taille=1024         |
| [DA] 0x0000201C à 0x0000205B: Données Allouées (1024 octets)                       |
+------------------------------------------------------------------------------------+
| [MD] 0x0000205C à 0x00002067: Méta-données: Flag=OCCUPÉ(BUSY), Taille=2048         |
| [DA] 0x00002068 à 0x000020E7: Données Allouées (2048 octets)                       |
+------------------------------------------------------------------------------------+
| [MD] 0x000020E8 à 0x000020F3: Méta-données: Flag=LIBRE(FREE), Taille=13312         |
| [ES] 0x000020F4 à 0x00003FFF: Espace Libre Agrandi                                 |
| (13312 octets désormais disponibles car 16384 - 3072 = 13312)                      |
+------------------------------------------------------------------------------------+

Allocation avec malloc pour la demande de 6000 octets :
+------------------------------------------------------------------------------------+
| [MD] 0x00002010 à 0x0000201B: Méta-données: Flag=OCCUPÉ(BUSY), Taille=1024         |
| [DA] 0x0000201C à 0x0000205B: Données Allouées (1024 octets)                       |
+------------------------------------------------------------------------------------+
| [MD] 0x0000205C à 0x00002067: Méta-données: Flag=OCCUPÉ(BUSY), Taille=2048         |
| [DA] 0x00002068 à 0x000020E7: Données Allouées (2048 octets)                       |
+------------------------------------------------------------------------------------+
| [MD] 0x000020E8 à 0x000020F3: Méta-données: Flag=OCCUPÉ(BUSY), Taille=6000         |
| [DA] 0x000020F4 à 0x000023F3: Données nouvellement allouées (6000 octets)          |
+------------------------------------------------------------------------------------+
| [MD] 0x000023F4 à 0x00002401: Méta-données: Flag=LIBRE(FREE), Taille=7312          |
| [ES] 0x00002402 à 0x00003FFF: Espace libre restant après nouvelle allocation       |
+------------------------------------------------------------------------------------+

Demande de nouvelle allocation avec calloc pour 4000 octets :
+------------------------------------------------------------------------------------+
| [MD] 0x000023F4 à 0x00002401: Méta-données: Flag=LIBRE(FREE), Taille=7312          |
| [ES] 0x00002402 à 0x00003FFF: Espace Libre disponible pour calloc                  |
+------------------------------------------------------------------------------------+

Application de calloc pour 4000 octets :
+------------------------------------------------------------------------------------+
| [MD] 0x000023F4 à 0x00002401: Méta-données: Flag=OCCUPÉ(BUSY), Taille=4000         |
| [DA] 0x00002402 à 0x0000268D: Données Allouées et initialisées à zéro (4000 octets)|
+------------------------------------------------------------------------------------+
| [MD] 0x0000268E à 0x00002699: Méta-données: Flag=LIBRE(FREE), Taille=3312          |
| [ES] 0x0000269A à 0x00003FFF: Espace Libre Restant après calloc (3312 octets)      |
+------------------------------------------------------------------------------------+

Libération avec free (tous les blocs) :
+------------------------------------------------------------------------------------+
| [MD] 0x00002010 à 0x0000201B: Méta-données: Flag=LIBRE(FREE), Taille=1024          |
| [ES] 0x0000201C à 0x0000202B: Espace Libéré (1024 octets libérés)                  |
+------------------------------------------------------------------------------------+
| [MD] 0x0000205C à 0x00002067: Méta-données: Flag=LIBRE(FREE), Taille=2048          |
| [ES] 0x00002068 à 0x00002077: Espace Libéré (2048 octets libérés)                  |
+------------------------------------------------------------------------------------+
| [MD] 0x000020E8 à 0x000020F3: Méta-données: Flag=LIBRE(FREE), Taille=6000          |
| [ES] 0x000020F4 à 0x000023F3: Espace Libéré (6000 octets libérés)                  |
+------------------------------------------------------------------------------------+
| [MD] 0x000023F4 à 0x00002401: Méta-données: Flag=OCCUPÉ(BUSY), Taille=4000         |
| [DA] 0x00002402 à 0x0000268D: Données Allouées et initialisées à zéro (4000 octets)|
+------------------------------------------------------------------------------------+
| [MD] 0x0000268E à 0x00002699: Méta-données: Flag=LIBRE(FREE), Taille=3312          |
| [ES] 0x0000269A à 0x00003FFF: Espace Libre Restant après calloc (3312 octets)      |
+------------------------------------------------------------------------------------+

Fusion des blocs libres (car ils sont adjacents et tous libres) :
+------------------------------------------------------------------------------------+
| [MD] 0x00002000 à 0x0000200F: Méta-données: Flag=LIBRE(FREE), Taille=16384         |
| [ES] 0x00002010 à 0x00003FFF: Espace Libre Fusionné                                |
| (tous les blocs libérés fusionnés en un seul de 16384)                             |
+------------------------------------------------------------------------------------+

Application de munmap après la fusion complète :
+------------------------------------------------------------------------------------+
| La mémoire de 16384 octets est rendue au système via munmap.                       |
| [MD] et [ES] sont non applicables.                                                 |
| Le bloc de mémoire n'est plus sous la gestion du système d'allocation, donc        |
| signifie que toute l'information de gestion associée à cet espace est effacée      |
| ou devient irrélevante, car l'espace est physiquement rendu au système.            |
+------------------------------------------------------------------------------------+
```

### Structure générale d'un chunk

- Méta-données :
  - Taille :
    - Indique la taille du chunk, souvent en octets. Cette information est cruciale pour savoir combien de mémoire le chunk occupe, ce qui est nécessaire lors de l'allocation ou de la libération de la mémoire.
  - Flag d'état :
    - Marque le chunk comme étant libre (FREE) ou occupé (BUSY). Cette information est utilisée par le gestionnaire de mémoire pour déterminer quels chunks sont disponibles pour de nouvelles allocations.
  - Pointeurs de contrôle :
    - Précédent (Prev): Dans certains gestionnaires, comme ceux utilisant des listes doublement chaînées, un pointeur vers le chunk précédent est nécessaire pour faciliter les opérations de fusion des chunks libres.
    - Suivant (Next): Pointeur vers le prochain chunk, utilisé de manière similaire au pointeur précédent, essentiel pour parcourir la liste de chunks lors de l'allocation et de la libération.

- Données :
  - Pour un chunk occupé :
    - Cet espace contient les données effectivement allouées par l'application. La quantité de données ici est déterminée par la taille du chunk moins la taille occupée par les métadonnées.
  - Pour un chunk libre :
    - Cet espace ne contient pas de données spécifiques et est considéré comme disponible pour les futures allocations. Dans certains gestionnaires de mémoire, les chunks libres peuvent également stocker des informations supplémentaires utiles pour la gestion de la mémoire libre, telles que des pointeurs vers d'autres chunks libres (similaires aux pointeurs de contrôle mais spécifiques à la gestion de l'espace libre).
