# Nombre del compilador
CXX = g++
# Flags del compilador (C++17 + todas las advertencias)
CXXFLAGS = -std=c++20 -Wall -Wextra
# Archivo fuente principal
SRC = src/main.cpp
# Nombre del ejecutable de salida
TARGET = arbol-b-optimizado

# Regla por defecto
all: $(TARGET)

# Cómo construir el ejecutable
$(TARGET): $(SRC)
	$(CXX) $(CXXFLAGS) $(SRC) -o $(TARGET)

# Limpiar archivos generados
clean:
	rm -f $(TARGET)
