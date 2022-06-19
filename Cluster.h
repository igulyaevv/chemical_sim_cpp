#pragma once
#include <vector>
#include "Cell.h"

class Cluster
{
private:
	std::vector<Cell> atoms; // ������ ���� ������
	bool isOnTheSurface; // ������ - �� �����������
	bool isDeleted; // ������ - ������
	bool isUp; // ������ - ����������� (����� ������������ �����������)
	int clusterNumber; // ����� ��������
	int adjoined; // ���������� ������������� � ����������� ������
	Cell min; // ���������� ����������� ��������� (min x, min y)
	Cell max; // ���������� ������������ ��������� (max x, max y)

	bool isMerged; // ��������� ������� - ������ - ����������� � ������� ��������

public:
	static int IndexCluster; // ��������� ������ ��������
	Cluster();
	void Add_Atom(Cell atom);
	void Separation(int step); // ������� ��������� �������� �� �����������
	void Transition(int step, int rows); // ������� ����������� �������� ����� �����������
	void Merger(int clusterNumber, std::vector<Cluster>& clusters); // ������� ���������� ���������
	int Size();
	int Number();
	bool IsOnTheSurface();
	bool IsDeleted();
	bool IsUp();
	void ChangeWay(); // ������� ��������� ����������� �������� ����� ����������
	void DefineMinMax(); // ���������� ����������� � ������������ ��������
	int Adjoined();
	std::vector<Cell> Atoms();
	Cell Max();
	Cell Min();

	void NotIsMerged(); // ��������� ������� - ��������� ������� ������������
	bool IsMerged();
};