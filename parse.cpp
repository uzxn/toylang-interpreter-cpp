#include <cctype>
#include <cstdlib>
#include <iostream>
#include <limits>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#include "toylang.h"

namespace {

// 指令元数据: 类型、语句参数个数、是否需要变量名参数
struct InsInfo {
    Type type;
    std::size_t arity;
    bool varArg;
};

// 指令表, 新增指令只需在枚举、本表和 eval 中登记
const std::unordered_map<std::string, InsInfo> instructions = {
    {"in", {Type::InputNum, 0, false}},
    {"cin", {Type::InputChar, 0, false}},
    {"vin", {Type::InputVis, 0, false}},
    {"break", {Type::Break, 0, false}},
    {"continue", {Type::Continue, 0, false}},

    {"not", {Type::Not, 1, false}},
    {"out", {Type::Print, 1, false}},
    {"cout", {Type::PrintChar, 1, false}},

    {"add", {Type::Add, 2, false}},
    {"sub", {Type::Sub, 2, false}},
    {"mul", {Type::Mul, 2, false}},
    {"div", {Type::Div, 2, false}},
    {"mod", {Type::Mod, 2, false}},
    {"gre", {Type::Greater, 2, false}},
    {"les", {Type::Less, 2, false}},
    {"eq", {Type::Equal, 2, false}},
    {"and", {Type::And, 2, false}},
    {"or", {Type::Or, 2, false}},

    {"&", {Type::BitAnd, 2, false}},
    {"|", {Type::BitOr, 2, false}},
    {"^", {Type::BitXor, 2, false}},
    {"~", {Type::BitNot, 1, false}},
    {"<<", {Type::LeftShift, 2, false}},
    {">>", {Type::RightShift, 2, false}},

    {"set", {Type::Set, 1, true}},
    {"gset", {Type::GlobalSet, 1, true}},
    {"aset", {Type::ArraySet, 2, true}},
    {"aget", {Type::ArrayGet, 1, true}},
    {"alen", {Type::ArrayLen, 0, true}},

    {"while", {Type::While, 2, false}},
};

[[noreturn]] void parseError(const std::string& msg) {
    std::cerr << msg << std::endl;
    std::exit(1);
}

// 读取并校验变量名参数, 要求以 $ 开头
std::string parseVarName(const std::vector<std::string>& list, std::size_t& index, const std::string& ins) {
    if (index >= list.size()) parseError(ins + ": missing variable name");
    std::string token = list.at(index++);
    if (token.size() < 2 || token.front() != '$') {
        parseError(ins + ": expected variable name, got " + token);
    }
    return token.substr(1);
}

}  // namespace

// 解析非负整数, 格式非法或溢出时返回 false
bool toIntNumber(const std::string& s, ll& out) {
    if (s.empty()) return false;
    constexpr ll max = std::numeric_limits<ll>::max();
    ll num = 0;
    for (char c : s) {
        if (!std::isdigit(static_cast<unsigned char>(c))) return false;
        ll digit = c - '0';
        if (num > (max - digit) / 10) return false;  // 溢出
        num = num * 10 + digit;
    }
    out = num;
    return true;
}

// 解析 list, 返回节点树的根节点
Statement* parse(const std::vector<std::string>& list, std::size_t& index) {
    if (index >= list.size()) parseError("unexpected end of program: missing argument");
    std::string token = list.at(index);
    index++;

    // 代码块
    if (token == "{") {
        Statement* block = new Statement{Type::Block};
        while (index < list.size() && list.at(index) != "}") {
            block->kids.push_back(parse(list, index));
        }
        index++;
        return block;
    }

    // 数字和变量名
    if (token.size() > 0 && std::isdigit(static_cast<unsigned char>(token.front()))) {
        ll num = 0;
        if (!toIntNumber(token, num)) parseError("invalid number: " + token);
        return new Statement{Type::Num, num};
    }

    if (token.size() > 1 && token.front() == '$') {
        return new Statement{Type::Var, 0, token.substr(1)};
    }

    // 函数定义: func $name [ $p1 $p2 ... ] { body }
    if (token == "func") {
        std::string name = parseVarName(list, index, token);
        if (index >= list.size() || list.at(index) != "[") parseError("func: expected '['");
        index++;
        std::vector<std::string> params;
        std::unordered_set<std::string> seen;
        while (index < list.size() && list.at(index) != "]") {
            std::string param = list.at(index++);
            if (param.size() < 2 || param.front() != '$') {
                parseError("func: expected parameter name, got " + param);
            }
            param = param.substr(1);
            if (!seen.insert(param).second) parseError("func: duplicate parameter $" + param);
            params.push_back(param);
        }
        if (index >= list.size()) parseError("func: missing ']'");
        index++;
        Statement* body = parse(list, index);
        if (body->type != Type::Block) parseError("func: body must be a block");
        return new Statement{Type::Func, 0, name, std::vector<Statement*>{body}, params};
    }

    // 函数调用: call $name [ arg1 arg2 ... ]
    if (token == "call") {
        std::string name = parseVarName(list, index, token);
        if (index >= list.size() || list.at(index) != "[") parseError("call: expected '['");
        index++;
        std::vector<Statement*> args;
        while (index < list.size() && list.at(index) != "]") args.push_back(parse(list, index));
        if (index >= list.size()) parseError("call: missing ']'");
        index++;
        return new Statement{Type::Call, 0, name, args};
    }

    // 返回: return expr
    if (token == "return") {
        Statement* value = parse(list, index);
        return new Statement{Type::Return, 0, "", std::vector<Statement*>{value}};
    }

    // 条件分支: if cond body [else body]
    if (token == "if") {
        Statement* cond = parse(list, index);
        Statement* thenBody = parse(list, index);
        std::vector<Statement*> kids{cond, thenBody};
        if (index < list.size() && list.at(index) == "else") {
            index++;
            if (index >= list.size()) parseError("else: missing statement");
            kids.push_back(parse(list, index));
        }
        return new Statement{Type::If, 0, "", kids};
    }

    if (token == "else") parseError("else: without matching if");

    // 指令: 按元数据读取变量名参数和语句参数
    auto ins = instructions.find(token);
    if (ins != instructions.end()) {
        const InsInfo& info = ins->second;
        std::string name;
        if (info.varArg) name = parseVarName(list, index, token);
        std::vector<Statement*> kids;
        kids.reserve(info.arity);
        for (std::size_t i = 0; i < info.arity; ++i) kids.push_back(parse(list, index));
        return new Statement{info.type, 0, name, kids};
    }

    parseError("invalid token: " + token);
}

namespace {

struct FuncInfo {
    std::size_t arity;
    std::size_t order;
};

// inFunction: 是否位于函数体内; inLoop: 是否位于 while 内; inArg: 是否位于参数位置
void validateStatement(const Statement* root, const std::unordered_map<std::string, FuncInfo>& functions,
                       bool inFunction, bool inLoop, bool inArg, std::size_t order) {
    switch (root->type) {
        case Type::Func: {
            parseError("func: function definition must be at top level");
            break;
        }

        case Type::Call: {
            auto it = functions.find(root->name);
            if (it == functions.end()) parseError("call: undefined function $" + root->name);
            if (root->kids.size() != it->second.arity) {
                parseError("call: arity mismatch for $" + root->name);
            }
            if (!inFunction && it->second.order > order) {
                parseError("call: function $" + root->name + " used before its definition");
            }
            for (const Statement* s : root->kids) {
                validateStatement(s, functions, inFunction, inLoop, true, order);
            }
            break;
        }

        case Type::Return: {
            if (inArg) parseError("return: not allowed in argument position");
            if (!inFunction) parseError("return: outside function");
            for (const Statement* s : root->kids) {
                validateStatement(s, functions, inFunction, inLoop, true, order);
            }
            break;
        }

        case Type::Break:
        case Type::Continue: {
            const std::string ins = root->type == Type::Break ? "break" : "continue";
            if (inArg) parseError(ins + ": not allowed in argument position");
            if (!inLoop) parseError(ins + ": outside loop");
            break;
        }

        case Type::Block: {
            for (const Statement* s : root->kids) {
                validateStatement(s, functions, inFunction, inLoop, inArg, order);
            }
            break;
        }

        case Type::If: {
            validateStatement(root->kids.at(0), functions, inFunction, inLoop, true, order);
            for (std::size_t i = 1; i < root->kids.size(); ++i) {
                validateStatement(root->kids.at(i), functions, inFunction, inLoop, inArg, order);
            }
            break;
        }

        case Type::While: {
            validateStatement(root->kids.at(0), functions, inFunction, inLoop, true, order);
            validateStatement(root->kids.at(1), functions, inFunction, true, inArg, order);
            break;
        }

        default: {
            for (const Statement* s : root->kids) {
                validateStatement(s, functions, inFunction, inLoop, true, order);
            }
            break;
        }
    }
}

}  // namespace

// 加载期校验: 函数定义位置/重名、调用 arity 与定义顺序、return/break/continue 位置
void validate(const std::vector<Statement*>& program) {
    std::unordered_map<std::string, FuncInfo> functions;
    for (std::size_t i = 0; i < program.size(); ++i) {
        const Statement* s = program.at(i);
        if (s->type != Type::Func) continue;
        auto result = functions.emplace(s->name, FuncInfo{s->params.size(), i});
        if (!result.second) parseError("func: duplicate function $" + s->name);
    }

    for (std::size_t i = 0; i < program.size(); ++i) {
        const Statement* s = program.at(i);
        if (s->type == Type::Func) {
            validateStatement(s->kids.at(0), functions, true, false, false, i);
        } else {
            validateStatement(s, functions, false, false, false, i);
        }
    }
}
