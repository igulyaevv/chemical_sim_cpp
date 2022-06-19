#pragma once

class Cell
{
private:
    int x; //i
    int y; //j

public:
    Cell();
    Cell(int i, int j);
    int get_x();
    int get_y();
    void set_x(int i);
    void set_y(int j);
};