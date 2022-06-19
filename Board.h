#pragma once
#include <vector>
#include "Cell.h"
#include "Cluster.h"

class Board
{
private:
	int rows;
	int columns;
	int** place;
	std::vector<Cluster> clusters; // массив всех кластеров
	int mode; // вариант воздействия тока (постояннотоковый, переменнотоковый)
	double createAtom; // вероятность появления атома
	double transitAtom; // вероятность перемещения атома, если он не прикреплен к кластеру
	double mergeCluster; // вероятность склеивания кластеров
	std::vector<Cell> queue; // очередь дейтсвий (перемещение кластеров)
	int atoms;

	void Resize(); // изменение размеров поверхности
	int Rows();
	Cell AddAtom(int row);
	void CheckCluster(Cell current, std::vector<Cell>& candidates, int**& tempPlace); // проверка данного поля на возможность образования кластера
	void CreateCluster(Cell current); // создание нового кластера
	Cell AtomTransition(Cell current); // функция перемещения атома
	int ClusterSearch(int number); // поиск кластера в массиве по номеру
	void CheckClusterforClusters(int found); // проверка всего кластера на нахождение других кластеров в его окружении
	void ClusterUncoloring(int found); // стирание кластера с поверхности
	void ClusterColoring(int found); // отрисовка кластера на поверхность
	void ClusterMerger(std::vector<int> candidates); // функция слияния кластеров
	void QueueTransit(); // запуск работы очереди перемещения
	void ClustersSort(); // сортировка кластеров в массиве

public:
	Board(int rows, double addprob, double transitprob, double mergeprob);
	Board(int rows, int mode, double rightborder, double transitprob, double mergeprob);

	void Run(); // запуск работы симулятора
	void Render(); // отрисовка текущего среза системы
	void Conclusion(); // статистика текущего среза системы

	~Board();
};