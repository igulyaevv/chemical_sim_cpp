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
	std::vector<Cluster> clusters; // ������ ���� ���������
	int mode; // ������� ����������� ���� (����������������, ����������������)
	double createAtom; // ����������� ��������� �����
	double transitAtom; // ����������� ����������� �����, ���� �� �� ���������� � ��������
	double mergeCluster; // ����������� ���������� ���������
	std::vector<Cell> queue; // ������� �������� (����������� ���������)
	int atoms;

	void Resize(); // ��������� �������� �����������
	int Rows();
	Cell AddAtom(int row);
	void CheckCluster(Cell current, std::vector<Cell>& candidates, int**& tempPlace); // �������� ������� ���� �� ����������� ����������� ��������
	void CreateCluster(Cell current); // �������� ������ ��������
	Cell AtomTransition(Cell current); // ������� ����������� �����
	int ClusterSearch(int number); // ����� �������� � ������� �� ������
	void CheckClusterforClusters(int found); // �������� ����� �������� �� ���������� ������ ��������� � ��� ���������
	void ClusterUncoloring(int found); // �������� �������� � �����������
	void ClusterColoring(int found); // ��������� �������� �� �����������
	void ClusterMerger(std::vector<int> candidates); // ������� ������� ���������
	void QueueTransit(); // ������ ������ ������� �����������
	void ClustersSort(); // ���������� ��������� � �������

public:
	Board(int rows, double addprob, double transitprob, double mergeprob);
	Board(int rows, int mode, double rightborder, double transitprob, double mergeprob);

	void Run(); // ������ ������ ����������
	void Render(); // ��������� �������� ����� �������
	void Conclusion(); // ���������� �������� ����� �������

	~Board();
};