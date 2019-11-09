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
        void roam();
        void home();
    private:
        node** floor;
        chain path;
        int row;
        int col;
        int battery;
        int charge_r, charge_c;
        int max_d;
        int dirty;
        int step;
};

int main() 
{
    int row, col, battery;
    cin >> row >> col >> battery;

    robot c(row, col, battery);
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

void chain::print()
{
    for (Step *i = first; i ; i = i->next)
        printf("%d %d\n", i->row, i->col);
}

// robot functions
robot::robot(int r, int c, int b)
{
    row = r; col = c; battery = b; max_d = dirty = step = 0;
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
    printf("\n%d %d %d %d %d %d\n", row, col, battery, max_d, dirty, step);
    for (int i = 0; i < row; i++) {
        for (int j = 0; j < col; j++) { 
            floor[i][j].print();
        }  
        cout << endl;
    } 
    path.print();
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
    if(c == col || c == 0 || r == row || r == 0 || floor[r][c].wall || floor[r][c].distance < d && floor[r][c].distance != -1)
        return;    
    floor[r][c].distance = d;
    if (d > max_d) max_d = d;
    dirty++;
    setdistance(r + 1, c, d + 1);
    setdistance(r - 1, c, d + 1);
    setdistance(r, c + 1, d + 1);
    setdistance(r, c - 1, d + 1);
}

void robot::run()
{
    int k = max_d;
    for (int k = max_d; k > 0; k--){
        for (int i = 0; i < row; i++) {
            for (int j = 0; j < col; j++) {
                if (floor[i][j].distance == max_d && !floor[i][j].passed)
                    expedition(i, j);
                if (dirty == 0) return;
            }
        }
    }
}

void robot::expedition(int r, int c)
{
    chain exped;
    exped.insert(r ,c);
    floor[r][c].passed = 1;

    while (r != charge_r || c != charge_c) {
        // goto block haven't passed 
        if (floor[r + 1][c].distance < floor[r][c].distance &&
            !floor[r + 1][c].wall && !floor[r + 1][c].passed) {
            exped.insert(++r, c); step++; dirty--; floor[r][c].passed = 1; continue;
        }
        if (floor[r][c + 1].distance < floor[r][c].distance &&
            !floor[r][c + 1].wall && !floor[r][c + 1].passed) {
            exped.insert(r, ++c); step++; dirty--; floor[r][c].passed = 1; continue;
        }
        if (floor[r][c - 1].distance < floor[r][c].distance &&
            !floor[r][c - 1].wall && !floor[r][c - 1].passed) {
            exped.insert(r, --c); step++; dirty--; floor[r][c].passed = 1; continue;
        }
        if (floor[r - 1][c].distance < floor[r][c].distance &&
            !floor[r - 1][c].wall && !floor[r - 1][c].passed) {
            exped.insert(--r, c); step++; dirty--; floor[r][c].passed = 1; continue;
        }
        // goto block have passed
        if (floor[r + 1][c].distance < floor[r][c].distance &&
            !floor[r + 1][c].wall) {
            exped.insert(++r, c); step++; continue;
        }
        if (floor[r][c + 1].distance < floor[r][c].distance && !floor[r][c + 1].wall) {
            exped.insert(r, ++c); step++; continue;
        }
        if (floor[r][c - 1].distance < floor[r][c].distance && !floor[r][c - 1].wall) {
            exped.insert(r, --c); step++;continue;
        }        
        if (floor[r - 1][c].distance < floor[r][c].distance && !floor[r - 1][c].wall) {
            exped.insert(--r, c); step++; continue;
        }
    }
    exped.reverse();
    path.add(exped);
}