#include <cctype>
#include <cstdlib>
#include <iostream>
#include <limits>
#include <string>
#include <unordered_map>
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
    {"aset", {Type::ArraySet, 2, true}},
    {"aget", {Type::ArrayGet, 1, true}},
    {"alen", {Type::ArrayLen, 0, true}},

    {"if", {Type::If, 2, false}},
    {"while", {Type::While, 2, false}},
};

void parseError(const std::string& msg) {
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

    std::cerr << "invalid token: " << token << std::endl;
    return nullptr;
}
