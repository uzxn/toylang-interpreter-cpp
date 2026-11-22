#ifndef TOYLANG_H
#define TOYLANG_H

#include <cstddef>
#include <string>
#include <unordered_map>
#include <vector>

using ll = long long;

enum class Type {
    Block,
    Num,
    Var,
    InputNum,
    InputChar,
    InputVis,
    Not,
    Print,
    PrintChar,
    Add,
    Sub,
    Mul,
    Div,
    Mod,
    Greater,
    Less,
    Equal,
    And,
    Or,
    BitAnd,
    BitOr,
    BitXor,
    BitNot,
    Shl,
    Shr,
    Set,
    ArraySet,
    ArrayGet,
    ArrayLen,
    If,
    While,
    Break,
    Continue,
};

enum class Flow {
    Normal,
    Break,
    Continue,
};

struct Result {
    ll value = 0;
    Flow flow = Flow::Normal;
};

struct Statement {
    Type type;
    ll num = 0;
    std::string name = "";
    std::vector<Statement*> kids = {};
};

struct State {
    std::unordered_map<std::string, ll> variable;
    std::unordered_map<std::string, std::vector<ll>> array;
};

// 解析非负整数, 格式非法或溢出时返回 false
bool toIntNumber(const std::string& s, ll& out);

// 解析 list, 返回节点树的根节点
Statement* parse(const std::vector<std::string>& list, std::size_t& index);

// 执行 root
Result eval(const Statement* root, State& state);

#endif  // TOYLANG_H
