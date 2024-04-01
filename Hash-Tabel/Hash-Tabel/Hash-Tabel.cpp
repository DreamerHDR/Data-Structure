#include <iostream>
#include <string>

// Класс, представляющий элемент справочника
class DirectoryEntry {
public:
    std::string name;
    std::string phoneNumber;

    DirectoryEntry(const std::string& name, const std::string& phoneNumber): name(name), phoneNumber(phoneNumber) {}
};

// Хеш-таблица для справочника
class Directory {
private:
    static const int TABLE_SIZE = 100; // Размер таблицы

    DirectoryEntry* table[TABLE_SIZE];

public:
    Directory() {
        // Инициализация таблицы
        for (int i = 0; i < TABLE_SIZE; ++i) {
            table[i] = nullptr;
        }
    }

    // Хеш-функция
    int hash(const std::string& key) 
    {
        int sum = 0;
        for (char c : key) {
            sum += static_cast<int>(c);
        }
        return sum % TABLE_SIZE;
    }

    // Метод свертки (квадратичное апробирование)
    int probe(int hashValue, int i) 
    {
        return (hashValue + i * i) % TABLE_SIZE;
    }

    // Добавление элемента в справочник
    void addEntry(const std::string& name, const std::string& phoneNumber) 
    {
        DirectoryEntry* entry = new DirectoryEntry(name, phoneNumber);
        int hashValue = hash(phoneNumber);

        // Поиск свободной ячейки для вставки
        int i = 0;
        while (table[hashValue] != nullptr) {
            hashValue = probe(hashValue, ++i);
        }

        table[hashValue] = entry;
    }

    // Поиск элемента по номеру телефона
    DirectoryEntry* findEntry(const std::string& phoneNumber) 
    {
        int hashValue = hash(phoneNumber);

        // Поиск элемента или пустой ячейки
        int i = 0;
        while (table[hashValue] != nullptr) {
            if (table[hashValue]->phoneNumber == phoneNumber) {
                return table[hashValue];
            }
            hashValue = probe(hashValue, ++i);
        }

        return nullptr; // Элемент не найден
    }

    // Удаление элемента из справочника
    void removeEntry(const std::string& phoneNumber) 
    {
        int hashValue = hash(phoneNumber);

        // Поиск элемента
        int i = 0;
        while (table[hashValue] != nullptr) {
            if (table[hashValue]->phoneNumber == phoneNumber) {
                delete table[hashValue];
                table[hashValue] = nullptr;
                return;
            }
            hashValue = probe(hashValue, ++i);
        }
    }
};

int main() 
{
    system("chcp 1251");
    Directory directory;

    // Добавление элементов в справочник
    directory.addEntry("Петрво П.В", "8800");
    directory.addEntry("Петров П.В", "8950");
    directory.addEntry("Сидоров Т.Т", "8920");

    // Поиск элементов в справочнике
    std::string phoneNumber;
    std::cout << "Введите номер телефона для поиска: ";
    std::cin >> phoneNumber;

    DirectoryEntry* entry = directory.findEntry(phoneNumber);
    if (entry != nullptr) 
    {
        std::cout << "Имя: " << entry->name << std::endl;
        std::cout << "Телефон: " << entry->phoneNumber << std::endl;
    }
    else 
    {
        std::cout << "Элемент не найден" << std::endl;
    }

    return 0;
}