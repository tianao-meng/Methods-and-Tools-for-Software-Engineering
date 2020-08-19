// defined std::unique_ptr
#include <memory>
// defines Var and Lit
#include "minisat/minisat/core/SolverTypes.h"
// defines Solver
#include "minisat/minisat/core/Solver.h"
//#include "minisat/minisat/mtl/Vec.h"

#include <iostream>
#include <sstream>
#include <vector>
#include <algorithm>
using namespace std;

// defined std::cout
#include <iostream>

int core(vector<int> E, int n, int k) {
    unique_ptr<Minisat::Solver> solver(new Minisat::Solver());
    vector<vector<Minisat::Lit> > x;
    for (int i = 0; i < n; i++) {
        vector<Minisat::Lit> temp;
        for (int j = 0; j < k; j++) {
            Minisat::Lit l;
            l = Minisat::mkLit(solver->newVar());
            temp.push_back(l);
        }
        x.push_back(temp);
    }

    // 1
    for (int j = 0; j < k; j++)  {
        Minisat::vec<Minisat::Lit> v;
        for (int i = 0; i < n; i++) {
            v.push(x[i][j]);
        }
        solver->addClause(v);
    }

    // 2
    for (int i = 0; i < n; i++) {
        for (int p = 0; p < k; p++) {
            for (int q = 0; q < k; q++) {
                if (p < q) {
                    solver->addClause(~x[i][p], ~x[i][q]);
                }
            }
        }
    }

    // 3
    for (int j = 0; j < k; j++) {
        for (int p = 0; p < n; p++) {
            for (int q = 0; q < n; q++) {
                if (p < q) {
                    solver->addClause(~x[p][j], ~x[q][j]);
                }
            }
        }
    }

    // 4
    for (int e = 0; e < E.size(); e += 2) {
        int i = E[e];
        int j = E[e+1];
        Minisat::vec<Minisat::Lit> v;
        for (int m = 0; m < k; m++) {
            v.push(x[i][m]);
            v.push(x[j][m]);
        }
        solver->addClause(v);
    }

    bool res = solver->solve();
    if (!res) {
        solver.reset (new Minisat::Solver());
        return 0;
    }

    vector<int> ans;
    for (int j = 0; j < k; j++) {
        for (int i = 0; i < n; i++) {
            if (Minisat::toInt(solver->modelValue(x[i][j])) == 0) {
                ans.push_back(i);
                continue;
            }
        }
    }
    sort(ans.begin(), ans.end());

    cout << ans[0];
    for (int i = 1; i < ans.size(); i++) {
        cout << " " << ans[i];
    }
    cout << endl;

    solver.reset (new Minisat::Solver());
    return 1;
}

void run(vector<int> p, int n) {
    for (int k = 1; k < n; k++) {
        int ans = core(p, n, k);
        if (ans == 1) return;
    }
}


void buildMatrix(vector<int>* p, string line, int n) {
    int v1, v2;
    string str = line;

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
        }

        (*p).push_back(v1);
        (*p).push_back(v2);
    }
}

int main(int argc, char** argv) {
    string line;

    while (true) {
        vector<int> m;
        int n;
        if (! getline(cin, line)) {
            break;
        }
        if (line.substr(0,1) == "V") {

            istringstream input(line);
            string temp;
            input >> temp >> n;

            continue;
        }
        if (line.substr(0,1) == "E") {
            istringstream ss(line);
            string temp;
            ss >> temp >> line;
            buildMatrix(&m, line, n);
            run(m, n);
            continue;
        }
    }
    return 0;
}
