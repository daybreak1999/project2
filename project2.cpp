#include <iostream>
#include <fstream>

using namespace std;

ifstream inFile("floor.data", ios::in); 
ofstream outFile("final.path", ios::out);

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

class Step
{
    friend class chain;
    public:
        Step(int r, int c): row(r), col(c), next(NULL) {}
    private:
        int row;
        int col;
        Step* next;
};

class chain
{
    friend class robot;
    public:
        chain() {first = last = NULL;}
        void insert(int r, int c);
        void reverse();
        void add(chain &b);
        void deletefirst();
        void print();
    private:
        Step* first;
        Step* last;
};

class robot 
{
    public:
        robot(int r, int c, int b);
        void in();
        void print();
        void setdistance();
        void setdistance(int r, int c, int d);
        void run();
        void expedition(int r, int c);
        void roam(int r, int c);
        void home(int r, int c);
    private:
        node** floor;
        chain path;
        int row;
        int col;
        int battery;
        int q;
        int charge_r, charge_c;
        int max_d;
        int dirty;
        long int step;
};

int main() 
{
    
    int row, col, battery;
    inFile >> row >> col >> battery;

    robot c(row, col, battery);
    
    char sign;

    c.in();
    c.setdistance();
    c.run();
    c.print();

    return 0;
}

// node function
void node::setnode(char sign) 
{
    distance = -1;
    if (sign == 'R')
        passed = 1;
    else
        passed = 0;
    wall = (sign == '1');
}

// chain functions
void chain::insert(int r, int c)
{
    if (first) {
        last->next = new Step(r, c);
        last = last->next;
    }
    else
        first = last = new Step(r, c);
}

void chain::reverse()
{
    Step *current = first, *previous = NULL; 
    while (current) {
        Step *r = previous;
        previous = current;
        current = current->next;
        previous->next = r;
    }
    last = first;
    first = previous;
}

void chain::add(chain &b)
{
    if (first) {last->next = b.first; last = b.last;}
    else {first = b.first; last = b.last;}
    b.first = b.last = 0;
}
void chain::deletefirst()
{
    if (first) {first = first->next;}
}
void chain::print()
{
    for (Step *i = first; i != NULL; i = i->next) 
        outFile << i->row << " " << i->col << endl;
}

// robot functions
robot::robot(int r, int c, int b)
{
    row = r; col = c; q = battery = b; max_d = dirty = step = 0;
    floor = new node* [row];
    for (int i = 0; i < row; i++) 
        floor[i] = new node[col];
}

void robot::in()
{
    char sign;

    for (int i = 0; i < row; i++) {
        for (int j = 0; j < col; j++) { 
            inFile >> sign;
            if (sign == '\n')
                inFile >> sign;
            if (sign == 'R') {
                charge_r = i; charge_c = j;
                path.insert(i, j);
            }
            floor[i][j].setnode(sign);
        }  
    }
    inFile.close();
}

void robot::print()
{
    outFile << step << endl;
    path.print();
    outFile.close();
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
    if (c >= col - 1|| c <= 0 || r >= row - 1|| r <= 0) 
        return;   
    if (floor[r][c].wall || floor[r][c].distance <= d && floor[r][c].distance != -1)
        return;
    if (floor[r][c].distance <= d)
        dirty++;
    floor[r][c].distance = d;
    if (d > max_d) max_d = d;
    setdistance(r + 1, c, d + 1);
    setdistance(r - 1, c, d + 1);
    setdistance(r, c + 1, d + 1);
    setdistance(r, c - 1, d + 1);
}

void robot::run()
{
    int k = max_d;
    for (int k = max_d; k > 0; k--) {
        for (int i = 1; i < row - 1; i++) {
            for (int j = 1; j < col -1; j++) {
                if (floor[i][j].distance == k && !floor[i][j].passed) { 
                    expedition(i, j);
                    roam(i, j);
                }
                if (dirty <= 0) return;
            }
        }
    }
}

void robot::expedition(int r, int c)
{
    chain exped;
    exped.insert(r ,c);
    floor[r][c].passed = 1; dirty--;

    while (r != charge_r || c != charge_c) {
        // goto block haven't passed 
        if (floor[r + 1][c].distance < floor[r][c].distance && !floor[r + 1][c].wall && !floor[r + 1][c].passed) {
            exped.insert(++r, c); step++; dirty--; q--; floor[r][c].passed = 1; continue;
        }
        if (floor[r][c + 1].distance < floor[r][c].distance && !floor[r][c + 1].wall && !floor[r][c + 1].passed) {
            exped.insert(r, ++c); step++; dirty--; q--; floor[r][c].passed = 1; continue;
        }        
        if (floor[r - 1][c].distance < floor[r][c].distance && !floor[r - 1][c].wall && !floor[r - 1][c].passed) {
            exped.insert(--r, c); step++; dirty--; q--; floor[r][c].passed = 1; continue;
        }
        if (floor[r][c - 1].distance < floor[r][c].distance && !floor[r][c - 1].wall && !floor[r][c - 1].passed) {
            exped.insert(r, --c); step++; dirty--; q--; floor[r][c].passed = 1; continue;
        }

        // goto block have passed
        if (floor[r + 1][c].distance < floor[r][c].distance && !floor[r + 1][c].wall) {
            exped.insert(++r, c); step++; q--; continue;
        }
        if (floor[r][c + 1].distance < floor[r][c].distance && !floor[r][c + 1].wall) {
            exped.insert(r, ++c); step++; q--; continue;
        }        
        if (floor[r - 1][c].distance < floor[r][c].distance && !floor[r - 1][c].wall) {
            exped.insert(--r, c); step++; q--; continue;
        }
        if (floor[r][c - 1].distance < floor[r][c].distance && !floor[r][c - 1].wall) {
            exped.insert(r, --c); step++; q--; continue;
        }        
    }

    exped.reverse();
    exped.deletefirst();
    path.add(exped);
}

void robot::roam(int r, int c)
{
    while (q - floor[r][c].distance >= 2  && (r != charge_r || c != charge_c)) {
        // goto block haven't passed & far
        if (floor[r + 1][c].distance > floor[r][c].distance && !floor[r + 1][c].wall && !floor[r + 1][c].passed) {
            path.insert(++r, c); step++; dirty--; q--; floor[r][c].passed = 1; continue;
        }
        if (floor[r][c + 1].distance > floor[r][c].distance && !floor[r][c + 1].wall && !floor[r][c + 1].passed) {
            path.insert(r, ++c); step++; dirty--; q--; floor[r][c].passed = 1; continue;
        }        
        if (floor[r - 1][c].distance > floor[r][c].distance && !floor[r - 1][c].wall && !floor[r - 1][c].passed) {
            path.insert(--r, c); step++; dirty--; q--; floor[r][c].passed = 1; continue;
        }
        if (floor[r][c - 1].distance > floor[r][c].distance && !floor[r][c - 1].wall && !floor[r][c - 1].passed) {
            path.insert(r, --c); step++; dirty--; q--; floor[r][c].passed = 1; continue;
        }
        // go back block haven't passed         
        if (floor[r + 1][c].distance < floor[r][c].distance && !floor[r + 1][c].wall && !floor[r + 1][c].passed) {
            path.insert(++r, c); step++; dirty--; q--; floor[r][c].passed = 1; continue;
        }
        if (floor[r][c + 1].distance < floor[r][c].distance && !floor[r][c + 1].wall && !floor[r][c + 1].passed) {
            path.insert(r, ++c); step++; dirty--; q--; floor[r][c].passed = 1; continue;
        }
        if (floor[r - 1][c].distance < floor[r][c].distance && !floor[r - 1][c].wall && !floor[r - 1][c].passed) {
            path.insert(--r, c); step++; dirty--; q--; floor[r][c].passed = 1; continue;
        }
        if (floor[r][c - 1].distance < floor[r][c].distance && !floor[r][c - 1].wall && !floor[r][c - 1].passed) {
            path.insert(r, --c); step++; dirty--; q--; floor[r][c].passed = 1; continue;
        }
        // go back block have passed
        if (floor[r + 1][c].distance < floor[r][c].distance && !floor[r + 1][c].wall) {
            path.insert(++r, c); step++; q--; continue;
        }
        if (floor[r][c + 1].distance < floor[r][c].distance && !floor[r][c + 1].wall) {
            path.insert(r, ++c); step++; q--; continue;
        }
        if (floor[r - 1][c].distance < floor[r][c].distance && !floor[r - 1][c].wall) {
            path.insert(--r, c); step++; q--; continue;
        }
        if (floor[r][c - 1].distance < floor[r][c].distance && !floor[r][c - 1].wall) {
            path.insert(r, --c); step++; q--; continue;
        }        

    }
    home(r, c);
}

void robot::home(int r, int c)
{  
    while (r != charge_r || c != charge_c) {
        // go back block haven't passed 
        if (floor[r + 1][c].distance < floor[r][c].distance && !floor[r + 1][c].wall && !floor[r + 1][c].passed) {
            path.insert(++r, c); step++; dirty--; q--; floor[r][c].passed = 1; continue;
        }
        if (floor[r][c + 1].distance < floor[r][c].distance && !floor[r][c + 1].wall && !floor[r][c + 1].passed) {
            path.insert(r, ++c); step++; dirty--; q--; floor[r][c].passed = 1; continue;
        }
        if (floor[r - 1][c].distance < floor[r][c].distance && !floor[r - 1][c].wall && !floor[r - 1][c].passed) {
            path.insert(--r, c); step++; dirty--; q--; floor[r][c].passed = 1; continue;
        }
        if (floor[r][c - 1].distance < floor[r][c].distance && !floor[r][c - 1].wall && !floor[r][c - 1].passed) {
            path.insert(r, --c); step++; dirty--; q--; floor[r][c].passed = 1; continue;
        }

        // goto block have passed
        if (floor[r + 1][c].distance < floor[r][c].distance && !floor[r + 1][c].wall) {
            path.insert(++r, c); step++; q--; continue;
        }
        if (floor[r][c + 1].distance < floor[r][c].distance && !floor[r][c + 1].wall) {
            path.insert(r, ++c); step++; q--; continue;
        }        
        if (floor[r - 1][c].distance < floor[r][c].distance && !floor[r - 1][c].wall) {
            path.insert(--r, c); step++; q--; continue;
        }
        if (floor[r][c - 1].distance < floor[r][c].distance && !floor[r][c - 1].wall) {
            path.insert(r, --c); step++; q--; continue;
        }        
    }
    q = battery;
}