// Compile with c++ ece650-a2cpp -std=c++11 -o ece650-a2
#include <iostream>
#include <sstream>
#include <vector>
#include <algorithm>
using namespace std;


struct Node {
    int val;
    int l;
    vector<Node*> children;

    Node(int v) {
        val = v;
    }
};

vector<int> vecCopy(vector<int> v) {
    int n = v.size();
    vector<int> init;

    for (int i = 0; i < n; ++i) {
        init.push_back(v[i]);
    }
    return init;
}

Node* createNode(int val, vector<vector<int> >* p, vector<int> visited) {
    vector<vector<int> > m = *p;
    Node* n = new Node(val);

    int len = m[val].size();
    visited.push_back(val);
    for (int i = 0; i < len; ++i) {
        if (find(visited.begin(), visited.end(), m[val][i]) == visited.end() ) {
            Node* c = createNode(m[val][i], p, visited );
            n->children.push_back(c);
        }
    }
    return n;
}

void printTree(Node* n) {
    cout << n->val << ":";

    for (int i = 0; i < n->children.size(); ++i) {
        cout << " " << n->children[i]->val ;
    }
    cout << endl;

    for (int i = 0; i < n->children.size(); ++i) {
        printTree(n->children[i]);
    }
}
int bfs(Node* n, int to) {
    if (n->val == to) {
        n->l = 0;
        return 0;
    }
    int len = n->children.size();
    int minSoFar = -1;

    for (int i =0;i < len; ++i) {
        int v = bfs(n->children[i], to);
        if (v != -1  && (minSoFar >= v || minSoFar == -1)) {
            minSoFar = v;
        }
    }
    n->l = minSoFar + 1;
    return minSoFar + 1;
}

string getPath(Node* n) {
    string str;
    stringstream ss;
    ss << "-" << n->val;
    ss >> str;
    int len = n->children.size();
    if (n->l == 0) {
        return str;
    }
    for (int i = 0; i < len; i++) {
        if (n->children[i]->l + 1 == n->l ) {
            return str + getPath(n->children[i]);
        }
    }
}

void showPath(vector<vector<int> >* p, int from, int to) {
    vector<vector<int> > m = *p;
    int n = m.size();
    vector<int> visited;
    Node* tree = createNode(from, p, visited);

    //printTree(tree);
    bfs(tree, to);
    if (tree->l == -1) {
        cerr << "Error: cannot find the path" << endl;
        exit(1);
    }

    string test = getPath(tree);
    cout << test.substr(1) << endl;
}

void buildMatrix(vector<vector<int> >* p, string line) {
    int v1, v2;
    string str = line;
    int n = p->size();

    while (str.size() > 3) {
        str = str.substr(2);
        stringstream ss1(str);
        ss1 >> v1 >> str;
        str = str.substr(1);
        stringstream ss2(str);
        ss2 >> v2 >> str;
        str = str.substr(1);

        if (v1 >= n || v2 >= n || v1 == v2) {
            cerr << "Error: input edge wrong" << endl;
            exit(1);
        }

        (*p)[v1].push_back(v2);
        (*p)[v2].push_back(v1);
    }
}

int main(int argc, char** argv) {
    string line;

    while (getline(cin, line)) {
        // 1, get V  init matrix

        istringstream input(line);
        string temp;
        int n;
        input >> temp >> n;

        // matrix init
        vector<vector<int> > m;
        for (int i = 0; i < n; i++) {
            vector<int> v;
            m.push_back(v);
        }

        // 2, get E  build matrix
        getline(cin, line);
        istringstream ss(line);
        ss >> temp >> line;
        buildMatrix(&m, line);

        // 3, get search from and target
        getline(cin, line);
        istringstream ss2(line);
        int from, to;
        ss2 >> temp >> from >> to;
        //
        // 4, find the shortest path
        showPath(&m, from, to);
    }
    return 0;
}
