#include <cstdlib>
#include <iostream>
#include <limits>
#include <string>
#include <unordered_map>
#include <vector>

#include "toylang.h"

namespace {

constexpr ll kBitWidth = static_cast<ll>(std::numeric_limits<unsigned long long>::digits);
constexpr std::size_t kMaxCallDepth = 1000;

// 读变量: 当前调用帧 -> 全局 -> 0
ll getVar(const State& state, const std::string& name) {
    if (!state.frames.empty()) {
        auto it = state.frames.back().find(name);
        if (it != state.frames.back().end()) return it->second;
    }
    auto it = state.global.find(name);
    return it == state.global.end() ? 0 : it->second;
}

// 写变量: 顶层写全局, 函数内写当前帧
void setVar(State& state, const std::string& name, ll value) {
    if (state.frames.empty()) {
        state.global[name] = value;
    } else {
        state.frames.back()[name] = value;
    }
}

}  // namespace

// 执行 root
Result eval(const Statement* root, State& state) {
    if (root == nullptr) {
        std::cerr << "fail to parse" << std::endl;
        std::exit(1);
    }

    switch (root->type) {
        case Type::Block: {
            Result ret;
            for (const Statement* s : root->kids) {
                ret = eval(s, state);
                if (ret.flow != Flow::Normal) break;
            }
            return ret;
        }

        case Type::Num: {
            return {root->num};
        }

        case Type::Var: {
            return {getVar(state, root->name)};
        }

        case Type::InputNum: {
            std::string s;
            if (!(std::cin >> s)) {
                std::cerr << "input number failed" << std::endl;
                return {0};
            }
            ll num = 0;
            if (!toIntNumber(s, num)) {
                std::cerr << "invalid number: " << s << std::endl;
                return {0};
            }
            return {num};
        }

        case Type::InputChar: {
            return {std::cin.get()};
        }

        case Type::InputVis: {
            char c;
            std::cin >> c;
            return {c};
        }

        case Type::Not: {
            return {eval(root->kids.at(0), state).value == 0 ? 1 : 0};
        }

        case Type::Print: {
            std::cout << eval(root->kids.at(0), state).value;
            return {1};
        }

        case Type::PrintChar: {
            ll c = eval(root->kids.at(0), state).value;
            if (c < 0 || c > 127) {
                std::cerr << "not a char: " << c << std::endl;
                return {0};
            }
            std::cout << static_cast<char>(c);
            return {1};
        }

        case Type::Add: {
            ll a = eval(root->kids.at(0), state).value;
            ll b = eval(root->kids.at(1), state).value;
            return {a + b};
        }

        case Type::Sub: {
            ll a = eval(root->kids.at(0), state).value;
            ll b = eval(root->kids.at(1), state).value;
            return {a - b};
        }

        case Type::Mul: {
            ll a = eval(root->kids.at(0), state).value;
            ll b = eval(root->kids.at(1), state).value;
            return {a * b};
        }

        case Type::Div: {
            ll a = eval(root->kids.at(0), state).value;
            ll b = eval(root->kids.at(1), state).value;
            if (b == 0) {
                std::cerr << "division by zero" << std::endl;
                return {0};
            }
            return {a / b};
        }

        case Type::Mod: {
            ll a = eval(root->kids.at(0), state).value;
            ll b = eval(root->kids.at(1), state).value;
            if (b == 0) {
                std::cerr << "division by zero" << std::endl;
                return {0};
            }
            return {a % b};
        }

        case Type::Greater: {
            ll a = eval(root->kids.at(0), state).value;
            ll b = eval(root->kids.at(1), state).value;
            return {a > b ? 1 : 0};
        }

        case Type::Less: {
            ll a = eval(root->kids.at(0), state).value;
            ll b = eval(root->kids.at(1), state).value;
            return {a < b ? 1 : 0};
        }

        case Type::Equal: {
            ll a = eval(root->kids.at(0), state).value;
            ll b = eval(root->kids.at(1), state).value;
            return {a == b ? 1 : 0};
        }

        case Type::And: {
            if (eval(root->kids.at(0), state).value == 0) return {0};  // 短路
            return {eval(root->kids.at(1), state).value != 0 ? 1 : 0};
        }

        case Type::Or: {
            if (eval(root->kids.at(0), state).value != 0) return {1};  // 短路
            return {eval(root->kids.at(1), state).value != 0 ? 1 : 0};
        }

        case Type::BitAnd: {
            ll a = eval(root->kids.at(0), state).value;
            ll b = eval(root->kids.at(1), state).value;
            return {a & b};
        }

        case Type::BitOr: {
            ll a = eval(root->kids.at(0), state).value;
            ll b = eval(root->kids.at(1), state).value;
            return {a | b};
        }

        case Type::BitXor: {
            ll a = eval(root->kids.at(0), state).value;
            ll b = eval(root->kids.at(1), state).value;
            return {a ^ b};
        }

        case Type::BitNot: {
            return {~eval(root->kids.at(0), state).value};
        }

        case Type::LeftShift: {
            ll a = eval(root->kids.at(0), state).value;
            ll b = eval(root->kids.at(1), state).value;
            if (b < 0 || b >= kBitWidth) {
                std::cerr << "shift amount out of range: " << b << std::endl;
                return {0};
            }
            return {static_cast<ll>(static_cast<unsigned long long>(a) << b)};
        }

        case Type::RightShift: {
            ll a = eval(root->kids.at(0), state).value;
            ll b = eval(root->kids.at(1), state).value;
            if (b < 0 || b >= kBitWidth) {
                std::cerr << "shift amount out of range: " << b << std::endl;
                return {0};
            }
            return {a >> b};
        }

        case Type::Set: {
            ll val = eval(root->kids.at(0), state).value;
            setVar(state, root->name, val);
            return {val};
        }

        case Type::GlobalSet: {
            ll val = eval(root->kids.at(0), state).value;
            state.global[root->name] = val;
            return {val};
        }

        case Type::ArraySet: {
            ll raw_idx = eval(root->kids.at(0), state).value;
            if (raw_idx < 0) {
                std::cerr << "negative index" << std::endl;
                return {0};
            }
            std::size_t idx = static_cast<std::size_t>(raw_idx);
            if (idx >= state.array[root->name].size()) {
                state.array[root->name].resize(idx + 1, 0);
            }
            ll val = eval(root->kids.at(1), state).value;
            state.array[root->name].at(idx) = val;
            return {1};
        }

        case Type::ArrayGet: {
            ll raw_idx = eval(root->kids.at(0), state).value;
            auto it = state.array.find(root->name);
            if (it == state.array.end()) {
                std::cerr << "undefined array" << std::endl;
                return {0};
            }
            if (raw_idx < 0) {
                std::cerr << "index out of range" << std::endl;
                return {0};
            }
            std::size_t idx = static_cast<std::size_t>(raw_idx);
            if (idx >= it->second.size()) {
                std::cerr << "index out of range" << std::endl;
                return {0};
            }
            return {it->second.at(idx)};
        }

        case Type::ArrayLen: {
            auto it = state.array.find(root->name);
            return {it == state.array.end() ? 0 : static_cast<ll>(it->second.size())};
        }

        case Type::If: {
            if (eval(root->kids.at(0), state).value == 0) {
                if (root->kids.size() == 2) return {0};
                return eval(root->kids.at(2), state);
            }
            return eval(root->kids.at(1), state);
        }

        case Type::While: {
            std::size_t count = 0;
            while (eval(root->kids.at(0), state).value != 0) {
                count++;
                Result r = eval(root->kids.at(1), state);
                if (r.flow == Flow::Break) break;
                if (r.flow == Flow::Return) return r;  // 返回穿透循环
            }
            return {static_cast<ll>(count)};
        }

        case Type::Break: {
            return {0, Flow::Break};
        }

        case Type::Continue: {
            return {0, Flow::Continue};
        }

        case Type::Func: {
            state.function[root->name] = root;
            return {1};
        }

        case Type::Call: {
            auto it = state.function.find(root->name);
            if (it == state.function.end()) {
                std::cerr << "undefined function: " << root->name << std::endl;
                return {0};
            }
            const Statement* fn = it->second;
            if (root->kids.size() != fn->params.size()) {
                std::cerr << "arity mismatch: " << root->name << std::endl;
                return {0};
            }
            if (state.depth >= kMaxCallDepth) {
                std::cerr << "call depth exceeded: " << root->name << std::endl;
                return {0};
            }

            // 实参在调用者帧内从左到右求值
            std::vector<ll> args;
            args.reserve(root->kids.size());
            for (const Statement* s : root->kids) {
                Result arg = eval(s, state);
                if (arg.flow != Flow::Normal) {
                    std::cerr << "control flow in argument list" << std::endl;
                    return {0};
                }
                args.push_back(arg.value);
            }

            state.frames.emplace_back();
            for (std::size_t i = 0; i < fn->params.size(); ++i) {
                state.frames.back()[fn->params.at(i)] = args.at(i);
            }
            state.depth++;
            Result r = eval(fn->kids.at(0), state);
            state.depth--;
            state.frames.pop_back();

            if (r.flow == Flow::Break || r.flow == Flow::Continue) {
                std::cerr << "break/continue escaped function: " << root->name << std::endl;
                return {0};
            }
            return {r.value};
        }

        case Type::Return: {
            Result r = eval(root->kids.at(0), state);
            r.flow = Flow::Return;
            return r;
        }
    }

    return {0};
}
