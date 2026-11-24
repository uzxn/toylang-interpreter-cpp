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
    LeftShift,
    RightShift,
    Set,
    GlobalSet,
    ArraySet,
    ArrayGet,
    ArrayLen,
    If,
    While,
    Break,
    Continue,
    Func,
    Call,
    Return,
};

enum class Flow {
    Normal,
    Break,
    Continue,
    Return,
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
    std::vector<std::string> params = {};  // 仅 Func 使用: 形参名, 按位置绑定
};

struct State {
    std::unordered_map<std::string, ll> global;                       // 全局变量
    std::vector<std::unordered_map<std::string, ll>> frames = {};     // 函数调用帧, 栈顶为当前帧
    std::unordered_map<std::string, std::vector<ll>> array;           // 数组仍为全局单表
    std::unordered_map<std::string, const Statement*> function = {};  // 函数名 -> 定义节点
    std::size_t depth = 0;                                            // 当前调用深度
};

// 解析非负整数, 格式非法或溢出时返回 false
bool toIntNumber(const std::string& s, ll& out);

// 解析 list, 返回节点树的根节点
Statement* parse(const std::vector<std::string>& list, std::size_t& index);

// 加载期校验: 函数定义位置/重名、调用 arity 与定义顺序、return/break/continue 位置
void validate(const std::vector<Statement*>& program);

// 执行 root
Result eval(const Statement* root, State& state);

#endif  // TOYLANG_H
