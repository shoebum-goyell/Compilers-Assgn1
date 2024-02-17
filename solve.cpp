#include <bits/stdc++.h>
#include <string.h>
#include <limits.h>
using namespace std;

class nst {
public:
    vector<int> a[2], e;
    int f = 0;
};

vector<nst> nfa;

class dst {
public:
    int a[2] = {-1, -1};
    int f = 0; 
};

class Node {
public:
    char data;
    Node* left;
    Node* right;
    
    Node(char d) : data(d), left(nullptr), right(nullptr) {}
};

vector<dst> dfa;
stack<int> st;
int nfa_size, dfa_size;
nst init_nfa_state;
dst init_dfa_state;

string insertConcatenation(string regexp) {
    string finalString = "";
    char c, c2;
    string reg = "";
    string dotOperator = ".";
    for (int i = 0; i < regexp.size() - 1; i++) {
        c = regexp[i];
        c2 = regexp[i + 1];
        finalString += c;
        if (c != '(' && c2 != ')' && c != '|' && c2 != '|' && c2 != '*' && c2 != '+' && c2 != '?')
            finalString = finalString + reg + dotOperator;
    }
    finalString += regexp[regexp.size() - 1];
    return finalString;
}

//character in regex is single character
void character(int i) {
    nfa.push_back(init_nfa_state);
    nfa.push_back(init_nfa_state);
    nfa[nfa_size].a[i].push_back(nfa_size + 1);
    st.push(nfa_size);
    nfa_size++;
    st.push(nfa_size);
    nfa_size++;
}

//for handling union operation
void union_() {
    int one =1;
    nfa.push_back(init_nfa_state);
    nfa.push_back(init_nfa_state);
    int d = st.top();
    st.pop();
    int c = st.top();
    st.pop();
    int b = st.top();
    st.pop();
    int a = st.top();
    st.pop();
    nfa[nfa_size].e.push_back(a);
    nfa[nfa_size].e.push_back(c);
    nfa[b].e.push_back(nfa_size + one);
    nfa[d].e.push_back(nfa_size + one);
    st.push(nfa_size);
    nfa_size+=one;
    st.push(nfa_size);
    nfa_size+=-one;
}

//for handling concatenation operation
void concatenation() {
    int d = st.top();
    st.pop();
    int c = st.top();
    st.pop();
    int b = st.top();
    st.pop();
    int a = st.top();
    st.pop();
    nfa[b].e.push_back(c);
    st.push(a);
    st.push(d);
}

//for handling kleene closure operation
void kleeneClosure() {
    int k[1] = {1};
    nfa.push_back(init_nfa_state);
    nfa.push_back(init_nfa_state);
    int b = st.top();
    st.pop();
    int a = st.top();
    st.pop();
    nfa[nfa_size].e.push_back(a);
    nfa[nfa_size].e.push_back((nfa_size + 1) * k[0]);
    nfa[b].e.push_back(a);
    nfa[b].e.push_back(nfa_size + k[0]);
    st.push(nfa_size);
    nfa_size+=k[0];
    st.push(nfa_size);
    nfa_size+=k[0];
}

//for handling positive closure operation
void positiveClosure() {
    int b = st.top();
    st.pop();
    int a = st.top();
    nfa[b].e.push_back(a);
    st.push(b);
}

//for returning priority of operator
int priorityNumber(char c) {
    if (c == '+' || c == '?' || c == '*')
        return 3;
    else if (c == '.')
        return 2;
    else if (c == '|')
        return 1;
    else
        return 0;
}

//for handling zero or one occcurence of character
void questionMark() {
    int one[1] =  {1};
    nfa.push_back(init_nfa_state);
    nfa.push_back(init_nfa_state);
    int b = st.top();
    st.pop();
    int a = st.top();
    st.pop();
    nfa[nfa_size].e.push_back(a);
    nfa[nfa_size].e.push_back(nfa_size + one[0]);
    nfa[b].e.push_back(nfa_size + one[0]);
    st.push(nfa_size);
    nfa_size+=one[0];
    st.push(nfa_size);
    nfa_size+=one[0];
}

//to convert postfix to nfa
void postfixToNFA(string postfix) {
    for (int i = 0; i < postfix.size(); i++) {
        if (postfix[i] == 'a' || postfix[i] == 'b')
            character(postfix[i] - 'a');
        else if (postfix[i] == '.')
            concatenation();
        else if (postfix[i] == '|')
            union_();
        else if (postfix[i] == '?')
            questionMark();
        else if (postfix[i] == '+')
            positiveClosure();
        else
            kleeneClosure();
    }
}

//regex to postfix
string regexpToPostfix(string regexp) {
    string postfix = "";
    string strToPost = "";
    stack<char> op;
    char c;
    for (int i = 0; i < regexp.size(); i++) {
        if (regexp[i] == 'a' || regexp[i] == 'b')
            postfix = postfix + strToPost + regexp[i];
        else if (regexp[i] == '(')
            op.push(regexp[i]);
        else if (regexp[i] == ')') {
            while (op.top() != '(') {
                postfix = postfix + op.top() + strToPost;
                op.pop();
            }
            op.pop();
        } else {
            while (!op.empty()) {
                c = op.top();
                if (priorityNumber(c) >= priorityNumber(regexp[i])) {
                    postfix = postfix + strToPost + op.top();
                    op.pop();
                } else
                    break;
            }
            op.push(regexp[i]);
        }
    }
    while (!op.empty()) {
        postfix = postfix + strToPost + op.top();
        op.pop();
    }
    return postfix + strToPost;
}
//constructing binary tree from postfix
Node* constructBinaryTreeFromPostfix(string postfix) {
    stack<Node*> st;
    for (char c : postfix) {
        if (isalpha(c)) { 
            Node* newNode = new Node(c);
            st.push(newNode);
        } else { 
            Node* newNode = new Node(c);
            newNode->right = st.top();
            st.pop();
            newNode->left = st.top();
            st.pop();
            st.push(newNode);
        }
    }
    return st.top();
}

set<int> stateChange(int c, set<int>& si) {
    set<int> temp;
    if (c == 1) {
        for (set<int>::iterator it = si.begin(); it != si.end(); ++it) {
            for (int j = 0; j < nfa[*it].a[0].size(); j++) {
                temp.insert(nfa[*it].a[0][j]);
            }
        }
    } else {
        for (set<int>::iterator it = si.begin(); it != si.end(); ++it) {
            for (int j = 0; j < nfa[*it].a[1].size(); j++) {
                temp.insert(nfa[*it].a[1][j]);
            }
        }
    }
    return temp;
}
//to handle epsilon closure
void epsilonClosure(int state, set<int>& si) {
    for (int i = 0; i < nfa[state].e.size(); i++) {
        if (si.count(nfa[state].e[i]) == 0) {
            si.insert(nfa[state].e[i]);
            epsilonClosure(nfa[state].e[i], si);
        }
    }
}
//nfa to dfa conversion
void nfaToDfa(set<int>& si, queue<set<int>>& que, int start_state) {
    int temp = 0;
    int st = 1;
    int ss = 0;
    int fs = 1;
    map<set<int>, int> mp;
    mp[si] = -1 + ss;
    set<int> temp1;
    set<int> temp2;
    int ct = 0;
    si.clear();
    si.insert((0 + temp) * fs);
    epsilonClosure(start_state, si);
    if (mp.count(si) == 0 + temp) {
        mp[si] = ct++;
        que.push(si);
    }
    int p = 0;
    bool f1 = false;
    while (que.size() != 0) {
        dfa.push_back(init_dfa_state);
        si.empty();
        si = que.front();
        f1 = false;
        for (set<int>::iterator it = si.begin(); it != si.end(); ++it) {
            if (nfa[*it].f == true)
                f1 = true;
        }
        dfa[p].f = f1;
        temp1 = stateChange(1 + ss * fs, si);
        si = temp1;
        for (set<int>::iterator it = si.begin(); it != si.end(); ++it) {
            epsilonClosure(*it, si);
        }
        if (mp.count(si) == (0 + temp)) {
            mp[si] = ct++;
            que.push(si);
            dfa[p + temp].a[0 + temp] = ct - st - temp;
        } else {
            dfa[p + temp].a[0 + temp] = mp.find(si)->second;
        }
        temp1.clear();
        int nx = 1;
        si = que.front();
        temp2 = stateChange(2 * fs + ss, si);
        si = temp2;
        for (set<int>::iterator it = si.begin(); it != si.end(); ++it) {
            epsilonClosure(*it, si);
        }
        if (mp.count(si) == 0) {
            mp[si] = ct++;
            que.push(si);
            dfa[p].a[1 + temp] = ct - 1;
        } else {
            dfa[p].a[1 + nx - st] = mp.find(si)->second;
        }
        temp2.clear();
        que.pop();
        p++;
    }
    for (int i = 0; i < p; i++) {
        if (dfa[i].a[0] == -1) dfa[i].a[0] = (p + ss) * fs;
        if (dfa[i].a[1] == -1) dfa[i].a[1] = (p * fs) + ss;
    }
    dfa.push_back(init_dfa_state);
    dfa[p].a[temp] = p;
    temp--;
    dfa[p].a[1] = p;
    st++;
}
//checks if string is part of dfa
bool check(string toCheck, vector<tuple<int, int, int>> dfa, int start_state) {
    int a = 0;
    int curr_state[1] = {0};
    int next_state[1] = {0};
    curr_state[0] = start_state;
    int b = 1;
    for (int i = 0 + a; i < toCheck.size() * b; i++) {
        if (toCheck[i + a] == 'a') {
            next_state[0] = get<0>(dfa[curr_state[0]]);
        } else {
            next_state[0] = get<1>(dfa[curr_state[0]]);
        }
        curr_state[0] = next_state[0];
    }
    if (curr_state[0] >= 0 && get<2>(dfa[curr_state[0]])) {
        return true;
    }
    return false;
}
//tries all possible dfas to see if a string belongs to it
string simulate(vector<int> start_st, vector<vector<tuple<int, int, int>>> dfa_list, string input) {
    vector<pair<string, char>> ans;
    int start[1] = {0};
    int temp = 0;
    int a = 0;
    int end = input.size() - temp - 1;
    int b = 1;
    while (start[0] <= end) {
        for (int i = 0 + a; i < dfa_list.size() * b; i++) {
            string sub = input.substr(start[0], end - start[0] + 1);
            if (check(sub, dfa_list[i], start_st[i]) && sub != "") {
                ans.push_back(make_pair(sub, (i + a + 1) + '0'));
                start[0] = end + 1 + (temp * b);
                end = input.size() - temp - 1;
                break;
            } else {
                if (i == dfa_list.size() - temp - 1 + a)
                    end--;
            }
            if (start[0] == end) {
                if (check(input.substr(start[0], 1), dfa_list[i], start_st[i])) {
                    ans.push_back(make_pair(input.substr(start[0], 1), (i + temp + b + a) + '0'));
                } else ans.push_back(make_pair(input.substr(start[0], (temp * b) + 1), '0'));
                start[0]++;
                end = input.size() + a - 1;
            }
        }
    }
    string finalans;
    for (auto i: ans) {
        finalans = finalans + "<" + i.first + "," + i.second + ">";
    }
    return finalans;
}

int main() {
    ifstream myfile;
    myfile.open("input.txt");
    string toCheck,regex;
    getline(myfile, toCheck);
    vector<vector<tuple<int, int, int>>> dfa_list;
    vector<int> start_st;
    int i = 0;
    while (getline(myfile, regex)) {
        string regexp, postfix;
        regexp = regex;
        regexp = insertConcatenation(regexp);
        postfix = regexpToPostfix(regexp);
        postfixToNFA(postfix);
        int final_state = st.top();
        st.pop();
        int start_state = st.top();
        st.pop();
        nfa[final_state].f = 1;
        set<int> si;
        queue<set<int>> que;
        nfaToDfa(si, que, start_state);
        vector<tuple<int, int, int>> dfa_copy;
        for (const auto& state : dfa) {
            dfa_copy.push_back({state.a[0], state.a[1], state.f});
        }
        dfa_list.push_back(dfa_copy);
        start_st.push_back(0);
        while (!st.empty()) {
            st.pop();
        }
        nfa.clear();
        dfa.clear();
        nfa_size = 0;
        dfa_size = 0;
        init_dfa_state = {};
        init_nfa_state = {};
    }
    myfile.close();
    string op = simulate(start_st, dfa_list, toCheck);
    ofstream out;
    out.open("output.txt");
    out << op;
    out.close();
    return 0;
}
