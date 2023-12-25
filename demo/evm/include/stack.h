#ifndef PENGLAI_EVM_STACK_H
#define PENGLAI_EVM_STACK_H

#include <stack>
#include <stdint.h>
#include <algorithm>
#include "./uint256/uint256_t.h"

class EVMStack {
private:
    std::deque<uint256_t> evm_stack;

public:
    uint256_t pop();
    uint64_t pop64();
    void push(const uint256_t& val);
    uint64_t size();
    void dup(uint64_t index);
    void swap(uint64_t index);
};

#endif