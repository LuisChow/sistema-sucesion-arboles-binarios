#include <iostream>
#include <fstream>
#include <sstream>
#include <string>

using namespace std;

// Estructura del Nodo
struct RoyalNode {
    // Datos de la persona
    int id;
    string name;
    string last_name;
    char gender;      // 'H' o 'M'
    int age;
    int id_father;
    bool is_dead;
    bool was_king;
    bool is_king;

    // Punteros del Arbol Binario
    // left: Primogenito (Hijo mayor)
    // right: Segundo hijo (Hermano del primogenito)
    // parent: Puntero hacia arriba para referencias
    RoyalNode* left;
    RoyalNode* right;
    RoyalNode* parent;

    // Constructor para inicializar punteros en null
    RoyalNode() : left(nullptr), right(nullptr), parent(nullptr) {}
};

class RoyalTree {
private:
    RoyalNode* root;

    // Metodo auxiliar para buscar un nodo por ID de manera recursiva
    // Reemplaza el uso de std::map para cumplir con "estructuras a mano"
    RoyalNode* findNodeById(RoyalNode* current, int id) {
        if (current == nullptr) return nullptr;
        if (current->id == id) return current;
        
        // Busca en la rama izquierda (primogenitos)
        RoyalNode* foundLeft = findNodeById(current->left, id);
        if (foundLeft) return foundLeft;
        
        // Busca en la rama derecha (hermanos)
        return findNodeById(current->right, id);
    }

    // Recorrido Pre-Orden para mostrar la jerarquia visualmente
    void printRecursive(RoyalNode* node) {
        if (node == nullptr) return;

        // Solo mostrar si esta vivo
        if (!node->is_dead) {
            string role = (node->is_king) ? " [REY/REINA ACTUAL]" : "";
            cout << "- " << node->name << " " << node->last_name 
                 << " (" << node->age << " a, " << node->gender << ")" << role << endl;
        }
        
        //Primero hijos, luego hermanos
        printRecursive(node->left);
        printRecursive(node->right);
    }

    // Busca candidatos recorriendo el arbol en profundidad:
    // 1. Hijo (Left) -> 2. Hermano (Right) -> 3. Tio (Right del padre, por recursion)
    bool findSuccessorRecursive(RoyalNode* node, RoyalNode*& bestMale, RoyalNode*& bestFemale) {
        if (node == nullptr) return false;

        // Si la persona esta viva, evaluamos si es candidata
        if (!node->is_dead) {
            // Prioridad 1: Varon menor a 70 años
            if (node->gender == 'H' && node->age < 70) {
                bestMale = node; 
                return true; // Encontrado el mejor candidato, detener busqueda
            }
            
            // Prioridad 2: Mujer mayor a 15 años (backup por si no hay hombres)
            if (node->gender == 'M' && node->age > 15) {
                // Guardamos la mas joven encontrada hasta ahora
                if (bestFemale == nullptr || node->age < bestFemale->age) {
                    bestFemale = node;
                }
            }
        }

        // Regla: Buscar primero en descendencia (Hijos/Left)
        if (findSuccessorRecursive(node->left, bestMale, bestFemale)) return true;
        
        // Regla: Si no hay descendencia valida, buscar en colaterales (Hermanos/Right)
        if (findSuccessorRecursive(node->right, bestMale, bestFemale)) return true;

        return false;
    }

    // Auxiliar para encontrar al rey actual y verificar su estado
    RoyalNode* findCurrentKingRecursive(RoyalNode* node) {
        if (!node) return nullptr;
        if (node->is_king) return node;
        
        RoyalNode* leftRes = findCurrentKingRecursive(node->left);
        if (leftRes) return leftRes;
        
        return findCurrentKingRecursive(node->right);
    }

public:
    RoyalTree() : root(nullptr) {}
    
    // Carga de datos desde CSV y construccion del arbol
    void loadFromCsv(string filepath) {
        ifstream file(filepath);
        if (!file.is_open()) {
            cerr << "Error: No se pudo abrir " << filepath << endl;
            return;
        }

        string line;
        bool isHeader = true;
        while (getline(file, line)) {
            // Elimina el retorno de carro de archivos CSV con formato Windows (CRLF),
            // evitando que la ultima columna quede con un caracter \r adicional.
            if (!line.empty() && line.back() == '\r') line.pop_back();
            if (isHeader) { isHeader = false; continue; } // Saltar cabecera
            if (line.empty()) continue;

            stringstream ss(line);
            string segment;
            RoyalNode* newNode = new RoyalNode();

            // Parseo manual de cada columna separada por comas
            try {
                getline(ss, segment, ','); newNode->id = stoi(segment);
                getline(ss, segment, ','); newNode->name = segment;
                getline(ss, segment, ','); newNode->last_name = segment;
                getline(ss, segment, ','); newNode->gender = segment[0];
                getline(ss, segment, ','); newNode->age = stoi(segment);
                getline(ss, segment, ','); 
                // Manejo de id_father vacio o 0
                newNode->id_father = (segment == "" || segment == "0") ? 0 : stoi(segment);
                getline(ss, segment, ','); newNode->is_dead = (segment == "1");
                getline(ss, segment, ','); newNode->was_king = (segment == "1");
                getline(ss, segment, ','); newNode->is_king = (segment == "1");
            } catch (...) { 
                delete newNode; 
                continue; // Saltar linea si hay error de formato
            }

            // Insercion en el arbol
            if (newNode->id_father == 0) {
                root = newNode; // Es el primer rey
            } else {
                // Buscamos al padre para enlazar
                RoyalNode* parent = findNodeById(root, newNode->id_father);
                if (parent) {
                    newNode->parent = parent;
                    // Asignar como hijo izquierdo (1ro) o derecho (2do)
                    if (parent->left == nullptr) parent->left = newNode;
                    else if (parent->right == nullptr) parent->right = newNode;
                    else { delete newNode; continue; } // Ignorar 3er hijo si hubiera
                } else { 
                    delete newNode; // Huerfano (padre no encontrado aun)
                }
            }
        }
        cout << "Datos cargados exitosamente." << endl;
        file.close();
    }

    void printSuccessionLine() {
        cout << "\n--- Linea de Sucesion (Vivos) ---" << endl;
        printRecursive(root);
    }

    //Funcion principal que decide si cambiar al rey
    void assignNewKing() {
        RoyalNode* current = findCurrentKingRecursive(root);
        bool needNew = false;
        
        //Verificar condiciones de retiro (Muerte o Edad > 70)
        if (!current) { 
            needNew = true; 
            cout << "Sin rey actual." << endl; 
        } else if (current->is_dead || current->age > 70) {
            cout << "Reinado de " << current->name << " terminado." << endl;
            current->is_king = false; 
            current->was_king = true; 
            needNew = true;
        } else { 
            cout << "El rey sigue reinando." << endl; 
        }

        if (!needNew) return;

        //Buscar nuevo rey 
        RoyalNode* male = nullptr; 
        RoyalNode* female = nullptr;
        
        findSuccessorRecursive(root, male, female);
        
        //Elegir varon si existe, sino la mejor candidata mujer
        RoyalNode* newKing = (male) ? male : female;
        
        if (newKing) {
            newKing->is_king = true;
            cout << ">>> NUEVO REY/REINA: " << newKing->name << " <<<" << endl;
        } else { 
            cout << "CRISIS: Sin herederos." << endl; 
        }
    }

    //Edicion de datos
    void editNode() {
        int id;
        cout << "Ingrese ID del nodo a editar: ";
        cin >> id;
        
        RoyalNode* node = findNodeById(root, id);
        if (!node) {
            cout << "ID no encontrado." << endl;
            return;
        }

        cout << "Editando a: " << node->name << " (Deje '-' para no cambiar)" << endl;
        
        string input;
        cout << "Nuevo Nombre [" << node->name << "]: ";
        cin >> input; 
        if (input != "-") node->name = input;

        cout << "Nueva Edad [" << node->age << "]: ";
        cin >> input;
        if (input != "-") node->age = stoi(input);

        cout << "Esta muerto? (1=Si, 0=No) [" << node->is_dead << "]: ";
        cin >> input;
        if (input != "-") node->is_dead = (input == "1");
        
        cout << "Datos actualizados." << endl;
    }
};

int main() {
    RoyalTree tree;
    
    //Ruta del CSV
    string path = "bin/royal_data.csv";
    ifstream check(path);
    if (!check.good()) {
        path = "royal_data.csv";
    }
    check.close();

    tree.loadFromCsv(path);

    bool running = true;
    while (running) {
        cout << "\n1. Ver Linea\n2. Asignar Rey\n3. Editar\n4. Salir\nOp: ";
        int opt; 
        //Validacion simple de entrada numerica
        if(!(cin >> opt)) { 
            cin.clear(); 
            cin.ignore(10000,'\n'); 
            continue; 
        }

        switch (opt) {
            case 1: tree.printSuccessionLine(); break;
            case 2: tree.assignNewKing(); break;
            case 3: tree.editNode(); break;
            case 4: running = false; break;
        }
    }
    return 0;
}
