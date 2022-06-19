#include <iostream>
#include <fstream>
#include <string>
#include "Board.h"

using namespace std;

int Cluster::IndexCluster = 0;

int main()
{
    setlocale(LC_ALL, "Rus");

    int choise;
    cout << "Выберите способ ввода входных параметров: 1 - из файла; 2 - с клавиатуры." << endl;
    cin >> choise;

    int N;
    int mode; // вариант тока - постоянно- или переменнотоковый
    double b; //вероятность появления
    double ts; //вероятность перехода
    double u; //вероятность слияния кластеров не на поверхности
    int G; //число проходов по сетке

    if (choise != 2)
    {
        ifstream fin("cfg.txt");
        if (fin.is_open())
        {
            fin >> N;
            fin >> mode;
            fin >> b;
            fin >> ts;
            fin >> u;
            fin >> G;

            cout << "Количество секторов сетки: " << N << endl;
            cout << "Режим: ";
            if (mode == 0)
                cout << "- постояннотоковый" << endl << "Вероятность появления атома: ";
            else if (mode == 1)
                cout << "- переменнотоковый" << endl << "Правая граница вероятности появления атома: ";
            else
            {
                cout << "Выбран некорректный режим" << endl;
                return 1;
            }
            cout << b << endl;
            cout << "Вероятность перехода атома в соседнюю ячейку: " << ts << endl;
            cout << "Вероятность слияния кластеров при столкновении их не на поверхности: " << u << endl;
            cout << "Количество проходов по сетке: " << G << endl;
        }
        else
        {
            cout << "Файл не найден!";
            return 2;
        }
        fin.close();
    }
    else
    {
        cout << "Введите число N - количество секторов сетки: ";
        cin >> N;
        cout << "Задание вероятностей" << endl << "Выберите режим: 0 - постояннотоковый, 1 - переменнотоковый: ";
        cin >> mode;
        if (mode == 0)
            cout << "Вероятность появления атома: ";
        else if (mode == 1)
            cout << "Введите правую границу вероятности появления: ";
        else
        {
            cout << "Выбран некорректный режим" << endl;
            return 1;
        }
        cin >> b;
        cout << "Вероятность перехода атома в соседнюю ячейку: ";
        cin >> ts;
        cout << "Вероятность слияния кластеров при столкновении их не на поверхности: ";
        cin >> u;
        cout << "Введите количество G проходов по сетке: ";
        cin >> G;
    }

    if (N < 0 || b < 0 || b > 1 || ts < 0 || ts > 1 || u < 0 || u > 1 || G < 0 || mode > 1 || mode < 0) // все эти проверки следует упростить
    {
        cout << "Введены некорректные данные" << endl;
        return 3;
    }

    char vote;
    cout << "Все данные введены правильно? Y/N" << endl;
    cin >> vote;
    if (vote == 'N')
        return 4;

    Board board(N, mode, b, ts, u);

    int step = 0;
    while (step < G)
    {
        board.Run();
        step++;
    }
    board.Conclusion();

    cout << endl << "Для выхода введите любой символ";
    cin >> vote;
    return 5;
}