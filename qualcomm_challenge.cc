//REQUIREMENT, RAM >= 5 PETABYTES
// DRIVERPIN_0: Name of the driver pin.
// + FIX ( 0 408119 ): Coordinates x and y of the driver pin.
#include <iostream>
#include <cmath>
#include <vector>
#include <fstream>
#include <algorithm>
#include <set>

using namespace std;


// Position in the die
struct Pos {
    int x;
    int y;
};

// Node which represent a pin or a driver
struct node {
    string name;
    node* prev = NULL;
    node* next = NULL;
    bool driver = false;
    bool input  = false;
    //
    node* pdriver = NULL;
    int travelled = 0;
    Pos pos;
    vector<node> not_go;
};

struct discol{
    pair<node, node> in_out;
    int dist;
    int travelled;
};

vector<node> D;     //driver vector
vector<node> N;     //node vector

// Vector of active used nodes
vector<discol> v_active(32);
vector<discol> v_next(32);

node curr;
// Returns the manhattan distance between two nodes
// in = input; out = output
// returns |in[x] - out[x]| + |in[y] - out[y]|
int distance(node in, node out, const int& j){
    node dr = D[j];
    int res = abs(in.pos.x - out.pos.x) + abs(in.pos.y - out.pos.y);
    res += abs(dr.pos.y - out.pos.y);
    return res;
}

int distance(node n, const int& i){
    node aux = D[i];
    return abs(n.pos.x - aux.pos.x) + abs(n.pos.y - aux.pos.y)*2;
}

// Auxiliar function to order for x and then for y in case of colision
bool comp(node a, node b){
    if (a.pos.x == b.pos.x) return a.pos.y < b.pos.y;
    return a.pos.x <= b.pos.x;
}



Pos init_pos(int x, int y){
    Pos p;
    p.x = x;
    p.y = y;
    return p;
}

node init_driver(string n, string put, int x, int y){
    node new_n;
    new_n.name = n;
    if (put == "INPUT") new_n.input = true;
    new_n.driver = true;
    new_n.pos = init_pos(x, y);
    return new_n;   
}

node init_node(string n, int x, int y){
    node new_n;
    new_n.name = n;
    new_n.pos = init_pos(x, y);
    return new_n;
}

bool node_comp(node a, node b, const int& j){
    int d_a = distance(curr, a, j);
    int d_b = distance(curr, b, j);
    return d_a >= d_b;
}

bool is_going(node& prev, const node& next, const int& j) {

    return v_next[j].in_out.first.pos.x == prev.pos.x and v_next[j].in_out.first.pos.y == prev.pos.y and v_next[j].in_out.second.pos.x == next.pos.x;

}

bool is_not_go(const node& n, const node& can_go) {

    for (int i = 0; i < n.not_go.size(); ++i) {

        if (n.not_go[i].pos.x == can_go.pos.x and n.not_go[i].pos.y == can_go.pos.y) return false;
        
    }
    return true;
    
}

bool node_equals(node a, node b){
    return (a.pos.x == b.pos.x and a.pos.y == b.pos.y);
}

void calc(node& start, const int& j) {
    curr = start;
    node min = N[0];
    for (int i = 1; i < N.size(); ++i) {
        if (not(N[i].pos.x == start.pos.x and N[i].pos.y == start.pos.y) and not is_going(start, N[i], j) and is_not_go(start, N[i])) {
            if (not distance(min, j) > distance(N[i], j)) min = N[i];
        }
    }

    v_next[j].in_out.first = start;
    v_next[j].in_out.second = min;
    v_next[j].dist = distance(start, min, j);
}

bool competition() {
    bool found = false;
    cout << "competition entry" << endl;
    for (int i = 0; i < v_next.size(); ++i) {
        for (int j = 0; j < v_next.size(); ++j) {
            if (v_next[i].in_out.second.pos.x == v_next[j].in_out.second.pos.x and v_next[i].in_out.second.pos.y == v_next[j].in_out.second.pos.y and v_next[i].in_out.first.pos.x == v_next[j].in_out.first.pos.x and v_next[i].in_out.first.pos.y == v_next[j].in_out.first.pos.y) {

                found = true;
                if (v_next[i].dist > v_next[j].dist) {

                    v_next[j].in_out.second.not_go.push_back(v_next[j].in_out.second);

                }
                else if (v_next[i].dist < v_next[j].dist) {

                    v_next[i].in_out.second.not_go.push_back(v_next[i].in_out.second);
                    
                }
                else {

                    if (v_next[i].travelled > v_next[j].travelled) {

                        v_next[j].in_out.second.not_go.push_back(v_next[j].in_out.second);

                    }
                    else if (v_next[i].travelled < v_next[j].travelled) {

                        v_next[i].in_out.second.not_go.push_back(v_next[i].in_out.second);

                    }
                    else {

                        if (v_next[i].in_out.second.pos.x < v_next[j].in_out.second.pos.x) {

                            v_next[j].in_out.second.not_go.push_back(v_next[j].in_out.second);

                        }
                        else if (v_next[i].in_out.second.pos.x > v_next[j].in_out.second.pos.x) {

                            v_next[i].in_out.second.not_go.push_back(v_next[i].in_out.second);

                        }
                        else v_next[i].in_out.second.not_go.push_back(v_next[i].in_out.second);

                    }

                }

            }
            cout << v_next[j].in_out.second.not_go.size() << endl;
        }

    }
    if (found) return true;
    return false;

}

void step(){
    for (int i = 0; i < 32; ++i){
        v_next[i].in_out.second.prev = &v_active[i].in_out.first; // un errror que no entenem
        v_active[i].in_out.first.next = &v_next[i].in_out.second; 
        v_next[i].in_out.first.pdriver = v_active[i].in_out.first.pdriver;             
        v_next[i].travelled = v_active[i].travelled + v_next[i].dist;
        while (not v_active[i].in_out.first.not_go.empty()){
            v_active[i].in_out.first.not_go.pop_back();
        }
        v_active[i] = v_next[i];  
        // v_active | v_next

    }
}


void cleanup(){
    for (auto d : v_active){
        bool found = false;
        for (int i = 0; not found and i < N.size(); ++i){
            if (node_equals(N[i], d.in_out.first)) {
                N.erase(N.begin()+i);
                found = true;
            }     
        }
    }  
    
}

void joint() {

    for (int i = 0; i < 16; ++i) {

        v_active[i].in_out.first.next = &v_active[i+16].in_out.first;
        v_active[i+16].in_out.first.prev = &v_active[i].in_out.first;

    }
    
}

void end_them() {

    for (int i = 0; i < N.size(); ++i){
        v_next[i].in_out.second.prev = &v_active[i].in_out.first;
        v_active[i].in_out.first.next = &v_next[i].in_out.second; 
        v_next[i].in_out.first.pdriver = v_active[i].in_out.first.pdriver;             
        v_next[i].travelled = v_active[i].travelled + v_next[i].dist;
        v_active[i] = v_next[i];  
        // v_active | v_next
    }

}
/*
void calc_first(const node& start, const int& j) {
    curr = start;
    vector<node*> closest(32);

    for (int i = 0; i < N.size(); ++i) {

        if (i < 32) {

            closest[i] = &N[i];

        }
        else {

            sort(closest.begin(), closest.end(), &node_comp);
            if (not(N[i].pos.x == start.pos.x and N[i].pos.y == start.pos.y)) {
                if (node_comp(&N[i], closest[closest.size()-1])) {
                    closest[closest.size()-1] = &N[i];
                }
                

            }
        }
    }
    sort(closest.begin(), closest.end(), &node_comp);

    v_next[j] = closest[0];
}
*/

void in_file(){
    cout << "File Name: ";
    string file_name;
    cin >> file_name;
    ifstream f(file_name);
    string l;                                   //string auxiliar para saltar lineas
    // SKIPS 26 LINES
    for (int i = 0; i < 26; ++i) getline(f, l);

    char c, aux;
    f >> c;
    string name, put;
    int x, y;
    // ES LLEGEIXEN ELS PARAMETRES DELS DRIVERS
    while (c == '-'){
        f >> name >> aux >> l >> l >> aux >> l >> put;
        getline(f, l);
        getline(f, l);
        f >> aux >> l >> aux >> x >> y;
        getline(f, l);
        D.push_back(init_driver(name, put, x, y));
        f >> c;
    }

    //Inici nodes
    f >> name;
    name = c + name;
    
    while (true){
        if (f.eof()) break;
        f >> l >> aux >> l >> aux >> x >> y;
        getline(f, l);
        N.push_back(init_node(name, x, y));
        f >> name;
    }

    
    f.close();
}

void print_node(const node& current){
    cout << "- MySQL.illo " << endl;
    cout << "  ( " << current.prev->name << " conn_in )"  << endl;
    cout << "  ( " << current.name      << " conn_out )" << endl; 
}

void print_output(){
    for (int i = 0; i < 32; ++i) {
        node current = D[i];
        if (current.input) {
            current = *current.next;
            while (not current.input and not current.driver){
                print_node(current);
                current = *current.next;
            }
            print_node(current);
        }
    }
}



int main()
{
    in_file();
    sort(N.begin(), N.end(), &comp);
    sort(D.begin(), D.end(), &comp);

    cout << "Hola" << endl;
    // While there are at least 32 nodes we connect them with our algorithm
    while (not N.empty() and N.size() >= D.size()) {

        // Post: We have in v_next tots els possibles candidats
        for (int i = 0; i < D.size(); ++i){
            calc(D[i], i);
        }
        cout << "Hola2" << endl;

        int i = 0;
        while (competition()) {
            cout << "entro, " << i << endl;
            calc(D[i%32], i%32);
            ++i;
            cout << "salgo" << endl;
        }
        cout << "Hola3" << endl;
        step();

        cleanup();

    }

    // Fer iteració final si quedem menys de 32 i mes de 0
    end_them();

    // Unir els finals
    joint();
    
    // Imprimir tot recorrent tots els nodes
    print_output();
}


