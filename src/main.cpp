// Árbol B* (optimizado) en C++20
// Implementación con redistribución antes de división y división en 3 nodos cuando es necesario
// Funciones públicas: insertar, buscar, imprimir ascendente/descendente, verificar si es válido

#include <iostream>
#include <vector>
#include <memory>
#include <algorithm>
using namespace std;

const int T = 3; // Grado mínimo

class NodoBStar {
public:
    bool hoja;
    vector<int> claves;
    vector<shared_ptr<NodoBStar>> hijos;

    NodoBStar(bool esHoja) : hoja(esHoja) {}
};

bool indiceValido(size_t i, const vector<shared_ptr<NodoBStar>>& hijos) {
    return i < hijos.size();
}

class ArbolBStar {
private:
    shared_ptr<NodoBStar> raiz;

    // Recursiva. Complejidad: O(n), donde n es el número de claves en el árbol
    void imprimirAsc(shared_ptr<NodoBStar> nodo) {
        if (!nodo) return;
        size_t i;
        for (i = 0; i < nodo->claves.size(); ++i) {
            if (!nodo->hoja) imprimirAsc(nodo->hijos[i]);
            cout << nodo->claves[i] << " ";
        }
        if (!nodo->hoja) imprimirAsc(nodo->hijos[i]);
    }

    // Recursiva. Complejidad: O(n)
    void imprimirDesc(shared_ptr<NodoBStar> nodo) {
        if (!nodo) return;
        int i;
        for (i = nodo->claves.size(); i > 0; --i) {
            if (!nodo->hoja) imprimirDesc(nodo->hijos[i]);
            cout << nodo->claves[i - 1] << " ";
        }
        if (!nodo->hoja) imprimirDesc(nodo->hijos[0]);
    }

    // Recursiva. Complejidad: O(log n) en promedio, O(n) en el peor caso
    shared_ptr<NodoBStar> buscar(shared_ptr<NodoBStar> nodo, int clave) {
        if (!nodo) return nullptr;
        size_t i = 0;
        while (static_cast<size_t>(i) < nodo->claves.size() && clave > nodo->claves[i]) i++;
        if (i < static_cast<size_t>(nodo->claves.size()) && clave == nodo->claves[i]) return nodo;
        if (nodo->hoja) return nullptr;
        return buscar(nodo->hijos[i], clave);
    }

    // Recursiva. Complejidad: O(n)
    bool esValido(shared_ptr<NodoBStar> nodo, int prof, int& nivelHoja) {
        if (nodo == raiz && nodo->hoja && nodo->claves.empty()) return true;
        if (!nodo->hoja && nodo->hijos.size() != nodo->claves.size() + 1) return false;
        int min_claves = (nodo == raiz) ? 1 : (2*T/3);
        if (nodo->hoja) {
            if (nivelHoja == -1) nivelHoja = prof;
            return nivelHoja == prof && static_cast<int>(nodo->claves.size()) >= min_claves;
        }
        if (static_cast<int>(nodo->claves.size()) < min_claves || static_cast<int>(nodo->claves.size()) > 2*T - 1)
            return false;
        for (auto hijo : nodo->hijos)
            if (!esValido(hijo, prof + 1, nivelHoja)) return false;
        return true;
    }

    // Iterativa. Complejidad: O(t), donde t es el grado mínimo (T)
    void redistribuirOdividir(shared_ptr<NodoBStar> padre, int i) {
        if (!indiceValido(i + 1, padre->hijos)) {
            cerr << "Error: acceso fuera de rango en redistribuirOdividir, índice: " << i << endl;
            return;
        }
        auto y = padre->hijos[i];
        auto z = padre->hijos[i+1];
        if (z->claves.size() < 2*T - 1) {
            int total = y->claves.size() + z->claves.size();
            vector<int> temp;
            temp.reserve(y->claves.size() + z->claves.size() + 1);
            temp.insert(temp.end(), y->claves.begin(), y->claves.end());
            temp.push_back(padre->claves[i]);
            temp.insert(temp.end(), z->claves.begin(), z->claves.end());
            int k = total / 2;
            y->claves.assign(temp.begin(), temp.begin() + k);
            padre->claves[i] = temp[k];
            z->claves.assign(temp.begin() + k + 1, temp.end());
        } else {
            auto w = make_shared<NodoBStar>(y->hoja);
            vector<int> total;
            total.insert(total.end(), y->claves.begin(), y->claves.end());
            total.push_back(padre->claves[i]);
            total.insert(total.end(), z->claves.begin(), z->claves.end());
            int t1 = total.size() / 3;
            int t2 = 2 * total.size() / 3;
            y->claves.assign(total.begin(), total.begin() + t1);
            padre->claves[i] = total[t1];
            w->claves.assign(total.begin() + t1 + 1, total.begin() + t2);
            padre->claves.insert(padre->claves.begin() + i + 1, total[t2]);
            z->claves.assign(total.begin() + t2 + 1, total.end());
            padre->hijos.insert(padre->hijos.begin() + i + 1, w);
        }
    }

    // Recursiva. Complejidad: O(log n)
    void insertarNoLleno(shared_ptr<NodoBStar> nodo, int clave) {
        if (!nodo) {
            cerr << "Error: nodo nulo en insertarNoLleno." << endl;
            return;
        }
        int i = nodo->claves.size() - 1;
        if (nodo->hoja) {
            nodo->claves.push_back(0);
            while (i >= 0 && clave < nodo->claves[i]) {
                nodo->claves[i + 1] = nodo->claves[i];
                --i;
            }
            nodo->claves[i + 1] = clave;
        } else {
            while (i >= 0 && clave < nodo->claves[i]) --i;
            ++i;
            if (!indiceValido(i, nodo->hijos)) {
                cerr << "Error: índice hijo fuera de rango antes de comprobar si está lleno." << endl;
                return;
            }
            if (nodo->hijos[i]->claves.size() == 2*T - 1) {
                if (indiceValido(i + 1, nodo->hijos))
                    redistribuirOdividir(nodo, i);
                if (i < static_cast<int>(nodo->claves.size()) && clave > nodo->claves[i]) ++i;
            }
            if (!indiceValido(i, nodo->hijos)) {
                cerr << "Error: índice hijo fuera de rango en insertarNoLleno." << endl;
                return;
            }
            insertarNoLleno(nodo->hijos[i], clave);
        }
    }

public:
    ArbolBStar() { raiz = make_shared<NodoBStar>(true); }

    // Recursiva. Complejidad: O(log n) en promedio
    void borrar(int clave) {
        borrar(raiz, clave);
        if (raiz->claves.empty() && !raiz->hoja) {
            raiz = raiz->hijos[0];
        }
    }

    // Recursiva. Complejidad: O(log n)
    void borrar(shared_ptr<NodoBStar> nodo, int clave) {
        int i = 0;
        while (static_cast<size_t>(i) < nodo->claves.size() && clave > nodo->claves[i]) ++i;

        if (static_cast<size_t>(i) < nodo->claves.size() && nodo->claves[i] == clave) {
            if (nodo->hoja) {
                nodo->claves.erase(nodo->claves.begin() + i);
            } else {
                borrarDeInterno(nodo, i);
            }
        } else {
            if (nodo->hoja) return;

            bool ultima = (static_cast<size_t>(i) == nodo->claves.size());
            if (nodo->hijos[i]->claves.size() < T) {
                llenar(nodo, i);
                if (ultima && static_cast<size_t>(i) > nodo->claves.size()) --i;
            }
            borrar(nodo->hijos[i], clave);
        }
    }

    // Recursiva. Complejidad: O(log n)
    void borrarDeInterno(shared_ptr<NodoBStar> nodo, int idx) {
        int clave = nodo->claves[idx];

        if (nodo->hijos[idx]->claves.size() >= T) {
            int predecesor = obtenerPredecesor(nodo, idx);
            nodo->claves[idx] = predecesor;
            borrar(nodo->hijos[idx], predecesor);
        } else if (nodo->hijos[idx+1]->claves.size() >= T) {
            int sucesor = obtenerSucesor(nodo, idx);
            nodo->claves[idx] = sucesor;
            borrar(nodo->hijos[idx+1], sucesor);
        } else {
            fusionar(nodo, idx);
            borrar(nodo->hijos[idx], clave);
        }
    }

    // Iterativa. Complejidad: O(log n)
    int obtenerPredecesor(shared_ptr<NodoBStar> nodo, int idx) {
        auto actual = nodo->hijos[idx];
        while (!actual->hoja)
            actual = actual->hijos.back();
        return actual->claves.back();
    }

    // Iterativa. Complejidad: O(log n)
    int obtenerSucesor(shared_ptr<NodoBStar> nodo, int idx) {
        auto actual = nodo->hijos[idx+1];
        while (!actual->hoja)
            actual = actual->hijos.front();
        return actual->claves.front();
    }

    // Recursiva. Complejidad: O(t), donde t es el grado mínimo (T)
    void llenar(shared_ptr<NodoBStar> nodo, int idx) {
        if (idx != 0 && nodo->hijos[idx-1]->claves.size() >= T)
            prestarDeIzquierda(nodo, idx);
        else if (static_cast<size_t>(idx) != nodo->claves.size() && nodo->hijos[idx+1]->claves.size() >= T)
            prestarDeDerecha(nodo, idx);
        else {
            if (static_cast<size_t>(idx) != nodo->claves.size())
                fusionar(nodo, idx);
            else
                fusionar(nodo, idx-1);
        }
    }

    // Iterativa. Complejidad: O(1)
    void prestarDeIzquierda(shared_ptr<NodoBStar> nodo, int idx) {
        auto hijo = nodo->hijos[idx];
        auto hermano = nodo->hijos[idx - 1];

        hijo->claves.insert(hijo->claves.begin(), nodo->claves[idx - 1]);
        nodo->claves[idx - 1] = hermano->claves.back();
        hermano->claves.pop_back();

        if (!hijo->hoja) {
            hijo->hijos.insert(hijo->hijos.begin(), hermano->hijos.back());
            hermano->hijos.pop_back();
        }
    }

    // Iterativa. Complejidad: O(1)
    void prestarDeDerecha(shared_ptr<NodoBStar> nodo, int idx) {
        auto hijo = nodo->hijos[idx];
        auto hermano = nodo->hijos[idx + 1];

        hijo->claves.push_back(nodo->claves[idx]);
        nodo->claves[idx] = hermano->claves.front();
        hermano->claves.erase(hermano->claves.begin());

        if (!hijo->hoja) {
            hijo->hijos.push_back(hermano->hijos.front());
            hermano->hijos.erase(hermano->hijos.begin());
        }
    }

    // Iterativa. Complejidad: O(t), donde t es el grado mínimo (T)
    void fusionar(shared_ptr<NodoBStar> nodo, int idx) {
        auto hijo = nodo->hijos[idx];
        auto hermano = nodo->hijos[idx + 1];

        hijo->claves.push_back(nodo->claves[idx]);
        hijo->claves.insert(hijo->claves.end(), hermano->claves.begin(), hermano->claves.end());

        if (!hijo->hoja)
            hijo->hijos.insert(hijo->hijos.end(), hermano->hijos.begin(), hermano->hijos.end());

        nodo->claves.erase(nodo->claves.begin() + idx);
        nodo->hijos.erase(nodo->hijos.begin() + idx + 1);
    }

    // Iterativa y recursiva (internamente). Complejidad: O(log n)
    void insertar(int clave) {
        if (raiz->claves.size() == 2*T - 1) {
            auto nuevaRaiz = make_shared<NodoBStar>(false);
            auto hijoIzq = make_shared<NodoBStar>(raiz->hoja);
            auto hijoDer = make_shared<NodoBStar>(raiz->hoja);

            if (!raiz->hoja && raiz->hijos.size() < static_cast<size_t>(2 * T)) {
                cerr << "Error: raíz no tiene suficientes hijos para dividir." << endl;
                return;
            }

            hijoIzq->claves.assign(raiz->claves.begin(), raiz->claves.begin() + T - 1);
            hijoDer->claves.assign(raiz->claves.begin() + T, raiz->claves.end());

            nuevaRaiz->claves.push_back(raiz->claves[T - 1]);
            nuevaRaiz->hijos.emplace_back(hijoIzq);
            nuevaRaiz->hijos.emplace_back(hijoDer);

            if (!raiz->hoja) {
                hijoIzq->hijos = vector<shared_ptr<NodoBStar>>(raiz->hijos.begin(), raiz->hijos.begin() + T);
                hijoDer->hijos = vector<shared_ptr<NodoBStar>>(raiz->hijos.begin() + T, raiz->hijos.end());
            }
            raiz = nuevaRaiz;
        }
        insertarNoLleno(raiz, clave);
    }

    bool buscar(int clave) {
        return buscar(raiz, clave) != nullptr;
    }

    void imprimirAscendente() {
        imprimirAsc(raiz);
        cout << endl;
    }

    void imprimirDescendente() {
        imprimirDesc(raiz);
        cout << endl;
    }

    bool esArbolBStar() {
        int nivelHoja = -1;
        return esValido(raiz, 0, nivelHoja);
    }
};

int main() {
    try {
        ArbolBStar arbol;
        for (int val : {10, 20, 5, 6, 12, 30, 7, 17, 3, 8, 15, 22, 35, 2, 1})
            arbol.insertar(val);

        cout << "Ascendente: ";
        arbol.imprimirAscendente();
        cout << "Descendente: ";
        arbol.imprimirDescendente();

        cout << "Buscar 15: " << (arbol.buscar(15) ? "Sí" : "No") << endl;
        cout << "Buscar 99: " << (arbol.buscar(99) ? "Sí" : "No") << endl;

        if (!arbol.esArbolBStar()) {
            cerr << "Error: la estructura del árbol B* no es válida tras las inserciones." << endl;
        } else {
            cout << "¿Es B* válido?: Sí" << endl;

            cout << "-- Borrando valores: 6, 15, 3 --" << endl;
            arbol.borrar(6);
            arbol.borrar(15);
            arbol.borrar(3);

            cout << "Ascendente tras borrado: ";
            arbol.imprimirAscendente();
            cout << "Descendente tras borrado: ";
            arbol.imprimirDescendente();

            if (!arbol.esArbolBStar()) {
                cerr << "Error: la estructura del árbol B* no es válida tras el borrado." << endl;
            } else {
                cout << "¿Sigue siendo B* válido?: Sí" << endl;
            }
        }
        
        cout << "-- Borrado masivo: 1 al 35 --" << endl;
        for (int val : {1,2,3,5,6,7,8,10,12,15,17,20,22,30,35})
            arbol.borrar(val);

        cout << "Ascendente tras borrado masivo: ";
        arbol.imprimirAscendente();
        cout << "Descendente tras borrado masivo: ";
        arbol.imprimirDescendente();

        if (!arbol.esArbolBStar()) {
            cerr << "Error: el árbol no es válido tras el borrado masivo." << endl;
        } else {
            cout << "¿Sigue siendo B* válido tras borrado masivo?: Sí" << endl;
        }
    } catch (const exception& e) {
        cerr << "Excepción atrapada: " << e.what() << endl;
    } catch (...) {
        cerr << "Error inesperado (posible acceso fuera de límites o puntero nulo)." << endl;
    }

    return 0;
}
