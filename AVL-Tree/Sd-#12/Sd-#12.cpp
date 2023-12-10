#include <iostream>
#include <cstdlib>
#include <ctime>

// Структура узла AVL-дерева
struct Node {
    int key;
    int height;
    Node* left;
    Node* right;
};

// Функция для создания нового узла
Node* newNode(int key) {
    Node* node = new Node();
    node->key = key;
    node->height = 1;
    node->left = nullptr;
    node->right = nullptr;
    return node;
}

// Функция для получения высоты узла
int getHeight(Node* node) {
    if (node == nullptr)
        return 0;
    return node->height;
}

// Функция для получения баланса узла
int getBalance(Node* node) {
    if (node == nullptr)
        return 0;
    return getHeight(node->left) - getHeight(node->right);
}

// Функция для обновления высоты узла
void updateHeight(Node* node) {
    int leftHeight = getHeight(node->left);
    int rightHeight = getHeight(node->right);
    node->height = (leftHeight > rightHeight ? leftHeight : rightHeight) + 1;
}

// Функция для поворота вправо(y - узел, вокруг которого выполняется поворот вправо, x - левый потомок узла y, T2 - правое поддерево узла x.)
//     y          x
//    / \        / \
//   x   C      A   y
//  / \            / \
// A   B          B   C
Node* rotateRight(Node* y) {
    Node* x = y->left;
    Node* T2 = x->right;
    x->right = y;
    y->left = T2;
    updateHeight(y);
    updateHeight(x);
    return x;
}

// Функция для поворота влево
Node* rotateLeft(Node* x) {
    Node* y = x->right;
    Node* T2 = y->left;
    y->left = x;
    x->right = T2;
    updateHeight(x);
    updateHeight(y);
    return y;
}

// Функция для вставки ключа в AVL-дерево
Node* insert(Node* root, int key) {
    if (root == nullptr)
        return newNode(key);
    if (key < root->key)
        root->left = insert(root->left, key);
    else if (key > root->key)
        root->right = insert(root->right, key);
    else
        return root; // Дублирование ключей не разрешено

    updateHeight(root);

    int balance = getBalance(root);

    // Проверка нарушения баланса и выполнение соответствующих поворотов
    if (balance > 1 && key < root->left->key)
        return rotateRight(root);

    if (balance < -1 && key > root->right->key)
        return rotateLeft(root);

    if (balance > 1 && key > root->left->key) {
        root->left = rotateLeft(root->left);
        return rotateRight(root);
    }

    if (balance < -1 && key < root->right->key) {
        root->right = rotateRight(root->right);
        return rotateLeft(root);
    }

    return root;
}

// Функция для вывода AVL-дерева в порядке возрастания ключей
void inorderTraversal(Node* root) {
    if (root != nullptr) {
        inorderTraversal(root->left);
        std::cout << root->key << " ";
        inorderTraversal(root->right);
    }
}

// Функция для проверки сбалансированности AVL-дерева
bool isBalanced(Node* root) {
    if (root == nullptr)
        return true;

    int balance = getBalance(root);
    if (balance < -1 || balance > 1)
        return false;

    return isBalanced(root->left) && isBalanced(root->right);
}

int main() {
    system("chcp 1251");
    std::srand(std::time(nullptr));

    int n;
    std::cout << "Введите количество случайных ключей: ";
    std::cin >> n;

    Node* root = nullptr;

    std::cout << "Случайные ключи: ";
    for (int i = 1; i < n; i++) {
        int key = i/*rand() % 100*/; // генерация случайного ключа от 0 до 99
        std::cout << key << " ";
        root = insert(root, key);
    }

    std::cout << std::endl;

    std::cout << "Порядок возрастания ключей в AVL-дереве: ";
    inorderTraversal(root);
    std::cout << std::endl;

    int newKey;
    std::cout << "Введите произвольный ключ для добавления: ";
    std::cin >> newKey;
    root = insert(root, newKey);

    std::cout << "Порядок возрастания ключей после добавления: ";
    inorderTraversal(root);
    std::cout << std::endl;

    if (isBalanced(root))
        std::cout << "Дерево сбалансировано." << std::endl;

    return 0;
}
