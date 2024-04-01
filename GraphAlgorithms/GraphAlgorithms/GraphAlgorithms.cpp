#include <iostream>
#include <vector>
#include <unordered_map>
#include <cmath>
#include <limits>

using namespace std;

// Структура для представления точки на плоскости
struct Point {
    double x, y;

    Point(double x, double y) : x(x), y(y) {}
};

// Структура для представления связи между двумя элементами
struct Edge {
    int from, to;

    Edge(int from, int to) : from(from), to(to) {}
};

// Класс для представления графа
class Graph {
private:
    unordered_map<int, vector<int>> adjacencyList;
    vector<Point> points;

public:
    Graph(const vector<Point>& points, const vector<Edge>& edges) : points(points) {
        // Создаем список смежности на основе ребер
        for (const auto& edge : edges) {
            adjacencyList[edge.from].push_back(edge.to);
            adjacencyList[edge.to].push_back(edge.from);
        }
    }

    // Функция для вычисления расстояния между двумя точками
    double distance(const Point& p1, const Point& p2) {
        return sqrt(pow(p1.x - p2.x, 2) + pow(p1.y - p2.y, 2));
    }

    // Рекурсивная функция для обхода графа и поиска контура
    void dfs(int v, vector<bool>& visited) {
        visited[v] = true;
        cout << v << " ";

        int closestNeighbor = -1;
        double minDistance = numeric_limits<double>::max();

        // Ищем ближайшего соседа текущего элемента
        for (int neighbor : adjacencyList[v]) {
            if (!visited[neighbor]) {
                double dist = distance(points[v], points[neighbor]);
                if (dist < minDistance) {
                    minDistance = dist;
                    closestNeighbor = neighbor;
                }
            }
        }

        // Если нашли ближайшего соседа, продолжаем обход с ним
        if (closestNeighbor != -1) {
            dfs(closestNeighbor, visited);
        }
    }

    // Функция для поиска контура с наибольшей суммарной длиной связей
    void findLargestContour() {
        vector<bool> visited(points.size(), false);

        // Находим элемент с наибольшей суммарной длиной связей
        int startNode = -1;
        double maxTotalLength = 0.0;

        for (int i = 0; i < points.size(); ++i) {
            double totalLength = 0.0;
            for (int neighbor : adjacencyList[i]) {
                totalLength += distance(points[i], points[neighbor]);
            }

            if (totalLength > maxTotalLength) {
                maxTotalLength = totalLength;
                startNode = i;
            }
        }

        // Начинаем обход с элемента с наибольшей суммарной длиной связей
        if (startNode != -1) {
            cout << "Contour: ";
            dfs(startNode, visited);
            cout << endl;
        }
        else {
            cout << "No contour found." << endl;
        }
    }

    // Функция для вывода списка смежности графа
    void printGraph() {
        cout << "Graph:" << endl;
        for (const auto& entry : adjacencyList) {
            cout << entry.first << ": ";
            for (int neighbor : entry.second) {
                cout << neighbor << " ";
            }
            cout << endl;
        }
    }
};

int main() {
    // Пример координат и связей между элементами
    vector<Point> points = {
        {0, 0}, {1, 0}, {2, 0}, {0, 1}, {1, 1}, {2, 1}
    };

    vector<Edge> edges = {
        {0, 1}, {0, 3}, {1, 2}, {1, 4}, {2, 5}, {3, 4}, {4, 5}
    };

    // Создаем граф
    Graph graph(points, edges);

    // Выводим граф
    graph.printGraph();

    // Находим контур с наибольшей суммарной длиной связей
    graph.findLargestContour();

    return 0;
}
