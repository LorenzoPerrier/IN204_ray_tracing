# Nom de l'exécutable
EXEC = raytracer

# Répertoires pour les sources et les en-têtes
SRCDIR = Sources
INCDIR = Headers

# Liste des fichiers sources et en-têtes
SOURCES = $(wildcard $(SRCDIR)/*.cpp)
HEADERS = $(wildcard $(INCDIR)/*.hpp)

# Options pour le compilateur
CXX = g++
CXXFLAGS = -Wall -Wno-reorder

# Commande de construction de l'exécutable
$(EXEC): $(SOURCES) $(HEADERS)
	$(CXX) $(CXXFLAGS) $(SOURCES) -lgdi32 -fopenmp -lpthread -o $(EXEC)

.PHONY: clean
clean:
	rm -f $(EXEC)
