#pragma once
#include <vector>
#include "Cell.h"

class Cluster
{
private:
	std::vector<Cell> atoms; // массив всех атомов
	bool isOnTheSurface; // статус - на поверхности
	bool isDeleted; // статус - удален
	bool isUp; // статус - направление (вверх относительно поверхности)
	int clusterNumber; // номер кластера
	int adjoined; // количество прикрепленных к поверхности атомов
	Cell min; // показатель минимальных элементов (min x, min y)
	Cell max; // показатель максимальных элементов (max x, max y)

	bool isMerged; // временное решение - статус - прикрепился к другому кластеру

public:
	static int IndexCluster; // генератор номера кластера
	Cluster();
	void Add_Atom(Cell atom);
	void Separation(int step); // функция отделения кластера от поверхности
	void Transition(int step, int rows); // функция перемещения кластера вдоль поверхности
	void Merger(int clusterNumber, std::vector<Cluster>& clusters); // функция склеивания кластеров
	int Size();
	int Number();
	bool IsOnTheSurface();
	bool IsDeleted();
	bool IsUp();
	void ChangeWay(); // функция изменения направления движения вдоль повехности
	void DefineMinMax(); // перерасчет минимальных и максимальных значений
	int Adjoined();
	std::vector<Cell> Atoms();
	Cell Max();
	Cell Min();

	void NotIsMerged(); // временное решение - изменение статуса прикрепления
	bool IsMerged();
};