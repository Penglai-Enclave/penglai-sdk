#ifndef PENGLAI_EVM_PROCESSOR_H
#define PENGLAI_EVM_PROCESSOR_H

#include <vector>
#include <map>
#include <set>
#include <algorithm>
#include <string>
#include "stack.h"
#include "instruction.h"
#include "util.h"

using namespace std;

struct EVMResult{
    vector<uint8_t> output = {};
    // 格式化：第一个数字表示有几个topic，之后每32个数字组成一个topic，然后是一个数字表示log内容的长度，接下去是具体的内容，如果还没结束，下一个数字代表一个新的log的开头数字
    vector<uint8_t> log = {};
    uint64_t evm_last_gas = 0;
};

class EVMContext{
public:
    EVMStack* evm_stack;
    vector<uint8_t> evm_codes;
    set<uint64_t> evm_jumpdest;
    vector<uint8_t> evm_input;
    vector<uint8_t> evm_memory;
    map<uint256_t, uint256_t> evm_storage;
    int evm_pc;
    uint256_t evm_value;
    uint64_t evm_last_gas;
    EVMResult* evm_result;

    EVMContext(EVMStack* _evm_stack, vector<uint8_t> _evm_codes, set<uint64_t> _evm_jumpdest,
               vector<uint8_t> _evm_input, vector<uint8_t> _evm_memory, map<uint256_t, uint256_t> _evm_storage,
               int _evm_pc, uint256_t _evm_value, uint64_t _evm_last_gas, EVMResult* _evm_result){
        evm_stack = _evm_stack;
        evm_codes = _evm_codes;
        evm_jumpdest = _evm_jumpdest;
        evm_input = _evm_input;
        evm_memory = _evm_memory;
        evm_storage = _evm_storage;
        evm_pc = _evm_pc;
        evm_value = _evm_value;
        evm_last_gas = _evm_last_gas;
        evm_result = _evm_result;
    }
};

class EVMProcessor {
private:
    EVMStack* evm_stack;
    vector<uint8_t> evm_codes;
    set<uint64_t> evm_jumpdest;
    vector<uint8_t> evm_input;
    vector<uint8_t> evm_memory;
    map<uint256_t, uint256_t> evm_storage;
    int evm_pc;
    uint256_t evm_value;
    EVMResult* evm_result;
    bool if_stopped;
    EVMContext* evm_context;
    uint64_t evm_last_gas;
    uint8_t evm_last_mem_size;
    static int temp_count;

    std::map<uint8_t, uint8_t> basic_op_gas = {
            {Instruction::STOP, 0},
            {Instruction::ADD, 3},
            {Instruction::MUL, 5},
            {Instruction::SUB, 3},
            {Instruction::DIV, 5},
            {Instruction::SDIV, 5},
            {Instruction::MOD, 5},
            {Instruction::SMOD, 5},
            {Instruction::ADDMOD, 8},
            {Instruction::MULMOD, 8},
            {Instruction::EXP, 0},
            {Instruction::SIGNEXTEND, 5},
            {Instruction::LT, 3},
            {Instruction::GT, 3},
            {Instruction::SLT, 3},
            {Instruction::SGT, 3},
            {Instruction::EQ, 3},
            {Instruction::ISZERO, 3},
            {Instruction::AND, 3},
            {Instruction::OR, 3},
            {Instruction::XOR, 3},
            {Instruction::NOT, 3},
            {Instruction::BYTE, 3},
            {Instruction::SHA3, 0},
            {Instruction::ADDRESS, 2},
            {Instruction::BALANCE, 0},
            {Instruction::ORIGIN, 2},
            {Instruction::CALLER, 2},
            {Instruction::CALLVALUE, 2},
            {Instruction::CALLDATALOAD, 2},
            {Instruction::CALLDATASIZE, 2},
            {Instruction::CALLDATACOPY, 0},
            {Instruction::CODESIZE, 2},
            {Instruction::CODECOPY, 0},
            {Instruction::GASPRICE, 2},
            {Instruction::EXTCODESIZE, 0},
            {Instruction::EXTCODECOPY, 0},
            {Instruction::BLOCKHASH, 20},
            {Instruction::COINBASE, 2},
            {Instruction::TIMESTAMP, 2},
            {Instruction::NUMBER, 2},
            {Instruction::DIFFICULTY, 2},
            {Instruction::GASLIMIT, 2},
            {Instruction::POP, 2},
            {Instruction::MLOAD, 3},
            {Instruction::MSTORE, 3},
            {Instruction::MSTORE8, 3},
            {Instruction::SLOAD, 2100},
            {Instruction::SSTORE, 2100},
            {Instruction::JUMP, 8},
            {Instruction::JUMPI, 10},
            {Instruction::PC, 2},
            {Instruction::MSIZE, 2},
            {Instruction::GAS, 2},
            {Instruction::JUMPDEST, 1},
            {Instruction::PUSH1, 3},
            {Instruction::PUSH2, 3},
            {Instruction::PUSH3, 3},
            {Instruction::PUSH4, 3},
            {Instruction::PUSH5, 3},
            {Instruction::PUSH6, 3},
            {Instruction::PUSH7, 3},
            {Instruction::PUSH8, 3},
            {Instruction::PUSH9, 3},
            {Instruction::PUSH10, 3},
            {Instruction::PUSH11, 3},
            {Instruction::PUSH12, 3},
            {Instruction::PUSH13, 3},
            {Instruction::PUSH14, 3},
            {Instruction::PUSH15, 3},
            {Instruction::PUSH16, 3},
            {Instruction::PUSH17, 3},
            {Instruction::PUSH18, 3},
            {Instruction::PUSH19, 3},
            {Instruction::PUSH20, 3},
            {Instruction::PUSH21, 3},
            {Instruction::PUSH22, 3},
            {Instruction::PUSH23, 3},
            {Instruction::PUSH24, 3},
            {Instruction::PUSH25, 3},
            {Instruction::PUSH26, 3},
            {Instruction::PUSH27, 3},
            {Instruction::PUSH28, 3},
            {Instruction::PUSH29, 3},
            {Instruction::PUSH30, 3},
            {Instruction::PUSH31, 3},
            {Instruction::PUSH32, 3},
            {Instruction::DUP1, 3},
            {Instruction::DUP2, 3},
            {Instruction::DUP3, 3},
            {Instruction::DUP4, 3},
            {Instruction::DUP5, 3},
            {Instruction::DUP6, 3},
            {Instruction::DUP7, 3},
            {Instruction::DUP8, 3},
            {Instruction::DUP9, 3},
            {Instruction::DUP10, 3},
            {Instruction::DUP11, 3},
            {Instruction::DUP12, 3},
            {Instruction::DUP13, 3},
            {Instruction::DUP14, 3},
            {Instruction::DUP15, 3},
            {Instruction::DUP16, 3},
            {Instruction::SWAP1, 3},
            {Instruction::SWAP2, 3},
            {Instruction::SWAP3, 3},
            {Instruction::SWAP4, 3},
            {Instruction::SWAP5, 3},
            {Instruction::SWAP6, 3},
            {Instruction::SWAP7, 3},
            {Instruction::SWAP8, 3},
            {Instruction::SWAP9, 3},
            {Instruction::SWAP10, 3},
            {Instruction::SWAP11, 3},
            {Instruction::SWAP12, 3},
            {Instruction::SWAP13, 3},
            {Instruction::SWAP14, 3},
            {Instruction::SWAP15, 3},
            {Instruction::SWAP16, 3},
            {Instruction::LOG0, 0},
            {Instruction::LOG1, 0},
            {Instruction::LOG2, 0},
            {Instruction::LOG3, 0},
            {Instruction::LOG4, 0},
            {Instruction::CREATE, 0},
            {Instruction::CALL, 0},
            {Instruction::CALLCODE, 0},
            {Instruction::RETURN, 0},
            {Instruction::DELEGATECALL, 0},
            {Instruction::SELFDESTRUCT, 0}
    };

    uint64_t get_gas_for_copy(uint8_t size);
    uint64_t get_gas_for_mem(uint8_t size);

public:
    EVMProcessor();
    EVMProcessor(uint256_t value);
    EVMProcessor(EVMContext* context);
    vector<uint8_t> copy_from_mem(const uint64_t offset, const uint64_t size);
    void copy_to_mem(vector<uint8_t>& src, vector<uint8_t>& dst);
    void push();
    void add();
    void mul();
    void sub();
    void div();
    void sdiv();
    void mod();
    void smod();
    void addmod();
    void mulmod();
    void exp();
    void signextend();
    void lt();
    void gt();
    void slt();
    void sgt();
    void eq();
    void iszero();
    void and_();
    void or_();
    void xor_();
    void not_();
    void byte();
    void callvalue();
    void calldataload();
    void calldatasize();
    void calldatacopy();
    void codesize();
    void codecopy();
    void pop();
    void mload();
    void mstore();
    void mstore8();
    void sload();
    void sstore();
    bool jump();
    bool jumpi();
    void pc();
    void msize();
    void jumpdest();
    void dup();
    void swap();
    void log();
    void return_();
    EVMResult* run(vector<uint8_t> codes, vector<uint8_t> input, uint64_t last_gas);
    void stop_evm();
    EVMResult* resume_evm();
    EVMContext* get_context();
    void update_mem_gas();
};

#endif