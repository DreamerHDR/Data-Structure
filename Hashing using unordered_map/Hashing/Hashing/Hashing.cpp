#include <iostream>
#include <unordered_map>
#include <queue>
#include <string>

using namespace std;

struct Passenger {
    string name;
    string flight;
};

class FlightBookingSystem {
private:
    unordered_map<string, unordered_map<string, Passenger>> flightPassengers;
    unordered_map<string, queue<Passenger>> waitingPassengers;
    unordered_map<string, int> maxSeats;

public:
    void addFlight(string flight, int maxSeats) {
        flightPassengers[flight] = unordered_map<string, Passenger>();
        waitingPassengers[flight] = queue<Passenger>();
        this->maxSeats[flight] = maxSeats;
    }

    void registerPassenger(string name) {
        // Регистрация пассажира в базе данных
        cout << "Пассажир " << name << " зарегистрирован." << endl;
    }

    void bookTicket(string name, string flight) {
        if (flightPassengers.find(flight) == flightPassengers.end()) {
            cout << "Рейс " << flight << " не существует." << endl;
            return;
        }

        if (flightPassengers[flight].size() < maxSeats[flight]) {
            flightPassengers[flight][name] = { name, flight };
            cout << "Билет забронирован на " << name << " в рейс " << flight << endl;
        }
        else {
            waitingPassengers[flight].push({ name, flight });
            cout << "Нет свободных мест на рейс " << flight << ". Добавлен(а) " << name << " в лист ожидания." << endl;
        }
    }

    void printPassengerInfo(string name) {
        bool found = false;

        // Поиск пассажира в забронированных билетах
        for (const auto& entry : flightPassengers) {
            const unordered_map<string, Passenger>& passengers = entry.second;
            if (passengers.find(name) != passengers.end()) {
                cout << "Пассажир " << name << " забронирован на рейс " << passengers.at(name).flight << endl;
                found = true;
                break;
            }
        }

        // Поиск пассажира в очереди
        if (!found) {
            for (const auto& entry : waitingPassengers) {
                const queue<Passenger>& waitingQueue = entry.second;
                queue<Passenger> temp = waitingQueue;
                int position = 1;
                while (!temp.empty()) {
                    if (temp.front().name == name) {
                        cout << "Пассажир " << name << " находится в позиции " << position << " в листе ожидания на рейс " << temp.front().flight << endl;
                        found = true;
                        break;
                    }
                    temp.pop();
                    position++;
                }
                if (found) {
                    break;
                }
            }
        }

        if (!found) {
            cout << "Пассажир " << name << " не найден." << endl;
        }
    }

    void printAllFlights() {
        cout << "Список доступных рейсов:" << endl;
        for (const auto& entry : flightPassengers) {
            cout << entry.first << endl;
        }
    }

    void printPassengersByFlight(string flight) {
        if (flightPassengers.find(flight) == flightPassengers.end()) {
            cout << "Рейс " << flight << " не существует." << endl;
            return;
        }

        const unordered_map<string, Passenger>& passengers = flightPassengers[flight];
        cout << "Пассажиры, забронировавшие билеты на рейс " << flight << ":" << endl;
        for (const auto& entry : passengers) {
            cout << entry.second.name << endl;
        }

        const queue<Passenger>& waitingQueue = waitingPassengers[flight];
        queue<Passenger> temp = waitingQueue;
        int position = 1;
        cout << "Пассажиры в списке ожидания на рейс " << flight << ":" << endl;
        while (!temp.empty()) {
            cout << position << ". " << temp.front().name << endl;
            temp.pop();
            position++;
        }
    }
};

int main() {
    system("chcp 1251 & cls");
    FlightBookingSystem bookingSystem;

    // Добавление рейсов с указанием максимального количества мест
    bookingSystem.addFlight("Москва-Владивосток", 4);
    bookingSystem.addFlight("Лондон-Париж", 3);

    // Регистрация пассажиров
    bookingSystem.registerPassenger("Иван Иванов");
    bookingSystem.registerPassenger("Петр Петров");
    bookingSystem.registerPassenger("Алексей Алексеев");
    bookingSystem.registerPassenger("Мария Сидорова");
    bookingSystem.registerPassenger("Екатерина Иванова");
    bookingSystem.registerPassenger("Виолетта Соколова");
    cout << endl;

    // Бронирования билетов
    bookingSystem.bookTicket("Иван Иванов", "Лондон-Париж");
    bookingSystem.bookTicket("Петр Петров", "Лондон-Париж");
    bookingSystem.bookTicket("Алексей Алексеев", "Лондон-Париж");
    bookingSystem.bookTicket("Мария Сидорова", "Лондон-Париж");    
    bookingSystem.bookTicket("Екатерина Иванова", "Лондон-Париж");
    bookingSystem.bookTicket("Виолетта Соколова", "Москва-Владивосток");
    cout << endl;

    // Вывод информации о пассажирах
    bookingSystem.printPassengerInfo("Иван Иванов");
    bookingSystem.printPassengerInfo("Петр Петров");
    bookingSystem.printPassengerInfo("Виолетта Соколова");
    cout << endl;

    // Вывод информации о рейсах и пассажирах
    cout << "Информация о пассажирах по рейсам:" << endl;
    bookingSystem.printAllFlights();
    cout << endl;

    string selectedFlight;
    cout << "Выберите рейс для просмотра пассажиров (название): ";
    getline(cin, selectedFlight);

    bookingSystem.printPassengersByFlight(selectedFlight);

    return 0;
}
