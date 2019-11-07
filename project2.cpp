#include <iostream>

using namespace std;

class node 
{
    friend class robot;
    public:
        void setnode(char sign);
        void print() {
            if (wall)
                cout << 'w';
            else
                cout << distance;
        }

    private:
        int distance;
        bool passed;
        bool wall; 
};

class robot 
{
    public:
        robot(int r, int c, int b);
        void in();
        void print();
        void setdistance();
        void setdistance(int r, int c, int d);

    private:
        node** floor;
        int row;
        int col;
        int battery;
        int charge_r, charge_c;
};

int main() 
{
    int row, col, battery;
    cin >> row >> col >> battery;

    robot c(row, col, battery);
    c.in();
    c.setdistance();
    c.print();

    return 0;
}

void node::setnode(char sign) 
{
    distance = -1;
    passed = 0;
    wall = (sign == '1');
}

robot::robot(int r, int c, int b)
{
    row = r; col = c; battery = b;
    floor = new node* [row];
    for (int i = 0; i < row; i++) 
        floor[i] = new node[col];
}

void robot::in()
{
    char sign;

    for (int i = 0; i < row; i++) {
        for (int j = 0; j < col; j++) { 
            cin >> sign;
            if (sign == 'R') {
                charge_r = i; charge_c = j;
            }
            floor[i][j].setnode(sign);
        }  
    }  
}

void robot::print()
{
    printf("\n%d %d %d\n", row, col, battery);
    for (int i = 0; i < row; i++) {
        for (int j = 0; j < col; j++) { 
            floor[i][j].print();
        }  
        cout << endl;
    } 
}

void robot::setdistance() 
{
    floor[charge_r][charge_c].distance = 0; 
    setdistance(charge_r + 1, charge_c, 1);
    setdistance(charge_r - 1, charge_c, 1);
    setdistance(charge_r, charge_c + 1, 1);
    setdistance(charge_r, charge_c - 1, 1);   
}

void robot::setdistance(int r, int c, int d)
{
    floor[r][c].distance = d;
    setdistance(r + 1, c, d + 1);
    setdistance(r - 1, c, d + 1);
    setdistance(r, c + 1, d + 1);
    setdistance(r, c - 1, d + 1);
}