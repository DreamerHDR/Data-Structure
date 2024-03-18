#include <iostream>
#include <vector>
#include <iomanip>
#include <algorithm>
#include <queue>


struct Point
{
	double x, y;
};

struct Rect
{
	double x1, y1, x2, y2;
};

class RTree
{
public:
	RTree(int min = 2, int max = 4, int child = 3): min(min), max(max), child(child), root(new Node{ { 0, 0, 0, 0 }, true, nullptr }) {}

	struct Node
	{
		Rect rect;
		bool isLeaf;
		Node* parent;

		// листовой узел
		std::vector<Point> points;

		// внутренний узел
		std::vector<Node*> childs;
	};

	void addPoint(Point point)
	{
		//Установка текущего узла на корень дерева
		Node* currentNode = root;

		// Вставка новой точки в листовой узел
		Node* leafNode = findLeafNode(currentNode, point);
		leafNode->points.push_back(point);
		// Обновление прямоугольника на основе новых точек
		leafNode->rect = updateRectFromPoints(leafNode->points);
		// Получаем указатель на родительский узел листового узла
		Node* parent = leafNode->parent;
		//Пока есть родительские узлы
		do {
			if (parent == nullptr)
				break;
			parent->rect = updateRectFromRects(parent->childs);
			parent = parent->parent;
		} while (parent != nullptr);

		// Если кол-во точек превышает максимальное значение, то листовой узел разделяется на два новых
		if (leafNode->points.size() > max)
		{
			splitLeafNode(leafNode);
		}

		// Если кол-во дочерних узлов превышает maxChid, внутренный узел делится на 2 новых
		if (currentNode->childs.size() > child)
		{
			splitInternalNode(currentNode);
		}
	}

	void removePoint(Point point)
	{
		if (!root->isLeaf && root->childs.empty())
			return;

		bool pointRemoved = false;
		removePointSup(root, point, pointRemoved);
	}

	void searchPoint(const Point& point)
	{
		if (isPointInTree(point))
			std::cout << "Точка: ( " << point.x << "; " << point.y << " ) находится в дереве" << std::endl;
		else
			std::cout << "Точка: ( " << point.x << "; " << point.y << " ) не находится в дереве" << std::endl;
	}

	void searchRect(const Rect& rect)
	{
		std::vector<Point> points;

		checkPointsInRectSup(root, rect, points);
		std::cout << "Область: ( " << rect.x1 << "; " << rect.y1 << "; " << rect.x2 << "; " << rect.y2 << " )" << std::endl;

		if (points.empty()) {
			std::cout << "Нет точек внутри области" << std::endl;
		}
		else {
			std::cout << "Точки внутри области: ";
			for (size_t i = 0; i < points.size(); ++i) {
				std::cout << "( " << points[i].x << ", " << points[i].y << " )";
				if (i < points.size() - 1) {
					std::cout << ", ";
				}
			}
			std::cout << std::endl;
		}
	}

	void searchExact(const Point& point)
	{
		if (root->isLeaf == true && root->points.empty())
		{
			std::cout << "Нет точек в дереве" << std::endl;
			return;
		}

		double minDistance = DBL_MAX;
		Point nearestPoint;
		bool found = false;

		checkNearestPointSup(root, point, minDistance, nearestPoint, found);

		std::cout << "Область: ( " << point.x << ", " << point.y << " )" << std::endl;
		if (found)
			std::cout << "Ближайший сосед: ( " << nearestPoint.x << ", " << nearestPoint.y << " )" << std::endl;
		else
			std::cout << "Нет точек в дереве" << std::endl;
	}

	void print()
	{
		if (root->isLeaf == true && root->points.empty())
		{
			std::cout << "Tree is empty" << std::endl;
			return;
		}

		// Выводим корень и начинаем рекурсивно выводить остальные узлы
		printNode(root);
	}

private:
	void removePointSup(Node* node, Point point, bool pointRemoved)
	{
		if (pointRemoved)
			return;

		if (node->isLeaf)
		{
			for (auto i = node->points.begin(); i != node->points.end(); ++i)
			{
				if (i->x == point.x && i->y == point.y)
				{
					node->points.erase(i);
					node->rect = updateRectFromPoints(node->points);


					if (node->points.size() < min)
					{
						mergeLeafNode(node);
					}
					pointRemoved = true;
					return;
				}
			}
		}
		else
		{
			for (auto child : node->childs)
			{
				removePointSup(child, point, pointRemoved);
			}
		}
	}

	Node* findLeafNode(Node* currentNode, const Point& point)
	{
		while (!currentNode->isLeaf)
		{
			double minDistance = DBL_MAX;
			Node* nextNode = nullptr;

			// проходимся по дочерним узлам и находим тот, который ближе к точке
			for (Node* child : currentNode->childs)
			{
				double pointDistance = calcDistance(child->rect, point);
				if (pointDistance < minDistance)
				{
					minDistance = pointDistance;
					nextNode = child;
				}
			}

			if (nextNode == nullptr)
				break;

			currentNode = nextNode;
		}

		currentNode->isLeaf = true;

		return currentNode;
	}

	void splitLeafNode(Node* leafNode)
	{
		Node* newLeafNode = new Node;
		newLeafNode->isLeaf = true;
		newLeafNode->parent = leafNode->parent;

		// определяем, какая размерность больше: ширина или высота прямоугольника узла (для более рационального деления области)
		double dx = leafNode->rect.x2 - leafNode->rect.x1;
		double dy = leafNode->rect.y2 - leafNode->rect.y1;
		if (dx > dy)
		{
			std::sort(leafNode->points.begin(), leafNode->points.end(), [](const Point& a, const Point& b) {
				return a.x < b.x;
				});
		}
		else
		{
			std::sort(leafNode->points.begin(), leafNode->points.end(), [](const Point& a, const Point& b) {
				return a.y < b.y;
				});
		}

		size_t newSize = leafNode->points.size() / 2;

		// переносим половину точек из старого узла в новый
		newLeafNode->points.insert(newLeafNode->points.begin(), leafNode->points.begin() + newSize, leafNode->points.end());
		leafNode->points.erase(leafNode->points.begin() + newSize, leafNode->points.end());


		leafNode->rect = updateRectFromPoints(leafNode->points);
		newLeafNode->rect = updateRectFromPoints(newLeafNode->points);


		// если узел является корневым, создаем новый корневой узел
		if (leafNode == root)
		{
			Node* newRoot = new Node;
			newRoot->isLeaf = false;
			newRoot->parent = nullptr;

			newRoot->childs.push_back(leafNode);
			newRoot->childs.push_back(newLeafNode);

			leafNode->parent = newRoot;
			newLeafNode->parent = newRoot;

			root = newRoot;
			root->rect = updateRectFromRects(root->childs);
		}
		else
		{
			// Если узел не является корневым, добавляем новый листовой узел к родительскому узлу
			Node* parent = leafNode->parent;
			parent->childs.push_back(newLeafNode);

			parent->rect = updateRectFromRects(parent->childs);

			if (parent->childs.size() > child)
			{
				splitInternalNode(parent);
			}
		}
	}

	void splitInternalNode(Node* internalNode)
	{
		Node* newInternalNode = new Node;
		newInternalNode->isLeaf = false;
		newInternalNode->parent = internalNode->parent;

		size_t newSize = internalNode->childs.size() / 2;


		// переносим половину дочерних узлов из старого узла в новый
		newInternalNode->childs.insert(newInternalNode->childs.begin(), internalNode->childs.begin() + newSize, internalNode->childs.end());

		for (Node* child : newInternalNode->childs)
		{
			child->parent = newInternalNode;
		}

		internalNode->childs.erase(internalNode->childs.begin() + newSize, internalNode->childs.end());


		internalNode->rect = updateRectFromRects(internalNode->childs);
		newInternalNode->rect = updateRectFromRects(newInternalNode->childs);

		if (internalNode == root)
		{
			Node* newRoot = new Node;
			newRoot->isLeaf = false;
			newRoot->parent = nullptr;
			newRoot->childs.push_back(internalNode);
			newRoot->childs.push_back(newInternalNode);

			internalNode->parent = newRoot;
			newInternalNode->parent = newRoot;

			newRoot->rect = updateRectFromRects(newRoot->childs);
			root = newRoot;
		}
		else
		{
			internalNode->parent->childs.push_back(newInternalNode);

			if (internalNode->parent->childs.size() > child)
			{
				splitInternalNode(internalNode->parent);
			}
		}
	}

	void mergeLeafNode(Node* leafNode)
	{
		std::vector<Point> tempPoints;
		tempPoints = std::move(leafNode->points);

		Node* parent = leafNode->parent;

		if (parent != nullptr)
		{
			for (size_t i = 0; i < parent->childs.size(); ++i)
				if (parent->childs[i] == leafNode)
				{
					parent->childs.erase(parent->childs.begin() + i);
					break;
				}
		}

		if (leafNode != root)
			delete leafNode;

		if (parent != nullptr)
		{
			if (!parent->childs.empty())
			{
				parent->rect = updateRectFromRects(parent->childs);
			}
			else if (parent->parent != nullptr)
			{
				Node* tempParent = parent;

				parent = parent->parent;
				parent->isLeaf = true;
				parent->rect = { 0, 0, 0, 0 };
			}

		}

		for (const auto& point : tempPoints)
		{
			addPoint(point);
		}
	}

	double pointDistance(const Point& p1, const Point& p2)
	{
		double dx = p1.x - p2.x;
		double dy = p1.y - p2.y;

		return std::sqrt(dx * dx + dy * dy);
	}

	double calcDistance(const Rect& rect, const Point& point)
	{
		if (isIntersecting(rect, point))
		{
			return 0;
		}
		double pointDistance = 0;

		if (point.x < rect.x1) // точка находится слева от прямоугольника
		{
			if (point.y < rect.y1)
			{ // ниже и левее
				double dx = rect.x1 - point.x;
				double dy = rect.y1 - point.y;
				pointDistance = std::sqrt(dx * dx + dy * dy);
			}
			else if (point.y > rect.y2)
			{ // выше и левее
				double dx = rect.x1 - point.x;
				double dy = point.y - rect.y2;
				pointDistance = std::sqrt(dx * dx + dy * dy);
			}
			else
			{ // только слева
				pointDistance = rect.x1 - point.x;
			}
			return pointDistance;
		}
		if (point.x > rect.x2) // точка находится справа от прямоугольника
		{
			if (point.y < rect.y1) // ниже и правее
			{
				double dx = point.x - rect.x2;
				double dy = rect.y1 - point.y;
				pointDistance = std::sqrt(dx * dx + dy * dy);
			}
			else if (point.y > rect.y2) // выше и правее
			{
				double dx = point.x - rect.x2;
				double dy = point.y - rect.y2;
				pointDistance = std::sqrt(dx * dx + dy * dy);
			}
			else // только справа
			{
				pointDistance = point.x - rect.x2;
			}
			return pointDistance;
		}
		// точка находится выше или ниже прямоугольника
		if (point.y < rect.y1) // ниже
		{
			pointDistance = rect.y1 - point.y;
		}
		else if (point.y > rect.y2) // выше
		{
			pointDistance = point.y - rect.y2;
		}
		return pointDistance;
	}

	bool isIntersecting(const Rect& rect, const Point& point)
	{
		return (point.x >= rect.x1 && point.x <= rect.x2 && point.y >= rect.y1 && point.y <= rect.y2);
	}

	bool isPointInTree(Point point)
	{
		return isPointInNode(root, point);
	}

	bool isPointInNode(Node* node, Point point)
	{
		if (node == nullptr)
			return false;

		if (node->isLeaf)
		{
			for (const auto& pnt : node->points)
				if (pnt.x == point.x && pnt.y == point.y)
					return true;

			return false;
		}
		else
		{
			for (Node* child : node->childs)
				if (isPointInNode(child, point))
					return true;
		}
		return false;
	}

	void checkPointsInRectSup(Node* node, const Rect& rect, std::vector<Point>& points)
	{
		if (node == nullptr)
			return;

		if (node->isLeaf)
		{
			for (const auto& p : node->points)
				if (isIntersecting(rect, p))
					points.push_back(p);
		}
		else
		{
			for (Node* child : node->childs)
				checkPointsInRectSup(child, rect, points);
		}
	}

	void checkNearestPointSup(Node* node, const Point& targetPoint, double& minDistance, Point& nearestPoint, bool& found)
	{
		if (node == nullptr)
			return;

		if (node->isLeaf)
		{
			for (const auto& point : node->points)
			{
				double distance = pointDistance(point, targetPoint);
				if (distance < minDistance)
				{
					minDistance = distance;
					nearestPoint = point;
					found = true;
				}
			}
		}
		else
		{
			for (Node* child : node->childs)
			{
				Rect childRect = child->rect;
				double closestDistToChild = calcDistance(childRect, targetPoint);
				if (closestDistToChild <= minDistance)
				{
					checkNearestPointSup(child, targetPoint, minDistance, nearestPoint, found);
				}
			}
		}
	}

	Rect updateRectFromPoints(const std::vector<Point>& points)
	{
		Rect rect;

		if (points.empty()) // если точек нет, область будет пустой
		{
			rect = { 0, 0, 0, 0 };
		}
		else
		{

			double minX = points[0].x;
			double minY = points[0].y;
			double maxX = minX;
			double maxY = minY;

			for (const auto& pnt : points)
			{
				minX = std::min(minX, pnt.x);
				minY = std::min(minY, pnt.y);
				maxX = std::max(maxX, pnt.x);
				maxY = std::max(maxY, pnt.y);
			}

			rect.x1 = minX;
			rect.y1 = minY;
			rect.x2 = maxX;
			rect.y2 = maxY;
		}

		return rect;
	}

	Rect updateRectFromRects(const std::vector<Node*>& nodes)
	{
		Rect rect;

		if (nodes.empty()) // если список узлов пуст, возвращаем пустой прямоугольник
		{
			rect = { 0, 0, 0, 0 };
		}
		else
		{
			double minX = nodes[0]->rect.x1;
			double minY = nodes[0]->rect.y1;
			double maxX = nodes[0]->rect.x2;
			double maxY = nodes[0]->rect.y2;

			for (const auto& node : nodes)
			{
				minX = std::min(minX, node->rect.x1);
				minY = std::min(minY, node->rect.y1);
				maxX = std::max(maxX, node->rect.x2);
				maxY = std::max(maxY, node->rect.y2);
			}

			rect.x1 = minX;
			rect.y1 = minY;
			rect.x2 = maxX;
			rect.y2 = maxY;
		}

		return rect;
	}

	void printNode(Node* node, int depth = 0)
	{
		if (node == nullptr)
			return;

		for (int i = 0; i < depth; ++i)
		{
			std::cout << "    ";
		}

		// Выводим информацию о прямоугольнике текущего узла
		std::cout << "Область узла: ( " << node->rect.x1 << "; " << node->rect.y1 << "; " << node->rect.x2 << "; " << node->rect.y2 << " )" << std::endl;

		if (node->isLeaf)
		{
			for (int i = 0; i < depth + 1; ++i)
			{
				std::cout << "    ";
			}

			// Выводим информацию о точках в листовом узле
			std::cout << "Точки:\n";
			for (size_t i = 0; i < node->points.size(); ++i)
			{
				for (int j = 0; j < depth + 2; ++j)
				{
					std::cout << "    ";
				}
				std::cout << "( " << node->points[i].x << "; " << node->points[i].y << " )\n";
			}
		}
		else
		{
			// Рекурсивно выводим информацию о дочерних узлах
			for (Node* child : node->childs)
			{
				std::cout << std::endl;
				printNode(child, depth + 1);
			}
		}
	}


private:
	int min;
	int max; 
	int child; 

	Node* root;
};

int main()
{
	system("chcp 1251 & cls");

	RTree rtree;

	Point p1 = { 1.0, 2.0 };
	Point p2 = { 3.0, 4.0 };
	Point p3 = { 4.5, 5.0 };
	Point p4 = { 6.0, 6.5 };
	Point p5 = { 7.0, 8.0 };
	Point p6 = { 9.0, 9.2 };
	Point p7 = { 9.6, 10.0 };
	Point p8 = { 11.0, 12.0 };

	rtree.addPoint(p1);
	rtree.addPoint(p2);
	rtree.addPoint(p3);
	rtree.addPoint(p4);
	rtree.addPoint(p5);
	rtree.addPoint(p6);
	rtree.addPoint(p7);
	rtree.addPoint(p8);

	rtree.removePoint(p5);

	rtree.print();

	std::cout << std::endl;
	rtree.searchPoint(p4);
	rtree.searchPoint(p5);

	std::cout << std::endl;
	Rect rect = { 4.0, 4.0, 7.0, 7.0 };
	rtree.searchRect(rect);

	std::cout << std::endl;
	Point point = { 8.6, 8.9 };
	rtree.searchExact(point);

	return 0;
}