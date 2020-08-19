#include <iostream>
#include <vector>
#include <sstream>
#include <fstream>
#include <string>
#include <unistd.h>
#include <signal.h>

using namespace std;
const int MIN_S = 2;
const int MIN_N = 1;
const int MIN_L = 5;

float slope(vector<int> l, bool* bp) {
    *bp = false;
    if (l[0] - l[2] == 0) {
        *bp = true;
        return 0;
    }
    return ((l[1] - l[3]) / (l[0] - l[2]));
}

bool onLine (vector<int> l, int x, int y) {
    bool bp = false;
    float s = slope(l, &bp);
    if (bp) {
        return x == l[0] && ((y - l[1]) * (y - l[3])) <= 0;
    }
    vector<int> temp;
    temp.push_back(x);
    temp.push_back(y);
    temp.push_back(l[0]);
    temp.push_back(l[1]);
    float s2 = slope(temp, &bp);
    if (bp) return false;
    return s == s2 && ((y - l[1]) * (y - l[3])) <= 0 &&
        ((x - l[0]) * (x - l[2])) <= 0;
}

bool overlap(vector<int> l1, vector<int> l2) {
    bool bp1,bp2;
    float s1 = slope(l1, &bp1);
    float s2 = slope(l2, &bp2);
    if (bp1 != bp2 || s1 != s2) return false;

    return onLine(l1, l2[0], l2[1]) ||
        onLine(l1, l2[2], l2[3]) ||
        onLine(l2, l1[0], l1[1]) ||
        onLine(l2, l1[2], l1[3]);
}

bool tryGraph(vector<vector<int> > g, int x, int y) {
    vector<int> street = g[g.size() - 1];
    if (street.size() == 0) return true;
    if (street[0] == x && street[1] == y) return false;
    if (street.size() == 2 && g.size() == 1) return true;

    vector<int> line1;
    line1.push_back(x);
    line1.push_back(y);
    line1.push_back(street[street.size() - 2]);
    line1.push_back(street[street.size() - 1]);

    for (int i = 0; i < g.size(); i++) {
        vector<int> st = g[i];
        for (int j = 0; j <= st.size() - 4; j += 2) {
            vector<int> line2;
            line2.push_back(st[j]);
            line2.push_back(st[j + 1]);
            line2.push_back(st[j + 2]);
            line2.push_back(st[j + 3]);

            if (overlap(line1, line2)) return false;
        }
    }

    return true;
}

int random(int s, int n, int l, int c) {
    ifstream urandom("/dev/urandom");
    if (urandom.fail()) {
        cerr << "Error: cannot open /dev/urandom\n";
        return 1;
    }
    char ch = 'a';
    vector<vector<int> > graph;

    urandom.read(&ch, 1);
    unsigned int sn = (unsigned int)ch % (s - MIN_S + 1) + MIN_S;
    //sn = 2 ;
    //cout << "a \"street0\" (0,-2) (0,2)\n";
    //cout << "a \"street1\" (2,0) (-2,0)\n";
    for (int i = 0; i < sn; i++) {
        string streetName = "street" + to_string(i);
        cout << "a \"" << streetName << "\"";
        vector<int> temp;
        graph.push_back(temp);

        urandom.read(&ch, 1);
        unsigned int lsn = (unsigned int)ch % (n - MIN_N + 1) + MIN_N;
        unsigned int randomPointN = lsn + 1;
        int acc = 0;
        for (int j = 0; j < randomPointN; j++) {
            int x, y;
            urandom.read(&ch, 1);
            x = (unsigned int)ch % (2 * c + 1) - c;
            urandom.read(&ch, 1);
            y = (unsigned int)ch % (2 * c + 1) - c;
            if (acc == 25) {
                cerr << "Error: failed to generate valid input for 25 simultaneous attempts" << endl;
                return 1;
            }

            if (tryGraph(graph, x, y)) {
                temp.push_back(x);
                temp.push_back(y);
            } else {
                j--;
                acc++;
                continue;
            }
            acc = 0;
            cout << " (" << x << "," << y<< ")";
        }
        cout << endl;
    }
    cout << "g" << endl;

    urandom.read(&ch, 1);
    unsigned int ln = (unsigned int)ch % (l - MIN_L + 1) + MIN_L;
    sleep(ln);
    for (int i = 0; i < sn; i++) {
        string streetName = "street" + to_string(i);
        cout << "r \"" << streetName << "\"\n";
    }
    random(s , n, l, c);
}

string formatTransfer () {
    int sw = 0;
    string line;
    // read  `V = {`
    //while (getline(cin, line)) {
        string ans = "";
        int acc = 0;
        while (true) {
            getline(cin, line);
            if (line == "}") break;
            acc ++;
        }
        if (acc < 2) sw = 1;
        //cout << "V " << acc << endl << "E {";
        ans += "V " + to_string(acc) + "\nE {";

        // read `E = {`
        getline(cin, line);
        while (true) {
            getline(cin, line);
            if (line == "}") break;
            //cout << line.substr(2);
            ans += line.substr(2);
        }
        //cout << "}" << endl;
        ans +=  "}\n";

        if (sw == 1) return formatTransfer();
        return ans;
        //cout << ans;
    //}
}

void rgen(char** margv, int margc) {
    char* argv[margc+1];

    argv[0] = "./rgen";
    for (int i = 1; i < margc; i++) {
        argv[i] = margv[i];
    }
    argv[margc] = nullptr;


    execv ("./rgen", argv);
}

void a1() {

    //execv ("/usr/bin/python", argv);
    //execv ("/usr/local/bin/python3", argv);
    execlp("python3", "python3","./ece650-a1.py",NULL);
}

void a2() {
    char* argv[2];

    argv[0] = (char*)"./ece650-a2";
    argv[1] = nullptr;

    execv ("./ece650-a2", argv);
}


// pipe1 read from random write to a1
// pipe2 read from a1 write to format transfer
// pipe3 read from transfer write to a2

void core(char** argv, int argc) {
    vector<pid_t> kids;
    int pipe1[2], pipe2[2], pipe3[2], pipe4[2];
    pipe(pipe1);
    pipe(pipe2);
    pipe(pipe3);
    pipe(pipe4);

    pid_t child_pid;

    // thread random
    child_pid = fork ();
    if (child_pid == 0)
    {
        dup2(pipe1[1], STDOUT_FILENO);
        close(pipe1[0]);
        close(pipe1[1]);     // Close this too!

        rgen(argv,argc);
        return;
    }
    else if (child_pid < 0) {
        cerr << "Error: could not fork\n";
        return;
    }
    kids.push_back(child_pid);

    // thread a1
    child_pid = fork();
    if (child_pid == 0)
    {
        dup2(pipe1[0], STDIN_FILENO);
        close(pipe1[1]);
        close(pipe1[0]);
        dup2(pipe2[1], STDOUT_FILENO);
        close(pipe2[1]);
        close(pipe2[0]);


        a1();
    }
    else if (child_pid < 0) {
        cerr << "Error: could not fork\n";
        return;
    }
    kids.push_back(child_pid);

    // thread format
    child_pid = fork();
    if (child_pid == 0)
    {
        dup2(pipe2[0], STDIN_FILENO);
        close(pipe2[1]);
        close(pipe2[0]);

        //dup2(pipe3[1], STDOUT_FILENO);
        //close(pipe3[1]);
        //close(pipe3[0]);

        //int so = dup(1);
        while (true) {
        string ans = formatTransfer();
        //cout << ans;
        dup2(pipe4[1], STDOUT_FILENO);
        close(pipe4[1]);
        close(pipe4[0]);
        cout << ans;
        //dup2(so, STDOUT_FILENO);
        }
    }
    else if (child_pid < 0) {
        cerr << "Error: could not fork\n";
        return;
    }
    kids.push_back(child_pid);

    // thread a2
    child_pid = fork();
    if (child_pid == 0)
    {
        dup2(pipe4[0], STDIN_FILENO);
        close(pipe4[1]);
        close(pipe4[0]);

        a2();
    }
    else if (child_pid < 0) {
        cerr << "Error: could not fork\n";
        return;
    }
    kids.push_back(child_pid);


    //// Main thread for read from stdin write to pipe3
    //string sline;
    //// read `s 1 6`
    //getline(cin, sline);
    //dup2(pipe3[1], STDIN_FILENO);


    // Main thread
    child_pid = 0;

    int saveStdin = dup(0);
    int saved_stdin = dup(STDIN_FILENO);
    int saveStdout = dup(1);
    //dup2(pipe3[0], STDIN_FILENO);
    //close(pipe3[1]);
    //close(pipe3[0]);

    dup2(pipe4[1], STDOUT_FILENO);
    close(pipe4[1]);
    close(pipe4[0]);

    string abc;
    while (getline(cin, abc)) {
        cout << abc << endl;
    }

    // MAIN PROBLEM
    //string sline, vline, eline;
    //// read from pipe3
    //while (getline(cin, vline)) {
    //    cerr << " here 1 " <<  endl;
    //    getline(cin, eline);
    //    //cerr << " here 2 " << dup(0) <<  endl;

    //    dup2(saved_stdin, STDIN_FILENO);
    //    dup2(saveStdout, STDOUT_FILENO);
    //    close(saved_stdin);
    //    close(saveStdout);
    //    dup(0);
    //    //cerr << " here 3 " << dup(0) << " " << saved_stdin<< endl;
    //    // print V E information
    //    cout << vline << endl << eline << endl;

    //    cerr << " here 4" << endl;
    //    getline(cin, sline);
    //    cerr << " here 5" << endl;

    //    dup2(pipe4[1], STDOUT_FILENO);
    //    close(pipe4[0]);
    //    close(pipe4[1]);
    //    //cerr << " here 6 " << dup(0) <<  endl;
    //    cout << vline << endl << eline << endl << sline << endl;
    //    //cerr << " here 7" << endl;

    //    dup(1);
    //    dup2(pipe3[0], STDIN_FILENO);
    //    close(pipe3[1]);
    //    close(pipe3[0]);
    //    dup(0);
    //    //cerr << " here 8 " << dup(0) << endl;
    //    //
    //}

    // send kill signal to all children
    for (pid_t k : kids) {
        int status;
        kill (k, SIGTERM);
        waitpid(k, &status, 0);
    }

    return;
}

int main (int argc, char **argv) {
    int s, n, l, c;
    s = 10;
    n = 5;
    l = 5;
    c = 20;

    for (int i = 1; i < argc; i += 2) {
        stringstream ss(argv[i+1]);
        int temp;
        // cin is istream
        // istream myin = cin;
        ss >> temp;
        //ss.operator>>(temp);

        switch(argv[i][1]) {
            case 's':
                s = temp;
                continue;
            case 'n':
                n = temp;
                continue;
            case 'l':
                l = temp;
                continue;
            case 'c':
                c = temp;
                continue;
        }
    }

    //a1();
    core(argv, argc);
    //random(s,n,l,c);

    return 0;
}