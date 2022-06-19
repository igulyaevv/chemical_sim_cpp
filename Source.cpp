#include <iostream>
#include <string>
#include <fstream>
#include <vector>
#include <set>
#include <time.h>
#include <windows.h> // дл€ Sleep в windows
// #include <unistd.h> // дл€ Sleep в linux
#include "Cell.h"
#include "Board.h"
#include "Cluster.h"

using namespace std;

//Cell place//

Cell::Cell()
{
	x = 0;
	y = 0;
}

Cell::Cell(int i, int j)
{
	this->x = i;
	this->y = j;
}

int Cell::get_x()
{
	return x;
}

int Cell::get_y()
{
	return y;
}

void Cell::set_x(int i)
{
	this->x = i;
}

void Cell::set_y(int j)
{
	this->y = j;
}

//Board place//

Board::Board(int rows, double addprob, double transitprob, double mergeprob)
{
	mode = 0;
	this->rows = rows;
	columns = 3;
	place = new int* [rows];
	for (int i = 0; i < rows; i++)
	{
		place[i] = new int[columns];
		for (int j = 0; j < columns; j++)
			place[i][j] = -1;
	}
	createAtom = addprob;
	transitAtom = transitprob;
	mergeCluster = mergeprob;
	atoms = 0;
}

Board::Board(int rows, int mode, double rightborder, double transitprob, double mergeprob) : Board(rows, rightborder, transitprob, mergeprob)
{
	this->mode = mode;
}

int Board::Rows()
{
	return rows;
}

void Board::Resize()
{
	int** new_Place = new int* [rows];
	for (int i = 0; i < rows; i++)
	{
		new_Place[i] = new int[columns + 3];
	}

	for (int i = 0; i < rows; i++)
		for (int j = 0; j < columns + 3; j++)
		{
			if (j < columns)
				new_Place[i][j] = place[i][j];
			else
				new_Place[i][j] = -1;
		}
	columns += 3;

	if (place)
	{
		for (int i = 0; i < rows; i++)
		{
			delete[] place[i];
		}
		delete[] place;
	}

	place = new_Place;
	new_Place = nullptr;
}

Cell Board::AddAtom(int row)
{
	int j = 0;
	if (place[row][0] == -1)
		place[row][0] = 0;
	else
	{
		while (place[row][j] != -1 && j < columns)
			j++;
		if (j >= columns)
			Resize();
		place[row][j] = 0;
	}
	Cell Temp(row, j);

	return Temp;
}

void Board::CheckCluster(Cell current, vector<Cell>& candidates, int**& tempPlace)
{
	if (current.get_x() >= 0 && current.get_y() >= 0 && current.get_x() < rows && current.get_y() < columns && place[current.get_x()][current.get_y()] != -1 && tempPlace[current.get_x()][current.get_y()] != 1)
	{
		Cell candidate(current.get_x(), current.get_y());
		tempPlace[current.get_x()][current.get_y()] = 1;
		candidates.push_back(candidate);

		CheckCluster(Cell(current.get_x() - 1, current.get_y()), candidates, tempPlace);
		CheckCluster(Cell(current.get_x() + 1, current.get_y()), candidates, tempPlace);
		CheckCluster(Cell(current.get_x(), current.get_y() - 1), candidates, tempPlace);
		CheckCluster(Cell(current.get_x(), current.get_y() + 1), candidates, tempPlace);
		CheckCluster(Cell(current.get_x() + 1, current.get_y() + 1), candidates, tempPlace);
		CheckCluster(Cell(current.get_x() + 1, current.get_y() - 1), candidates, tempPlace);
		CheckCluster(Cell(current.get_x() - 1, current.get_y() + 1), candidates, tempPlace);
		CheckCluster(Cell(current.get_x() - 1, current.get_y() - 1), candidates, tempPlace);
	}
}

void Board::CreateCluster(Cell current)
{
	int min_clusternumber = 0;
	int** tempPlace = new int* [rows];
	for (int i = 0; i < rows; i++)
	{
		tempPlace[i] = new int[columns];
		for (int j = 0; j < columns; j++)
			tempPlace[i][j] = 0;
	}
	vector<Cell> candidates;
	CheckCluster(current, candidates, tempPlace);
	int count = candidates.size();
	for (int i = 0; i < rows; i++)
	{
		delete[] tempPlace[i];
	}
	delete[] tempPlace;

	for (int i = 0; i < count; i++)
	{
		if (place[candidates[i].get_x()][candidates[i].get_y()] != 0)
		{
			if (min_clusternumber == 0)
				min_clusternumber = place[candidates[i].get_x()][candidates[i].get_y()];
			else if (place[candidates[i].get_x()][candidates[i].get_y()] < min_clusternumber)
				min_clusternumber = place[candidates[i].get_x()][candidates[i].get_y()];
		}
	}

	if (min_clusternumber == 0)
	{
		if (count >= 4)
		{
			Cluster new_cluster;
			for (int i = 0; i < count; i++)
				new_cluster.Add_Atom(candidates[i]);
			clusters.push_back(new_cluster);
			int found = ClusterSearch(new_cluster.Number());
			ClusterColoring(found);
		}
	}
	else
	{
		int found = ClusterSearch(min_clusternumber);
		set<int> merging;
		for (int i = 0; i < count; i++)
		{
			if (place[candidates[i].get_x()][candidates[i].get_y()] == min_clusternumber)
				continue;
			else if (place[candidates[i].get_x()][candidates[i].get_y()] != 0)
			{
				int another_found = ClusterSearch(place[candidates[i].get_x()][candidates[i].get_y()]);
				merging.insert(another_found);
				/*
				clusters[found].Merger(another_found, clusters);
				clusters[another_found].NotIsMerged();
				ClusterColoring(found);*/
				// можно сначала присоединить свободные атомы, но не к минимальному,
				// а к креп€щемус€ минимальному, т.е. еще раз проверить ее окружение и соединить с мимнимальным

				// в таком подходе может быть проблема, если проверка идет не по пор€дку в векторе candidats, т.к. если есть 000 и мы будем рассматривать первый ноль первым,
				// то он ни к чему не присоединитс€, возможно нужно сделать доп. переменную и доп. проверку в некст else
			}
			else
			{
				int new_found = -1;
				if (candidates[i].get_x() - 1 >= 0 && place[candidates[i].get_x() - 1][candidates[i].get_y()] > 0)
					new_found = ClusterSearch(place[candidates[i].get_x() - 1][candidates[i].get_y()]);

				if (candidates[i].get_x() + 1 < rows && place[candidates[i].get_x() + 1][candidates[i].get_y()] > 0)
					new_found = ClusterSearch(place[candidates[i].get_x() + 1][candidates[i].get_y()]);

				if (candidates[i].get_y() - 1 >= 0 && place[candidates[i].get_x()][candidates[i].get_y() - 1] > 0)
					new_found = ClusterSearch(place[candidates[i].get_x()][candidates[i].get_y() - 1]);

				if (candidates[i].get_y() + 1 < columns && place[candidates[i].get_x()][candidates[i].get_y() + 1] > 0)
					new_found = ClusterSearch(place[candidates[i].get_x()][candidates[i].get_y() + 1]);

				if (candidates[i].get_x() - 1 >= 0 && candidates[i].get_y() - 1 >= 0 && place[candidates[i].get_x() - 1][candidates[i].get_y() - 1] > 0)
					new_found = ClusterSearch(place[candidates[i].get_x() - 1][candidates[i].get_y() - 1]);

				if (candidates[i].get_x() - 1 >= 0 && candidates[i].get_y() + 1 < columns && place[candidates[i].get_x() - 1][candidates[i].get_y() + 1] > 0)
					new_found = ClusterSearch(place[candidates[i].get_x() - 1][candidates[i].get_y() + 1]);

				if (candidates[i].get_x() + 1 < rows && candidates[i].get_y() - 1 >= 0 && place[candidates[i].get_x() + 1][candidates[i].get_y() - 1] > 0)
					new_found = ClusterSearch(place[candidates[i].get_x() + 1][candidates[i].get_y() - 1]);

				if (candidates[i].get_x() + 1 < rows && candidates[i].get_y() + 1 < columns && place[candidates[i].get_x() + 1][candidates[i].get_y() + 1] > 0)
					new_found = ClusterSearch(place[candidates[i].get_x() + 1][candidates[i].get_y() + 1]);

				if (new_found != -1)
				{
					clusters[new_found].Add_Atom(candidates[i]);
					ClusterColoring(new_found);
				}
			}
		}
		if (merging.size() > 0)
		{
			vector<int> temp(merging.begin(), merging.end());
			temp.push_back(found);
			ClusterMerger(temp);
		}
	}
}

Cell Board::AtomTransition(Cell current)
{
	double randway = 0.001 * (rand() % 1001);
	bool UporDown = true;
	if (randway > 0.5)
		UporDown = false;

	if (UporDown && current.get_x() - 1 >= 0)
	{
		place[current.get_x()][current.get_y()] = -1;
		current = AddAtom(current.get_x() - 1);
	}
	else if (!UporDown && current.get_x() + 1 < rows)
	{
		place[current.get_x()][current.get_y()] = -1;
		current = AddAtom(current.get_x() + 1);
	}

	return current;
}

int Board::ClusterSearch(int number)
{
	int found = -1;
	for (int i = 0; i < clusters.size(); i++)
	{
		if (clusters[i].Number() == number)
			found = i;
	}
	return found;
}

void Board::CheckClusterforClusters(int found)
{
	int clusterNumber = clusters[found].Number();
	for (int i = 0; i < clusters[found].Size(); i++)
	{
		int x = clusters[found].Atoms()[i].get_x();
		int y = clusters[found].Atoms()[i].get_y();
		if (x >= 0 && y >= 0 && x < rows && y < columns)
			if (x - 1 >= 0 && place[x - 1][y] != clusterNumber && place[x - 1][y] != -1 ||
				x + 1 < rows && place[x + 1][y] != clusterNumber && place[x + 1][y] != -1 ||
				y - 1 >= 0 && place[x][y - 1] != clusterNumber && place[x][y - 1] != -1 ||
				y + 1 < columns && place[x][y + 1] != clusterNumber && place[x][y + 1] != -1 ||
				x - 1 >= 0 && y - 1 >= 0 && place[x - 1][y - 1] != clusterNumber && place[x - 1][y - 1] != -1 ||
				x - 1 >= 0 && y + 1 < columns && place[x - 1][y + 1] != clusterNumber && place[x - 1][y + 1] != -1 ||
				x + 1 < rows && y - 1 >= 0 && place[x + 1][y - 1] != clusterNumber && place[x + 1][y - 1] != -1 ||
				x + 1 < rows && y + 1 < columns && place[x + 1][y + 1] != clusterNumber && place[x + 1][y + 1] != -1)
			{
				CreateCluster(clusters[found].Atoms()[i]);
				return;
			}
	}
}

void Board::ClusterUncoloring(int found)
{
	for (int i = 0; i < clusters[found].Atoms().size(); i++)
		if (clusters[found].Atoms()[i].get_x() >= 0 && clusters[found].Atoms()[i].get_x() < rows && clusters[found].Atoms()[i].get_y() >= 0 && clusters[found].Atoms()[i].get_y() < columns)
			place[clusters[found].Atoms()[i].get_x()][clusters[found].Atoms()[i].get_y()] = -1;
}

void Board::ClusterColoring(int found)
{
	for (int i = 0; i < clusters[found].Atoms().size(); i++)
	{
		if (clusters[found].Atoms()[i].get_y() >= columns)
			Resize();
		if (clusters[found].Atoms()[i].get_x() >= 0 && clusters[found].Atoms()[i].get_x() < rows && clusters[found].Atoms()[i].get_y() >= 0 && clusters[found].Atoms()[i].get_y() < columns)
			place[clusters[found].Atoms()[i].get_x()][clusters[found].Atoms()[i].get_y()] = clusters[found].Number();
	}
}

void Board::ClusterMerger(vector<int> candidates)
{
	if (candidates.size() > 2)
	{
		for (int i = 0; i < candidates.size() - 1; i++)
		{
			clusters[candidates.size() - 1].Merger(candidates[i], clusters);
			clusters[candidates[i]].NotIsMerged();
		}
		ClusterColoring(candidates.size() - 1);
	}
	else
	{
		double randmerge = 0.001 * (rand() % 1001);
		if (randmerge <= mergeCluster || clusters[candidates[0]].IsOnTheSurface() || clusters[candidates[1]].IsOnTheSurface())
		{
			clusters[candidates[1]].Merger(candidates[0], clusters);
			clusters[candidates[0]].NotIsMerged();
		}
		else
		{
			int repulsion_index;
			int stay_index;
			if (clusters[candidates[0]].Size() >= clusters[candidates[1]].Size())
			{
				repulsion_index = candidates[1];
				stay_index = candidates[0];
			}
			else
			{
				repulsion_index = candidates[0];
				stay_index = candidates[1];
			}

			if (clusters[repulsion_index].Min().get_x() >= clusters[stay_index].Max().get_x())
			{
				if (clusters[repulsion_index].IsUp())
					clusters[repulsion_index].ChangeWay();
			}
			else
			{
				if (!clusters[repulsion_index].IsUp())
					clusters[repulsion_index].ChangeWay();
			}

			ClusterUncoloring(repulsion_index);
			clusters[repulsion_index].Transition(1, rows);
			ClusterColoring(repulsion_index);
		}
	}
}

void Board::QueueTransit()
{
	for (int i = 0; i < queue.size(); i++)
	{
		int found = ClusterSearch(queue[i].get_x());
		if (found == -1 || clusters[found].IsDeleted() || clusters[found].IsMerged())
		{
			queue.erase(queue.begin() + i); // € удал€ю кластер, если функци€ поиска возвращает -1, это норм?
		}
		else
		{
			if (queue[i].get_y() > 0)
			{
				queue[i].set_y(queue[i].get_y() - 1);
				CheckClusterforClusters(found); //здесь если что 100 % произойдет сли€ние
				if (clusters[found].IsMerged())
				{
					queue.erase(queue.begin() + i);
					i--;
					continue;
				}
				ClusterUncoloring(found);
				clusters[found].Separation(1);
				ClusterColoring(found);
			}
			else
			{
				int random = rand() % clusters[found].Size();
				int V = 1 + random > clusters[found].Size() ? clusters[found].Size() : 1 + random;
				if (V >= rows)
					V = rows - 1;
				bool exit = false;

				for (int j = V; j > 0 && !exit; j--)
				{
					CheckClusterforClusters(found);
					if (clusters[found].IsMerged() || clusters[found].IsOnTheSurface())
					{
						queue.erase(queue.begin() + i);
						i--;
						exit = true;
						continue;
					}
					ClusterUncoloring(found);
					clusters[found].Transition(1, rows);
					ClusterColoring(found);
				}
			}
		}
	}
}

void Board::ClustersSort()
{
	if (clusters.size() <= 1)
		return;
	for (int i = 0; i < clusters.size() - 1; i++)
		for (int j = 0; j < clusters.size() - i - 1; j++)
			if (clusters[j].Size() > clusters[j + 1].Size())
				swap(clusters[j], clusters[j + 1]);
}

void Board::Run()
{
	srand(time(NULL));

	double createAtomProb = createAtom;
	if (mode == 1)
		createAtomProb = 0.001 * (rand() % int(createAtom * 1001));

	for (int i = 0; i < rows; i++)
	{
		double randcreate = 0.001 * (rand() % 1001);

		if (randcreate <= createAtomProb)
		{
			atoms++;
			double position = rand() % rows;
			Cell current = AddAtom(position);
			CreateCluster(current);
			Render();
			Sleep(1);
			if (place[current.get_x()][current.get_y()] == 0)
			{
				double randtransition = 0.001 * (rand() % 1001);
				if (randtransition <= transitAtom)
				{
					current = AtomTransition(current);
					CreateCluster(current);
					Render();
					Sleep(1);
				}
			}
			if (place[current.get_x()][current.get_y()] > 0)
			{
				double randseparation = 0.001 * (rand() % 1001);
				int found = ClusterSearch(place[current.get_x()][current.get_y()]);
				double separationprob = 1.0 - ((double)clusters[found].Adjoined() / (double)clusters[found].Size());
				if (randseparation < separationprob)
				{
					int sep = clusters[found].Size() / 2 <= rows ? clusters[found].Size() / 2 : rows - 1;
					Cell new_separation(place[current.get_x()][current.get_y()], sep);
					queue.push_back(new_separation);
				}
			}
			if (queue.size() != 0)
				QueueTransit();
			Render();
			Sleep(1);
		}
	}
}

void Board::Render()
{
	for (int i = 0; i < rows; i++)
	{
		for (int j = 0; j < columns; j++)
		{
			if (place[i][j] == -1)
				cout << ' ';
			else
				//cout << '*';
				cout << place[i][j];
		}
		cout << endl;
	}
	cout << "----------\n";
}

void Board::Conclusion()
{
	ClustersSort();

	string clusterinfo;
	string distribution = "";

	int AverageSize = 0;
	cout << endl
		<< "---------" << endl
		<< "’арактеристика кластеров" << endl;
	cout << " оличество кластеров: " << clusters.size() << endl
		<< "---" << endl;
	for (int i = 0; i < clusters.size(); i++)
	{
		if (clusters[i].IsMerged()) // временное решение, мб можно как-то по-другому
			continue;

		clusterinfo = "";
		cout << "Ќомер кластера: " << clusters[i].Number() << endl;
		cout << "–азмер кластера (количество атомов): " << clusters[i].Size() << endl;
		clusterinfo += "¬ес: " + to_string(clusters[i].Size()) + '\n';
		cout << " райние точки кластера: по вертикали: " << clusters[i].Max().get_x() << ", " << clusters[i].Min().get_x();
		cout << "; по горизонтали: " << clusters[i].Max().get_y() << ", " << clusters[i].Min().get_y() << endl;
		cout << "—татус: ";
		if (clusters[i].IsOnTheSurface())
		{
			cout << "Ќа поверхности";
			clusterinfo += "Ќа поверхности\n\n";
		}
		else if (!clusters[i].IsDeleted())
		{
			cout << "¬не поверхности, в пределах области видимости";
			clusterinfo += "¬не поверхности, в пределах области видимости\n\n";
		}
		else
		{
			cout << "¬не пределов области видимости";
			clusterinfo += "¬не пределов области видимости\n\n";
		}
		cout << endl
			<< "ќтрисовка кластера" << endl
			<< endl;
		for (int t = 0; t < clusters[i].Max().get_x() - clusters[i].Min().get_x() + 1; t++)
		{
			for (int j = 0; j < clusters[i].Max().get_y() - clusters[i].Min().get_y() + 1; j++)
			{
				for (int k = 0; k < clusters[i].Size(); k++)
				{
					if (clusters[i].Atoms()[k].get_x() - clusters[i].Min().get_x() == t && clusters[i].Atoms()[k].get_y() - clusters[i].Min().get_y() == j)
					{
						cout << '*';
						clusterinfo += '*';
						break;
					}
					if (k == clusters[i].Size() - 1)
					{
						cout << ' ';
						clusterinfo += ' ';
					}
				}
			}
			cout << endl;
			clusterinfo += '\n';
		}
		string path = "ClusterN-";
		path += to_string(clusters[i].Number());
		path += ".txt";

		ofstream fout1;
		fout1.open(path);
		if (!fout1.is_open())
		{
			cout << endl
				<< "ќшибка открыти€ файла " << path << endl;
		}
		else
		{
			fout1 << clusterinfo;
		}
		fout1.close();

		distribution += to_string(clusters[i].Size()) + ' ';

		cout << endl
			<< "---" << endl;

		AverageSize += clusters[i].Size();
	}

	cout << "¬сего присоединилось атомов: " << atoms << endl;
	int notinCluster = 0;
	for (int i = 0; i < rows; i++)
		for (int j = 0; j < columns; j++)
			if (place[i][j] == 0)
				notinCluster++;
	cout << "ѕотер€но атомов: " << atoms - AverageSize - notinCluster << endl;

	if (clusters.size() != 0)
	{
		AverageSize /= clusters.size();
		cout << "ћедиана веса кластеров: ";
		if (clusters.size() % 2 == 0 || clusters.size() == 1 || clusters.size() == 0)
			cout << clusters[clusters.size() / 2].Size();
		else
			cout << (clusters[clusters.size() / 2].Size() + clusters[(clusters.size() / 2) + 1].Size()) / 2;
		cout << endl
			<< "—реднее значение веса кластеров: " << AverageSize << endl;
		cout << "–азмах веса кластеров: " << clusters[clusters.size() - 1].Size() - clusters[0].Size() << endl;
	}

	string path = "WeightAnalysis.txt";
	ofstream fout2;
	fout2.open(path);
	if (!fout2.is_open())
	{
		cout << "ќшибка открыти€ файла " << path << endl;
	}
	else
	{
		fout2 << distribution;
		//system("python GraphPainter.py");
	}
	fout2.close();
}

Board::~Board()
{
	for (int i = 0; i < rows; i++)
	{
		delete[] place[i];
	}
	delete[] place;
}

//Cluster place//

Cluster::Cluster()
{
	isOnTheSurface = true;
	isDeleted = false;
	isUp = true;
	double randway = 0.001 * (rand() % 1001);
	if (randway > 0.5)
		isUp = false;
	clusterNumber = ++IndexCluster;
	adjoined = 0;

	isMerged = false;
}

void Cluster::Add_Atom(Cell atom)
{
	atoms.push_back(atom);
	DefineMinMax();
}

void Cluster::Separation(int step)
{
	isOnTheSurface = false;
	for (int i = 0; i < step; i++)
	{
		for (int j = 0; j < atoms.size(); j++)
		{
			int temp = atoms[j].get_y();
			temp++;
			atoms[j].set_y(temp);
		}
		DefineMinMax();
	}
}

void Cluster::Transition(int step, int rows)
{
	for (int i = 0; i < step; i++)
	{
		if (isUp)
		{
			for (int j = 0; j < atoms.size(); j++)
			{
				int temp = atoms[j].get_x();
				temp--;
				atoms[j].set_x(temp);
			}
			DefineMinMax();
			if (max.get_x() < 0 || min.get_x() >= rows)
				isDeleted = true; // этот повтор€ющийс€ блок скорее всего можно вытащить из ифа в фор
		}
		else
		{
			for (int j = 0; j < atoms.size(); j++)
			{
				int temp = atoms[j].get_x();
				temp++;
				atoms[j].set_x(temp);
			}
			DefineMinMax();
			if (max.get_x() < 0 || min.get_x() >= rows)
				isDeleted = true;
		}
	}
}

void Cluster::Merger(int found, vector<Cluster>& clusters)
{
	for (int i = 0; i < clusters[found].Size(); i++)
	{
		this->Add_Atom(clusters[found].Atoms()[i]);
	}
	this->isDeleted = this->isDeleted && clusters[found].IsDeleted();
	this->isOnTheSurface = this->isOnTheSurface || clusters[found].IsOnTheSurface();
}

int Cluster::Size()
{
	return atoms.size();
}

int Cluster::Number()
{
	return clusterNumber;
}

bool Cluster::IsOnTheSurface()
{
	return isOnTheSurface;
}

bool Cluster::IsDeleted()
{
	return isDeleted;
}

bool Cluster::IsUp()
{
	return isUp;
}

void Cluster::ChangeWay()
{
	isUp = !isUp;
}

void Cluster::DefineMinMax()
{
	int x_min = atoms[0].get_x();
	int x_max = atoms[0].get_x();
	int y_min = atoms[0].get_y();
	int y_max = atoms[0].get_y();
	int temp = 0;

	for (int i = 0; i < atoms.size(); i++)
	{
		if (atoms[i].get_x() < x_min)
			x_min = atoms[i].get_x();

		if (atoms[i].get_x() > x_max)
			x_max = atoms[i].get_x();

		if (atoms[i].get_y() < y_min)
			y_min = atoms[i].get_y();

		if (atoms[i].get_y() > y_max)
			y_max = atoms[i].get_y();

		if (atoms[i].get_y() == 0)
			temp++;
	}

	adjoined = temp; // зачем лишн€€ переменна€ temp ?
	max.set_x(x_max);
	max.set_y(y_max);
	min.set_x(x_min);
	min.set_y(y_min);

	if (y_min != 0)
		isOnTheSurface = false;
	else
		isOnTheSurface = true;
}

int Cluster::Adjoined()
{
	return adjoined;
}

vector<Cell> Cluster::Atoms()
{
	return atoms;
}

Cell Cluster::Max()
{
	return max;
}

Cell Cluster::Min()
{
	return min;
}

void Cluster::NotIsMerged()
{
	isMerged = !isMerged;
}

bool Cluster::IsMerged()
{
	return isMerged;
}
