CC = gcc
CFLAGS = -Wall -Wextra -g -fPIC

# Directory variables
BUILD_DIR = build
INCLUDE_DIR = include
SRC_DIR = src
LIB_DIR = $(BUILD_DIR)/lib
OBJ_DIR = $(BUILD_DIR)/obj
TEST_DIR = test

# Default make target
all: clean static dynamic script test

# Build static library
static: $(BUILD_DIR) $(LIB_DIR) $(OBJ_DIR) $(OBJ_DIR)/my_secmalloc.o
	ar rcs $(LIB_DIR)/libmy_secmalloc.a $(OBJ_DIR)/my_secmalloc.o

# Build dynamic library
dynamic: CFLAGS += -DDYNAMIC
dynamic: $(BUILD_DIR) $(LIB_DIR) $(OBJ_DIR) $(OBJ_DIR)/my_secmalloc.o
	$(CC) -shared -o $(LIB_DIR)/libmy_secmalloc.so $(OBJ_DIR)/my_secmalloc.o

# Build script for using the dynamic library
script: $(BUILD_DIR)/my_secmalloc.sh

# Shell script to run commands with the library
$(BUILD_DIR)/my_secmalloc.sh: $(BUILD_DIR)
	echo '#!/bin/bash' > $(BUILD_DIR)/my_secmalloc.sh
	echo '' >> $(BUILD_DIR)/my_secmalloc.sh
	echo '# Chemin vers la bibliothèque my_secmalloc.so' >> $(BUILD_DIR)/my_secmalloc.sh
	echo 'LIB_PATH="./build/lib/libmy_secmalloc.so"' >> $(BUILD_DIR)/my_secmalloc.sh
	echo '' >> $(BUILD_DIR)/my_secmalloc.sh
	echo '# Vérifier si la bibliothèque existe' >> $(BUILD_DIR)/my_secmalloc.sh
	echo 'if [ ! -f "$$LIB_PATH" ]; alors' >> $(BUILD_DIR)/my_secmalloc.sh
	echo '  echo "Erreur : la bibliothèque $$LIB_PATH n'\''existe pas. Veuillez compiler le projet en lançant un make à sa racine."' >> $(BUILD_DIR)/my_secmalloc.sh
	echo '  exit 1' >> $(BUILD_DIR)/my_secmalloc.sh
	echo 'fi' >> $(BUILD_DIR)/my_secmalloc.sh
	echo '' >> $(BUILD_DIR)/my_secmalloc.sh
	echo '# Définir LD_PRELOAD pour utiliser la bibliothèque my_secmalloc' >> $(BUILD_DIR)/my_secmalloc.sh
	echo 'export LD_PRELOAD="$$LIB_PATH"' >> $(BUILD_DIR)/my_secmalloc.sh
	echo '' >> $(BUILD_DIR)/my_secmalloc.sh
	echo '# Exécuter la commande passée en argument dans une nouvelle instance de bash' >> $(BUILD_DIR)/my_secmalloc.sh
	echo 'exec bash -i -c "$$@"' >> $(BUILD_DIR)/my_secmalloc.sh
	chmod +x $(BUILD_DIR)/my_secmalloc.sh

# Build and run tests
test: static $(BUILD_DIR)/test/test

# Compile and execute tests
$(BUILD_DIR)/test/test: $(BUILD_DIR) $(LIB_DIR) $(OBJ_DIR) $(BUILD_DIR)/test $(OBJ_DIR)/my_secmalloc.o
	$(CC) $(CFLAGS) -I$(INCLUDE_DIR) $(TEST_DIR)/test.c -L$(LIB_DIR) -lmy_secmalloc -lcriterion -Wl,-rpath,$(LIB_DIR) -o $@
	LD_LIBRARY_PATH=$(LIB_DIR) ./$(BUILD_DIR)/test/test

# Create necessary directories
$(BUILD_DIR) $(LIB_DIR) $(OBJ_DIR) $(BUILD_DIR)/test:
	mkdir -p $@

# Compile source file to object file
$(OBJ_DIR)/my_secmalloc.o: $(SRC_DIR)/my_secmalloc.c
	$(CC) $(CFLAGS) -I$(INCLUDE_DIR) -c $< -o $@

# Clean build files
clean:
	rm -rf $(BUILD_DIR)
