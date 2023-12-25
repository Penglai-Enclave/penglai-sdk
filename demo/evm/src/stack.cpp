#include "../include/stack.h"

void EVMStack::push(const uint256_t &val) {
    evm_stack.push_front(val);
}

uint256_t EVMStack::pop() {
    uint256_t result = evm_stack.front();
    evm_stack.pop_front();
    return result;
}

uint64_t EVMStack::pop64() {
    uint256_t result = evm_stack.front();
    evm_stack.pop_front();
    return static_cast<uint64_t>(result);
}

uint64_t EVMStack::size(){
    return evm_stack.size();
}

void EVMStack::dup(uint64_t index){
    if(index < evm_stack.size()){
        evm_stack.push_front(evm_stack[index]);
    }
}

void EVMStack::swap(uint64_t index){
    if(index < evm_stack.size()){
        std::swap(evm_stack[0], evm_stack[index]);
    }
}