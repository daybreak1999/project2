#include <iostream>

using namespace std;

class node {

    public:
        node() {};
        void setnode(int r, int c, char sign);
        void print() {cout << wall;};

    private:
        int distance;
        bool passed;
        int row;
        int col;
        bool wall; 
};

int main() 
{
    int row, col, battery;
    cin >> row >> col >> battery;
    char sign;

    // set dynamic array
    node** floor;
    floor = new node* [row];
    for (int i = 0; i < row; i++) 
        floor[i] = new node[col];

    for (int i = 0; i < row; i++) {
        for (int j = 0; j < col; j++) { 
            cin >> sign;
            floor[i][j].setnode(i, j, sign);
        }  
    }  
    cout <<  row << col << battery << endl;
    for (int i = 0; i < row; i++) {
        for (int j = 0; j < col; j++) { 
            floor[i][j].print();
        }  
        cout << endl;
    } 

    return 0;
}

void node::setnode(int r, int c, char sign) 
{
    distance = 0;
    passed = 0;
    row = r; col = c;
    wall = (sign == '1');
}

