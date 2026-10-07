#include <bits/stdc++.h>
using namespace std;

enum Type {
    NUM,
    VAR,
    INPUTNUM,
    INPUTCHAR,
    INPUTVIS,
    NOT,
    PRINT,
    PRINTCHAR,
    ADD,
    SUB,
    MUL,
    DIV,
    MOD,
    ADDSET,
    SUBSET,
    MULSET,
    DIVSET,
    MODSET,
    GREATER,
    LESS,
    EQUAL,
    AND,
    OR,
    SET,
    ARRAYSET,
    ARRAYGET,
    ARRAYLEN,
    IF,
    GOTO
};

struct Node {
    Type type;
    int num = 0;
    string name = "";
    vector<Node*> kids = {};
};

// 零元指令
static const unordered_map<string, Type> nullaryIns{
    {"inputNum", INPUTNUM},
    {"inputChar", INPUTCHAR},
    {"inputVis", INPUTVIS},
};

// 一元指令
static const unordered_map<string, Type> unaryIns = {
    {"not", NOT},
    {"print", PRINT},
    {"printChar", PRINTCHAR},
};

// 二元指令
static const unordered_map<string, Type> binaryIns = {
    {"add", ADD},         {"sub", SUB},   {"mul", MUL},     {"div", DIV}, {"mod", MOD},
    {"greater", GREATER}, {"less", LESS}, {"equal", EQUAL}, {"and", AND}, {"or", OR},
};

int toIntNumber(string s) {
    int num = 0;
    for (char c : s) {
        if (!isdigit(c)) break;
        num = num * 10 + c - '0';
    }
    return num;
}

// 解析 list, 返回节点树的根节点, 不进行语法检查
Node* parse(vector<string>& list, size_t& index) {
    string token = list.at(index++);

    // 特殊单节点 (无子节点)
    if (token.size() > 0 && isdigit(token.front())) return new Node{NUM, toIntNumber(token)};
    if (token.size() > 1 && token.front() == '$') return new Node{VAR, 0, token.substr(1)};

    // 零元指令
    if (nullaryIns.count(token)) {
        return new Node{nullaryIns.at(token), 0, "", {}};
    }

    // 一元指令
    if (unaryIns.count(token)) {
        Node* a = parse(list, index);
        return new Node{unaryIns.at(token), 0, "", {a}};
    }

    // 二元指令
    if (binaryIns.count(token)) {
        Node* a = parse(list, index);
        Node* b = parse(list, index);
        return new Node{binaryIns.at(token), 0, "", {a, b}};
    }

    // 其他指令
    if (token == "set") {
        string var = list.at(index++);
        Node* val = parse(list, index);
        return new Node{SET, 0, var.substr(1), {val}};
    }
    if (token == "addSet") {
        string var = list.at(index++);
        Node* b = parse(list, index);
        return new Node{ADDSET, 0, var.substr(1), {b}};
    }
    if (token == "subSet") {
        string var = list.at(index++);
        Node* b = parse(list, index);
        return new Node{SUBSET, 0, var.substr(1), {b}};
    }
    if (token == "mulSet") {
        string var = list.at(index++);
        Node* b = parse(list, index);
        return new Node{MULSET, 0, var.substr(1), {b}};
    }
    if (token == "divSet") {
        string var = list.at(index++);
        Node* b = parse(list, index);
        return new Node{DIVSET, 0, var.substr(1), {b}};
    }
    if (token == "modSet") {
        string var = list.at(index++);
        Node* b = parse(list, index);
        return new Node{MODSET, 0, var.substr(1), {b}};
    }
    if (token == "arraySet") {
        string var = list.at(index++);
        Node* idx = parse(list, index);
        Node* val = parse(list, index);
        return new Node{ARRAYSET, 0, var.substr(1), {idx, val}};
    }
    if (token == "arrayGet") {
        string var = list.at(index++);
        Node* idx = parse(list, index);
        return new Node{ARRAYGET, 0, var.substr(1), {idx}};
    }
    if (token == "arrayLen") {
        string var = list.at(index++);
        return new Node{ARRAYLEN, 0, var.substr(1)};
    }
    if (token == "if") {
        Node* condition = parse(list, index);
        Node* then = parse(list, index);
        return new Node{IF, 0, "", {condition, then}};
    }
    if (token == "goto") {
        string lab = list.at(index++);
        return new Node{GOTO, 0, lab};
    }

    cerr << "invalid token: " << token << endl;
    return nullptr;
}

struct State {
    unordered_map<string, int> variable;
    unordered_map<string, vector<int>> array;
    unordered_map<string, size_t> label;
    size_t next_index = 0;
};

// 执行 root
int eval(Node* root, State& state) {
    if (root == nullptr) {
        cerr << "fail to parse" << endl;
        return 0;
    }

    switch (root->type) {
        case NUM: {
            return root->num;
        }
        case VAR: {
            return state.variable[root->name];
        }
        case INPUTNUM: {
            string s;
            cin >> s;
            return toIntNumber(s);
        }
        case INPUTCHAR: {
            return cin.get();
        }
        case INPUTVIS: {
            char c;
            cin >> c;
            return c;
        }
        case NOT: {
            return eval(root->kids.at(0), state) == 0 ? 1 : 0;
        }
        case PRINT: {
            cout << eval(root->kids.at(0), state);
            return 1;
        }
        case PRINTCHAR: {
            int c = eval(root->kids.at(0), state);
            if (c < 0 || c > 127) {
                cerr << "not a char: " << c << endl;
                return 0;
            }
            cout << (char)c;
            return 1;
        }
        case ADD: {
            return eval(root->kids.at(0), state) + eval(root->kids.at(1), state);
        }
        case SUB: {
            return eval(root->kids.at(0), state) - eval(root->kids.at(1), state);
        }
        case MUL: {
            return eval(root->kids.at(0), state) * eval(root->kids.at(1), state);
        }
        case DIV: {
            int a = eval(root->kids.at(0), state), b = eval(root->kids.at(1), state);
            if (b == 0) {
                cerr << "division by zero" << endl;
                return 0;
            }
            return a / b;
        }
        case MOD: {
            int a = eval(root->kids.at(0), state), b = eval(root->kids.at(1), state);
            if (b == 0) {
                cerr << "division by zero" << endl;
                return 0;
            }
            return a % b;
        }
        case GREATER: {
            return eval(root->kids.at(0), state) > eval(root->kids.at(1), state) ? 1 : 0;
        }
        case LESS: {
            return eval(root->kids.at(0), state) < eval(root->kids.at(1), state) ? 1 : 0;
        }
        case EQUAL: {
            return eval(root->kids.at(0), state) == eval(root->kids.at(1), state) ? 1 : 0;
        }
        case AND: {
            if (eval(root->kids.at(0), state) == 0) return 0;  // 短路
            return eval(root->kids.at(1), state) != 0 ? 1 : 0;
        }
        case OR: {
            if (eval(root->kids.at(0), state) != 0) return 1;  // 短路
            return eval(root->kids.at(1), state) != 0 ? 1 : 0;
        }
        case SET: {
            int val = eval(root->kids.at(0), state);
            state.variable[root->name] = val;
            return 1;
        }
        case ADDSET: {
            int b = eval(root->kids.at(0), state);
            state.variable[root->name] += b;
            return 1;
        }
        case SUBSET: {
            int b = eval(root->kids.at(0), state);
            state.variable[root->name] -= b;
            return 1;
        }
        case MULSET: {
            int b = eval(root->kids.at(0), state);
            state.variable[root->name] *= b;
            return 1;
        }
        case DIVSET: {
            int b = eval(root->kids.at(0), state);
            if (b == 0) {
                cerr << "division by zero" << endl;
                return 0;
            }
            state.variable[root->name] /= b;
            return 1;
        }
        case MODSET: {
            int b = eval(root->kids.at(0), state);
            if (b == 0) {
                cerr << "division by zero" << endl;
                return 0;
            }
            state.variable[root->name] %= b;
            return 1;
        }
        case ARRAYSET: {
            int idx = eval(root->kids.at(0), state);
            if (idx < 0) {
                cerr << "negative index" << endl;
                return 0;
            }
            if (idx >= (int)state.array[root->name].size()) {
                state.array[root->name].resize(idx + 1, 0);
            }
            int val = eval(root->kids.at(1), state);
            state.array[root->name].at(idx) = val;
            return 1;
        }
        case ARRAYGET: {
            int idx = eval(root->kids.at(0), state);
            auto it = state.array.find(root->name);
            if (it == state.array.end()) {
                cerr << "undefined array" << endl;
                return 0;
            }
            if (idx < 0 || idx >= (int)it->second.size()) {
                cerr << "index out of range" << endl;
                return 0;
            }
            return it->second.at(idx);
        }
        case ARRAYLEN: {
            auto it = state.array.find(root->name);
            return it == state.array.end() ? 0 : (int)it->second.size();
        }
        case IF: {
            int condition = eval(root->kids.at(0), state);
            if (condition == 0) return 0;
            return eval(root->kids.at(1), state);
        }
        case GOTO: {
            state.next_index = state.label.at(root->name);
            return 1;
        }
    }

    return 0;
}

int main(int argc, char** argv) {
    if (argc == 1) {
        cerr << "need input file" << endl;
        return 0;
    }

    ifstream fin(argv[1]);
    if (!fin) {
        cerr << "cannot open file: " << argv[1] << endl;
        return 0;
    }

    State state;
    vector<string> list;
    for (string token; fin >> token;) list.push_back(token);

    vector<Node*> program;
    for (size_t index = 0; index < list.size();) {
        string token = list.at(index);
        if (token.back() == ':') {
            string lab = token.substr(0, token.size() - 1);
            state.label[lab] = program.size();  // label 指向下一条语句
            index++;
        } else {
            program.push_back(parse(list, index));
        }
    }

    while (state.next_index < program.size()) {
        size_t cur = state.next_index;
        state.next_index = cur + 1;    // 默认执行下一条
        eval(program.at(cur), state);  // 若里面有 goto, 会覆盖 next_index
    }

    return 0;
}
