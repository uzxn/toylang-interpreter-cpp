#include "toylang.h"

#include <fstream>
#include <iostream>
#include <string>
#include <vector>

int main(int argc, char** argv) {
    if (argc == 1) {
        std::cerr << "need input file" << std::endl;
        return 1;
    }

    std::ifstream fin(argv[1]);
    if (!fin) {
        std::cerr << "cannot open file: " << argv[1] << std::endl;
        return 1;
    }

    State state;
    std::vector<std::string> list;
    for (std::string token; fin >> token;) list.push_back(token);

    std::size_t index = 0;
    std::vector<Statement*> program;
    while (index < list.size()) program.push_back(parse(list, index));
    validate(program);

    index = 0;
    while (index < program.size()) eval(program.at(index++), state);

    return 0;
}
