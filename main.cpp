#include <iostream>       // Для ввода-вывода
#include <unordered_map>  // Для хранения частот и кодов символов
#include <queue>          // Для приоритетной очереди (min-heap)
#include <string>         // Для работы со строками

using std::cout;
using std::cin;
using std::endl;
using std::string;
using std::unordered_map;
using std::priority_queue;

// Структура узла дерева Хаффмана
struct Node {
    char ch;       // Символ, хранящийся в узле (для внутренних узлов '\0')
    int freq;      // Частота появления символа или сумма частот дочерних узлов
    Node* left;    // Указатель на левое поддерево (кодируемый 0)
    Node* right;   // Указатель на правое поддерево (кодируемый 1)

    // Конструктор инициализирует узел заданным символом и частотой
    // Изначально дети равны nullptr (нет детей)
    Node(char c, int f) : ch(c), freq(f), left(nullptr), right(nullptr) {}
};

// Компаратор для приоритетной очереди: узлы с меньшей частотой должны иметь больший приоритет
struct Compare {
    bool operator()(Node* a, Node* b) {
        // Если freq a больше freq b, то a имеет меньший приоритет,
        // т.к. в min-heap должны быть сначала меньшие freq
        return a->freq > b->freq;
    }
};

// Рекурсивная функция для построения кодов Хаффмана для каждого символа
// node — текущий узел дерева, code — текущий путь к узлу (строка из '0' и '1')
// codes — карта, где ключ — символ, а значение — его код Хаффмана
void buildCodes(Node* node, const string& code, unordered_map<char, string>& codes) {
    if (!node) return;  // Если узел пустой — выход

    // Если узел листовой (нет детей), значит здесь символ, код — путь до него
    if (!node->left && !node->right) {
        codes[node->ch] = code;  // Сохраняем код символа
        return;
    }

    // Идем в левое поддерево — добавляем '0' к пути
    buildCodes(node->left, code + "0", codes);

    // Идем в правое поддерево — добавляем '1' к пути
    buildCodes(node->right, code + "1", codes);
}

// Функция кодирования текста с использованием таблицы кодов
// text — исходная строка, codes — карта символов и их кодов
string encode(const string& text, const unordered_map<char, string>& codes) {
    string result;  // Итоговая бинарная строка

    // Для каждого символа из текста добавляем его код в результат
    for (char ch : text) {
        result += codes.at(ch);  // .at() используется, чтобы получить код по символу
    }
    return result;  // Возвращаем полную бинарную строку
}

// Функция декодирования бинарной строки с помощью дерева Хаффмана
// encoded — бинарная строка, root — корень дерева Хаффмана
string decode(const string& encoded, Node* root) {
    string result;      // Итоговая расшифрованная строка
    Node* current = root;  // Начинаем с корня дерева

    // Для каждого бита в закодированной строке
    for (char bit : encoded) {
        if (bit == '0')
            current = current->left;  // Идем в левое поддерево
        else
            current = current->right; // Идем в правое поддерево

        // Если достигли листа — нашли символ
        if (!current->left && !current->right) {
            result += current->ch;  // Добавляем символ к результату
            current = root;         // Возвращаемся к корню для следующего символа
        }
    }

    return result;  // Возвращаем расшифрованный текст
}

// Рекурсивная функция освобождения памяти дерева Хаффмана
void freeTree(Node* node) {
    if (!node) return;          // Если узел пустой, выходим
    freeTree(node->left);       // Освобождаем левое поддерево
    freeTree(node->right);      // Освобождаем правое поддерево
    delete node;                // Удаляем текущий узел
}

int main() {
    cout << "Введите текст: ";  // Просим пользователя ввести строку
    string text;
    getline(cin, text);         // Считываем строку с пробелами

    // Создаем карту для подсчета частоты каждого символа в тексте
    unordered_map<char, int> freq;
    for (char ch : text) {
        freq[ch]++;  // Увеличиваем счётчик для символа
    }

    // Создаем приоритетную очередь для построения дерева
    // Узлы с наименьшей частотой будут на вершине
    priority_queue<Node*, std::vector<Node*>, Compare> pq;

    // Добавляем все символы как отдельные узлы в очередь
    for (auto& pair : freq) {
        pq.push(new Node(pair.first, pair.second));
    }

    // Строим дерево Хаффмана пока в очереди не останется один узел (корень)
    while (pq.size() > 1) {
        Node* left = pq.top(); pq.pop();   // Узел с наименьшей частотой
        Node* right = pq.top(); pq.pop();  // Второй узел с наименьшей частотой

        // Создаем новый узел — объединение двух, частота сумма их частот
        Node* merged = new Node('\0', left->freq + right->freq);
        merged->left = left;    // Левый ребёнок
        merged->right = right;  // Правый ребёнок

        pq.push(merged);        // Добавляем объединённый узел обратно в очередь
    }

    Node* root = pq.top();     // Корень построенного дерева Хаффмана

    // Создаем карту для хранения кодов каждого символа
    unordered_map<char, string> codes;
    buildCodes(root, "", codes);  // Рекурсивно строим таблицу кодов

    // Выводим коды символов
    cout << "\nКоды Хаффмана:\n";
    for (auto& pair : codes) {
        cout << pair.first << ": " << pair.second << "\n";
    }

    // Кодируем исходный текст
    string encoded = encode(text, codes);
    cout << "\nЗакодировано: " << encoded << "\n";

    // Декодируем обратно
    string decoded = decode(encoded, root);
    cout << "Декодировано: " << decoded << "\n";

    // Освобождаем память, удаляя все узлы дерева
    freeTree(root);

    return 0;  // Завершаем программу успешно
}
