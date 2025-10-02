// Project identifier: 1CAEF3A0FEDD0DEC26BA9808C69D4D22A9962768

#include "evaluate.hpp"

#include <cstdint>
#include <vector>

int64_t applyOp(char op, int64_t a, int64_t b) {
    switch (op) {
        case '+': return a + b;
        case '-': return a - b;
        case '*': return a * b;
        case '/': return a / b; 
    }

    return 0;
}

void tryEvaluate(std::vector<int64_t> &nums, std::vector<char> &ops) {

    while (!ops.empty() && nums.size() >= 2) {
        char op = ops.back(); ops.pop_back();
        int64_t b = nums.back(); nums.pop_back();
        int64_t a = nums.back(); nums.pop_back();
        nums.push_back(applyOp(op, a, b));
    }
}

std :: int64_t evaluate (std :: string const & expression ){
    std::vector<int64_t> nums;
    std::vector<char> ops;

    for (char c : expression) {
        if (isdigit(c)) {
            nums.push_back(c - '0');
            tryEvaluate(nums, ops);
        } else { // 运算符
            ops.push_back(c);
            tryEvaluate(nums, ops);
        }
    }

    return nums.back();
}