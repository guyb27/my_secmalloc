# Variables de compilation
CC = gcc
CFLAGS = -Wall -Wextra -g -fPIC
LDFLAGS_SHARED = -shared -Wl,--version-script=libmy_secmalloc.map

# Répertoires
BUILD_DIR = build
INCLUDE_DIR = include
SRC_DIR = src
LIB_DIR = $(BUILD_DIR)/lib
OBJ_DIR = $(BUILD_DIR)/obj
TEST_DIR = test

# Cibles par défaut
.PHONY: all clean static dynamic script test

all: clean static dynamic script test

# Construction de la bibliothèque statique
static: $(BUILD_DIR) $(LIB_DIR) $(OBJ_DIR) $(LIB_DIR)/libmy_secmalloc.a

$(LIB_DIR)/libmy_secmalloc.a: $(OBJ_DIR)/my_secmalloc.o
	ar rcs $@ $^

# Construction de la bibliothèque dynamique
dynamic: CFLAGS += -DDYNAMIC
dynamic: $(BUILD_DIR) $(LIB_DIR) $(OBJ_DIR) $(LIB_DIR)/libmy_secmalloc.so

$(LIB_DIR)/libmy_secmalloc.so: $(OBJ_DIR)/my_secmalloc.o
	$(CC) $(LDFLAGS_SHARED) -o $@ $^

# Construction du script pour utiliser la bibliothèque dynamique
script: $(BUILD_DIR)/my_secmalloc.sh

$(BUILD_DIR)/my_secmalloc.sh: $(BUILD_DIR)
	@echo "Création du script my_secmalloc.sh..."
	@echo '#!/bin/bash' > $@
	@echo '# Chemin vers la bibliothèque my_secmalloc.so' >> $@
	@echo 'LIB_PATH="./build/lib/libmy_secmalloc.so"' >> $@
	@echo '# Vérifier si la bibliothèque existe' >> $@
	@echo 'if [ ! -f "$$LIB_PATH" ]; then' >> $@
	@echo '  echo "Erreur : la bibliothèque $$LIB_PATH n'\''existe pas. Veuillez compiler le projet en lançant un make à sa racine."' >> $@
	@echo '  exit 1' >> $@
	@echo 'fi' >> $@
	@echo '# Définir LD_PRELOAD pour utiliser la bibliothèque my_secmalloc' >> $@
	@echo 'export LD_PRELOAD="$$LIB_PATH"' >> $@
	@echo '# Exécuter la commande passée en argument dans une nouvelle instance de bash' >> $@
	@echo 'exec bash -i -c "$$@"' >> $@
	@chmod +x $@

# Construction et exécution des tests
test: static $(BUILD_DIR)/test/test

$(BUILD_DIR)/test/test: $(BUILD_DIR) $(LIB_DIR) $(OBJ_DIR) $(BUILD_DIR)/test $(OBJ_DIR)/my_secmalloc.o
	$(CC) $(CFLAGS) -I$(INCLUDE_DIR) $(TEST_DIR)/test.c -L$(LIB_DIR) -lmy_secmalloc -lcriterion -Wl,-rpath,$(LIB_DIR) -o $@
	LD_LIBRARY_PATH=$(LIB_DIR) ./$@

# Création des répertoires nécessaires
$(BUILD_DIR) $(LIB_DIR) $(OBJ_DIR) $(BUILD_DIR)/test:
	mkdir -p $@

# Compilation du fichier source en fichier objet
$(OBJ_DIR)/my_secmalloc.o: $(SRC_DIR)/my_secmalloc.c
	$(CC) $(CFLAGS) -I$(INCLUDE_DIR) -c $< -o $@

# Nettoyage des fichiers de construction
clean:
	rm -rf $(BUILD_DIR)

# Message pour une cible non trouvée
%:
	@echo "La cible '$@' n'existe pas."
	@exit 1
