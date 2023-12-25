#include "../include/processor.h"

extern "C" void eapp_print(const char*s, ...);

int EVMProcessor::temp_count = 0;

int get_sign(uint256_t& num){
    if((num >> 255) == 0){
        return 1;
    }else{
        return -1;
    }
}

EVMProcessor::EVMProcessor() {
    evm_stack = new EVMStack();
    evm_pc = 0;
    evm_memory.resize(50000);
    evm_last_mem_size = 0;
    evm_result = new EVMResult();
    if_stopped = false;
}

EVMProcessor::EVMProcessor(uint256_t value) {
    evm_stack = new EVMStack();
    evm_pc = 0;
    evm_last_mem_size = 0;
    evm_memory.resize(50000);
    evm_result = new EVMResult();
    evm_value = value;
    if_stopped = false;
}

EVMProcessor::EVMProcessor(EVMContext *context) {
    evm_stack = context->evm_stack;
    evm_codes = context->evm_codes;
    evm_jumpdest = context->evm_jumpdest;
    evm_input = context->evm_input;
    evm_memory = context->evm_memory;
    evm_storage = context->evm_storage;
    evm_pc = context->evm_pc;
    evm_value = context->evm_value;
    evm_result = new EVMResult();
    if_stopped = false;
    evm_last_mem_size = evm_memory.size();
    evm_last_gas = context->evm_last_gas;
}

std::vector<uint8_t> EVMProcessor::copy_from_mem(const uint64_t offset, const uint64_t size){
    return {evm_memory.begin() + offset, evm_memory.begin() + offset + size};
}

void EVMProcessor::copy_to_mem(vector<uint8_t>& source, vector<uint8_t>& destination){
    uint64_t destination_offset = evm_stack->pop64();
    uint64_t source_offset = evm_stack->pop64();
    uint64_t size = evm_stack->pop64();
    if(size == 0){
        return;
    }

    uint64_t destination_last = destination_offset + size;
    if(destination_last < destination_offset){
        return;
    }

    if(destination_last > destination.size()){
        destination.resize(destination_last);
    }

    uint64_t source_last = source_offset + size;
    uint64_t real_source_last = min(source_last, source.size());

    uint64_t remaining = 0;
    if (real_source_last > source_offset){
        copy(source.begin() + source_offset, source.begin() + real_source_last, destination.begin() + destination_offset);
        remaining = source_last - real_source_last;
    }else{
        remaining = size;
    }

    // 用0填充多余的部分
    fill(destination.begin() + destination_last - remaining, destination.begin() + destination_last, 0);
}

void EVMProcessor::push() {
    uint8_t bytes = evm_codes[evm_pc] - PUSH1 + 1;
    int temp_pc = evm_pc + 1;

    uint256_t imm(0);
    for (int i = 0; i < bytes; i++){
        imm = ((imm << 8) | (evm_codes[temp_pc++]));
    }
    evm_stack->push(imm);
    evm_pc = temp_pc;
}

void EVMProcessor::add(){
    uint256_t a = evm_stack->pop();
    uint256_t b = evm_stack->pop();
    evm_stack->push(a + b);
}

void EVMProcessor::mul(){
    uint256_t a = evm_stack->pop();
    uint256_t b = evm_stack->pop();
    evm_stack->push(a * b);
}

void EVMProcessor::sub(){
    uint256_t a = evm_stack->pop();
    uint256_t b = evm_stack->pop();
    evm_stack->push(a - b);
}

void EVMProcessor::div(){
    uint256_t a = evm_stack->pop();
    uint256_t b = evm_stack->pop();
    if(b == 0){
        evm_stack->push(0);
    }else{
        evm_stack->push(a / b);
    }
}

void EVMProcessor::sdiv(){
    uint256_t a = evm_stack->pop();
    uint256_t b = evm_stack->pop();
    if(b == 0){
        evm_stack->push(0);
        return;
    }

    uint256_t uint256_max(uint128_t((uint64_t) -1, (uint64_t) -1), uint128_t((uint64_t) -1, (uint64_t) -1));
    uint256_t max_negative = (uint256_max / 2) + 1;
    if(a == max_negative && b == -1){
        evm_stack->push(a);
        return;
    }

    int sign_a = get_sign(a);
    int sign_b = get_sign(b);
    if (sign_a == -1){
        a = 0 - a;
    }
    if (sign_b == -1){
        b = 0 - b;
    }

    uint256_t result = (a / b);
    if (sign_a != sign_b){
        result = 0 - result;
    }
    evm_stack->push(result);
}

void EVMProcessor::mod(){
    uint256_t a = evm_stack->pop();
    uint256_t b = evm_stack->pop();
    if(b == 0){
        evm_stack->push(0);
    }else{
        evm_stack->push(a % b);
    }
}

void EVMProcessor::smod(){
    uint256_t a = evm_stack->pop();
    uint256_t b = evm_stack->pop();
    if(b == 0){
        evm_stack->push(0);
        return;
    }

    int sign_a = get_sign(a);
    int sign_b = get_sign(b);
    if (sign_a == -1){
        a = 0 - a;
    }
    if (sign_b == -1){
        b = 0 - b;
    }

    uint256_t result = (a % b);
    if (sign_a == -1){
        result = 0 - result;
    }
    evm_stack->push(result);
}

void EVMProcessor::addmod(){
    // TODO: 修改类型为uint512
    uint256_t a = evm_stack->pop();
    uint256_t b = evm_stack->pop();
    uint256_t n = evm_stack->pop();
    if(n == 0){
        evm_stack->push(0);
    }else{
        evm_stack->push((a + b) % n);
    }
}

void EVMProcessor::mulmod(){
    // TODO: 修改类型为uint512
    uint256_t a = evm_stack->pop();
    uint256_t b = evm_stack->pop();
    uint256_t n = evm_stack->pop();
    if(n == 0){
        evm_stack->push(0);
    }else{
        evm_stack->push((a * b) % n);
    }
}

void EVMProcessor::exp(){
    uint256_t a = evm_stack->pop();
    uint64_t b = evm_stack->pop64();
    uint256_t result = 1;
    while(b > 0){
        result = result * a;
        b--;
    }
    evm_stack->push(result);
}

void EVMProcessor::signextend(){
    uint256_t b = evm_stack->pop();
    uint256_t x = evm_stack->pop();
    if(b >= 31){
        evm_stack->push(x);
        return;
    }

    int current_bit = static_cast<uint8_t>(b & std::numeric_limits<uint8_t>::max()) * 8 + 7;
    int sign = static_cast<uint8_t>((x >> current_bit) & 1);
    uint256_t zero = uint256_t(0);
    uint256_t mask = (~zero) >> (256 - current_bit);
    uint256_t result = (x | mask); // 先把符号位全部设置为0
    if(sign == 1){
        result = result | ((~zero) << current_bit);
    }
    evm_stack->push(result);
}

void EVMProcessor::lt(){
    uint256_t a = evm_stack->pop();
    uint256_t b = evm_stack->pop();
    if(a < b){
        evm_stack->push(1);
    }else{
        evm_stack->push(0);
    }
}

void EVMProcessor::gt(){
    uint256_t a = evm_stack->pop();
    uint256_t b = evm_stack->pop();
    if(a > b){
        evm_stack->push(1);
    }else{
        evm_stack->push(0);
    }
}

void EVMProcessor::slt(){
    uint256_t a = evm_stack->pop();
    uint256_t b = evm_stack->pop();
    int sign_a = get_sign(a);
    int sign_b = get_sign(b);

    if(sign_a != sign_b){
        if(sign_a == -1){
            evm_stack->push(1);
        }else{
            evm_stack->push(0);
        }
    }else{
        if(a < b){
            evm_stack->push(1);
        }else{
            evm_stack->push(0);
        }
    }
}

void EVMProcessor::sgt(){
    uint256_t a = evm_stack->pop();
    uint256_t b = evm_stack->pop();
    int sign_a = get_sign(a);
    int sign_b = get_sign(b);

    if(sign_a != sign_b){
        if(sign_a == -1){
            evm_stack->push(0);
        }else{
            evm_stack->push(1);
        }
    }else{
        if(a > b){
            evm_stack->push(1);
        }else{
            evm_stack->push(0);
        }
    }
}

void EVMProcessor::eq(){
    uint256_t a = evm_stack->pop();
    uint256_t b = evm_stack->pop();
    if(a == b){
        evm_stack->push(1);
    }else{
        evm_stack->push(0);
    }
}

void EVMProcessor::iszero(){
    uint256_t a = evm_stack->pop();
    if(a == 0){
        evm_stack->push(1);
    }else{
        evm_stack->push(0);
    }
}

void EVMProcessor::and_(){
    uint256_t a = evm_stack->pop();
    uint256_t b = evm_stack->pop();
    evm_stack->push(a & b);
}

void EVMProcessor::or_(){
    uint256_t a = evm_stack->pop();
    uint256_t b = evm_stack->pop();
    evm_stack->push(a | b);
}

void EVMProcessor::xor_(){
    uint256_t a = evm_stack->pop();
    uint256_t b = evm_stack->pop();
    evm_stack->push(a ^ b);
}

void EVMProcessor::not_(){
    uint256_t a = evm_stack->pop();
    evm_stack->push(~a);
}

void EVMProcessor::byte(){
    uint256_t i = evm_stack->pop();
    uint256_t x = evm_stack->pop();
    if(i >= 32){
        evm_stack->push(0);
        return;
    }

    int shift = 256 - 8 - 8 * static_cast<uint8_t>(i & std::numeric_limits<uint8_t>::max());
    uint256_t result = (x >> shift);
    result = result & uint256_t(255);
    evm_stack->push(result);
}

void EVMProcessor::callvalue(){
    evm_stack->push(evm_value);
}

void EVMProcessor::calldataload(){
    uint64_t offset = evm_stack->pop64();
    int input_size = evm_input.size();

    uint256_t value = 0;
    for(int i = 0; i < 32; i++){
        if(offset + i < input_size){
            value = (value << 8) + evm_input[offset + i];
        }else{
            value = (value << 8);
        }
    }
    evm_stack->push(value);
}

void EVMProcessor::calldatasize(){
    evm_stack->push(evm_input.size());
}

void EVMProcessor::calldatacopy(){
    copy_to_mem(evm_input, evm_memory);
    uint64_t gas_cost = get_gas_for_copy(evm_input.size());
    evm_last_gas = evm_last_gas - gas_cost;
}

void EVMProcessor::codesize(){
    evm_stack->push(evm_codes.size());
}

void EVMProcessor::codecopy(){
    copy_to_mem(evm_codes, evm_memory);
    uint64_t gas_cost = get_gas_for_copy(evm_codes.size());
    evm_last_gas = evm_last_gas - gas_cost;
}

void EVMProcessor::pop(){
    evm_stack->pop();
}

void EVMProcessor::mload(){
    uint64_t offset = evm_stack->pop64();
    uint8_t* start = evm_memory.data() + offset;
    evm_stack->push(uint8_vector_to_uint256(start, 32));
}

void EVMProcessor::mstore(){
    uint64_t offset = evm_stack->pop64();
    uint256_t word = evm_stack->pop();
    store_uint256_to_uint8_vector(word, evm_memory.data() + offset);
}

void EVMProcessor::mstore8() {
    uint64_t offset = evm_stack->pop64();
    uint8_t b = static_cast<uint8_t>(evm_stack->pop() & std::numeric_limits<uint8_t>::max());
    evm_memory[offset] = b;
}

void EVMProcessor::sload(){
    uint256_t key = evm_stack->pop();
    if(evm_storage.count(key) == 0){
        evm_stack->push(0);
    }else{
        evm_stack->push(evm_storage[key]);
    }
}

void EVMProcessor::sstore(){
    uint256_t key = evm_stack->pop();
    uint256_t value = evm_stack->pop();
    evm_storage[key] = value;
}

bool EVMProcessor::jump(){
    uint64_t destination = evm_stack->pop64();
    if(evm_jumpdest.count(destination) == 0){
        return true;
    }

    evm_pc = destination;
    return false;
}

bool EVMProcessor::jumpi(){
    uint64_t destination = evm_stack->pop64();
    uint256_t condition = evm_stack->pop();
    if(!condition){
        return true;
    }

    if(evm_jumpdest.count(destination) == 0){
        return true;
    }

    evm_pc = destination;
    return false;
}

void EVMProcessor::pc(){
    evm_stack->push(evm_pc);
}

void EVMProcessor::msize(){
    evm_stack->push(evm_memory.size());
}

void EVMProcessor::jumpdest(){}

void EVMProcessor::dup(){
    uint64_t index = evm_codes[evm_pc] - DUP1;
    if(index < evm_stack->size()){
        evm_stack->dup(index);
    }
}

void EVMProcessor::swap(){
    uint64_t index = evm_codes[evm_pc] - SWAP1 + 1;
    if(index < evm_stack->size()){
        evm_stack->swap(index);
    }
}

void EVMProcessor::log(){
    uint8_t total_num = evm_codes[evm_pc] - LOG0;
    uint64_t offset = evm_stack->pop64();
    uint64_t size = evm_stack->pop64();

    vector<uint256_t> topics(total_num);
    for (int i = 0; i < total_num; i++){
        topics[i] = evm_stack->pop();
    }

    evm_result->log.push_back(total_num);
    for (int i = 0; i < total_num; i++){
        uint8_t temp_vector[32];
        store_uint256_to_uint8_vector(topics[i], temp_vector);
        for(int i = 0; i < 32; i++){
            evm_result->log.push_back(temp_vector[i]);
        }
    }

    // 存储明文
    vector<unsigned char> plaintext_vector;
    for(int i = offset; i < offset + size; i++){
        plaintext_vector.push_back(evm_memory[i]);
    }

    evm_result->log.push_back(plaintext_vector.size());
    for(int i = 0; i < plaintext_vector.size(); i++){
        evm_result->log.push_back(plaintext_vector[i]);
    }

    uint64_t gas_cost = 375 + 375 * total_num + 8 * plaintext_vector.size();

    // 存储密文
//    unsigned char ciphertext[1000];
//    int ciphertext_len;
//    encrypt_data(&plaintext_vector[0], size, ciphertext, &ciphertext_len);
//
//    evm_result->log.push_back(ciphertext_len);
//    for(int i = 0; i < ciphertext_len; i++){
//        evm_result->log.push_back(ciphertext[i]);
//    }
//
//    uint64_t gas_cost = 375 + 375 * total_num + 8 * ciphertext_len;

    evm_last_gas = evm_last_gas - gas_cost;
}

void EVMProcessor::return_() {
    uint256_t offset = evm_stack->pop();
    uint256_t size = evm_stack->pop();

    std::vector<uint8_t> output = copy_from_mem(offset, size);
    evm_result->output = output;
    evm_result->evm_last_gas = evm_last_gas;
}

set<uint64_t> get_jumpdest(vector<uint8_t> code){
    set<uint64_t> jumpdest;
    for (uint64_t i = 0; i < code.size(); i++){
        uint8_t op = code[i];
        if (op >= PUSH1 && op <= PUSH32){
            uint8_t bytes = op - PUSH1 + 1;
            i += bytes;
        }
        else if (op == JUMPDEST){
            jumpdest.insert(i);
        }
    }
    return jumpdest;
}

uint64_t EVMProcessor::get_gas_for_copy(uint8_t size)
{
    uint8_t data_size_words = (size + 31) / 32;
    uint64_t gas_cost = 3 + 3 * data_size_words;
    return gas_cost;
}

uint64_t EVMProcessor::get_gas_for_mem(uint8_t size)
{
    int size_word = size / 32;
    return size_word * size_word / 512 + 3 * size_word;
}

void EVMProcessor::update_mem_gas(){
    if(evm_memory.size() <= evm_last_mem_size){
        return;
    }
    uint8_t new_mem_size = evm_memory.size() - evm_last_mem_size;
    uint8_t new_mem_size_word = (new_mem_size + 31) / 32 * 32;
    uint64_t mem_gas = get_gas_for_mem(evm_memory.size()) - get_gas_for_mem(evm_last_mem_size);
    evm_last_gas = evm_last_gas - mem_gas;
    evm_last_mem_size = evm_memory.size();
}

void print_op(uint8_t op){
    switch (op)
    {
        case Instruction::PUSH1:
            eapp_print("PUSH1\n");
            break;
        case Instruction::PUSH2:
            eapp_print("PUSH2\n");
            break;
        case Instruction::PUSH3:
            eapp_print("PUSH3\n");
            break;
        case Instruction::PUSH4:
            eapp_print("PUSH4\n");
            break;
        case Instruction::PUSH5:
            eapp_print("PUSH5\n");
            break;
        case Instruction::PUSH6:
            eapp_print("PUSH6\n");
            break;
        case Instruction::PUSH7:
            eapp_print("PUSH7\n");
            break;
        case Instruction::PUSH8:
            eapp_print("PUSH8\n");
            break;
        case Instruction::PUSH9:
            eapp_print("PUSH9\n");
            break;
        case Instruction::PUSH10:
            eapp_print("PUSH10\n");
            break;
        case Instruction::PUSH11:
            eapp_print("PUSH11\n");
            break;
        case Instruction::PUSH12:
            eapp_print("PUSH12\n");
            break;
        case Instruction::PUSH13:
            eapp_print("PUSH13\n");
            break;
        case Instruction::PUSH14:
            eapp_print("PUSH14\n");
            break;
        case Instruction::PUSH15:
            eapp_print("PUSH15\n");
            break;
        case Instruction::PUSH16:
            eapp_print("PUSH16\n");
            break;
        case Instruction::PUSH17:
            eapp_print("PUSH17\n");
            break;
        case Instruction::PUSH18:
            eapp_print("PUSH18\n");
            break;
        case Instruction::PUSH19:
            eapp_print("PUSH19\n");
            break;
        case Instruction::PUSH20:
            eapp_print("PUSH20\n");
            break;
        case Instruction::PUSH21:
            eapp_print("PUSH21\n");
            break;
        case Instruction::PUSH22:
            eapp_print("PUSH22\n");
            break;
        case Instruction::PUSH23:
            eapp_print("PUSH23\n");
            break;
        case Instruction::PUSH24:
            eapp_print("PUSH24\n");
            break;
        case Instruction::PUSH25:
            eapp_print("PUSH25\n");
            break;
        case Instruction::PUSH26:
            eapp_print("PUSH26\n");
            break;
        case Instruction::PUSH27:
            eapp_print("PUSH27\n");
            break;
        case Instruction::PUSH28:
            eapp_print("PUSH28\n");
            break;
        case Instruction::PUSH29:
            eapp_print("PUSH29\n");
            break;
        case Instruction::PUSH30:
            eapp_print("PUSH30\n");
            break;
        case Instruction::PUSH31:
            eapp_print("PUSH31\n");
            break;
        case Instruction::PUSH32:
            eapp_print("PUSH32\n");
            break;
        case Instruction::POP:
            eapp_print("POP\n");
            break;
        case Instruction::SWAP1:
            eapp_print("SWAP1\n");
            break;
        case Instruction::SWAP2:
            eapp_print("SWAP2\n");
            break;
        case Instruction::SWAP3:
            eapp_print("SWAP3\n");
            break;
        case Instruction::SWAP4:
            eapp_print("SWAP4\n");
            break;
        case Instruction::SWAP5:
            eapp_print("SWAP5\n");
            break;
        case Instruction::SWAP6:
            eapp_print("SWAP6\n");
            break;
        case Instruction::SWAP7:
            eapp_print("SWAP7\n");
            break;
        case Instruction::SWAP8:
            eapp_print("SWAP8\n");
            break;
        case Instruction::SWAP9:
            eapp_print("SWAP9\n");
            break;
        case Instruction::SWAP10:
            eapp_print("SWAP10\n");
            break;
        case Instruction::SWAP11:
            eapp_print("SWAP11\n");
            break;
        case Instruction::SWAP12:
            eapp_print("SWAP12\n");
            break;
        case Instruction::SWAP13:
            eapp_print("SWAP13\n");
            break;
        case Instruction::SWAP14:
            eapp_print("SWAP14\n");
            break;
        case Instruction::SWAP15:
            eapp_print("SWAP15\n");
            break;
        case Instruction::SWAP16:
            eapp_print("SWAP16\n");
            break;
        case Instruction::DUP1:
            eapp_print("DUP1\n");
            break;
        case Instruction::DUP2:
            eapp_print("DUP2\n");
            break;
        case Instruction::DUP3:
            eapp_print("DUP3\n");
            break;
        case Instruction::DUP4:
            eapp_print("DUP4\n");
            break;
        case Instruction::DUP5:
            eapp_print("DUP5\n");
            break;
        case Instruction::DUP6:
            eapp_print("DUP6\n");
            break;
        case Instruction::DUP7:
            eapp_print("DUP7\n");
            break;
        case Instruction::DUP8:
            eapp_print("DUP8\n");
            break;
        case Instruction::DUP9:
            eapp_print("DUP9\n");
            break;
        case Instruction::DUP10:
            eapp_print("DUP10\n");
            break;
        case Instruction::DUP11:
            eapp_print("DUP11\n");
            break;
        case Instruction::DUP12:
            eapp_print("DUP12\n");
            break;
        case Instruction::DUP13:
            eapp_print("DUP13\n");
            break;
        case Instruction::DUP14:
            eapp_print("DUP14\n");
            break;
        case Instruction::DUP15:
            eapp_print("DUP15\n");
            break;
        case Instruction::DUP16:
            eapp_print("DUP16\n");
            break;
        case Instruction::LOG0:
            eapp_print("LOG0\n");
            break;
        case Instruction::LOG1:
            eapp_print("LOG1\n");
            break;
        case Instruction::LOG2:
            eapp_print("LOG2\n");
            break;
        case Instruction::LOG3:
            eapp_print("LOG3\n");
            break;
        case Instruction::LOG4:
            eapp_print("LOG4\n");
            break;
        case Instruction::ADD:
            eapp_print("ADD\n");
            break;
        case Instruction::MUL:
            eapp_print("MUL\n");
            break;
        case Instruction::SUB:
            eapp_print("SUB\n");
            break;
        case Instruction::DIV:
            eapp_print("DIV\n");
            break;
        case Instruction::SDIV:
            eapp_print("SDIV\n");
            break;
        case Instruction::MOD:
            eapp_print("MOD\n");
            break;
        case Instruction::SMOD:
            eapp_print("SMOD\n");
            break;
        case Instruction::ADDMOD:
            eapp_print("ADDMOD\n");
            break;
        case Instruction::MULMOD:
            eapp_print("MULMOD\n");
            break;
        case Instruction::EXP:
            eapp_print("EXP\n");
            break;
        case Instruction::SIGNEXTEND:
            eapp_print("SIGNEXTEND\n");
            break;
        case Instruction::LT:
            eapp_print("LT\n");
            break;
        case Instruction::GT:
            eapp_print("GT\n");
            break;
        case Instruction::SLT:
            eapp_print("SLT\n");
            break;
        case Instruction::SGT:
            eapp_print("SGT\n");
            break;
        case Instruction::EQ:
            eapp_print("EQ\n");
            break;
        case Instruction::ISZERO:
            eapp_print("ISZERO\n");
            break;
        case Instruction::AND:
            eapp_print("AND\n");
            break;
        case Instruction::OR:
            eapp_print("OR\n");
            break;
        case Instruction::XOR:
            eapp_print("XOR\n");
            break;
        case Instruction::NOT:
            eapp_print("NOT\n");
            break;
        case Instruction::BYTE:
            eapp_print("BYTE\n");
            break;
        case Instruction::JUMP:
            eapp_print("JUMP\n");
            break;
        case Instruction::JUMPI:
            eapp_print("JUMPI\n");
            break;
        case Instruction::PC:
            eapp_print("PC\n");
            break;
        case Instruction::MSIZE:
            eapp_print("MSIZE\n");
            break;
        case Instruction::MLOAD:
            eapp_print("MLOAD\n");
            break;
        case Instruction::MSTORE:
            eapp_print("MSTORE\n");
            break;
        case Instruction::MSTORE8:
            eapp_print("MSTORE8\n");
            break;
        case Instruction::CODESIZE:
            eapp_print("CODESIZE\n");
            break;
        case Instruction::CODECOPY:
            eapp_print("CODECOPY\n");
            break;
        case Instruction::EXTCODESIZE:
            eapp_print("EXTCODESIZE\n");
            break;
        case Instruction::EXTCODECOPY:
            eapp_print("EXTCODECOPY\n");
            break;
        case Instruction::SLOAD:
            eapp_print("SLOAD\n");
            break;
        case Instruction::SSTORE:
            eapp_print("SSTORE\n");
            break;
        case Instruction::ADDRESS:
            eapp_print("ADDRESS\n");
            break;
        case Instruction::BALANCE:
            eapp_print("BALANCE\n");
            break;
        case Instruction::ORIGIN:
            eapp_print("ORIGIN\n");
            break;
        case Instruction::CALLER:
            eapp_print("CALLER\n");
            break;
        case Instruction::CALLVALUE:
            eapp_print("CALLVALUE\n");
            break;
        case Instruction::CALLDATALOAD:
            eapp_print("CALLDATALOAD\n");
            break;
        case Instruction::CALLDATASIZE:
            eapp_print("CALLDATASIZE\n");
            break;
        case Instruction::CALLDATACOPY:
            eapp_print("CALLDATACOPY\n");
            break;
        case Instruction::RETURN:
            eapp_print("RETURN\n");
            break;
        case Instruction::SELFDESTRUCT:
            eapp_print("SELFDESTRUCT\n");
            break;
        case Instruction::CREATE:
            eapp_print("CREATE\n");
            break;
        case Instruction::CALL:
            eapp_print("CALL\n");
            break;
        case Instruction::CALLCODE:
            eapp_print("CALLCODE\n");
            break;
        case Instruction::DELEGATECALL:
            eapp_print("DELEGATECALL\n");
            break;
        case Instruction::JUMPDEST:
            eapp_print("JUMPDEST\n");
            break;
        case Instruction::BLOCKHASH:
            eapp_print("BLOCKHASH\n");
            break;
        case Instruction::NUMBER:
            eapp_print("NUMBER\n");
            break;
        case Instruction::GASPRICE:
            eapp_print("GASPRICE\n");
            break;
        case Instruction::COINBASE:
            eapp_print("COINBASE\n");
            break;
        case Instruction::TIMESTAMP:
            eapp_print("TIMESTAMP\n");
            break;
        case Instruction::DIFFICULTY:
            eapp_print("DIFFICULTY\n");
            break;
        case Instruction::GASLIMIT:
            eapp_print("GASLIMIT\n");
            break;
        case Instruction::GAS:
            eapp_print("GAS\n");
            break;
        case Instruction::SHA3:
            eapp_print("SHA3\n");
            break;
        case Instruction::STOP:
            eapp_print("STOP\n");
            break;
        default:
            eapp_print("wrong op = %d\n", op);
    }
}

EVMResult* EVMProcessor::run(vector<uint8_t> codes, vector<uint8_t> input, uint64_t last_gas) {
    evm_codes = codes;
    evm_input = input;
    evm_last_gas = last_gas;
    evm_jumpdest = get_jumpdest(evm_codes); // 获取合法的跳转地址
    bool pc_incremented = true;
    bool finished = false;
    while (evm_pc < evm_codes.size() && !finished){
        if(if_stopped){
            stop_evm();

            vector<unsigned char> plaintext_vector;
            for(int i = 0; i < evm_context->evm_codes.size(); i++){
                plaintext_vector.push_back(evm_context->evm_codes[i]);
            }
            for(int i = 0; i < evm_context->evm_memory.size(); i++){
                plaintext_vector.push_back(evm_context->evm_memory[i]);
            }
            for(int i = 0; i < evm_context->evm_input.size(); i++){
                plaintext_vector.push_back(evm_context->evm_input[i]);
            }

            unsigned char ciphertext[80000];
            int ciphertext_len;
            encrypt_data(&plaintext_vector[0], plaintext_vector.size(), ciphertext, &ciphertext_len);

            unsigned char plaintext[80000];
            int plaintext_len;
            decrypt_data(ciphertext, ciphertext_len, plaintext, &plaintext_len);

            return nullptr;
        }

        uint8_t op = evm_codes[evm_pc];
        switch (op){
            case Instruction::PUSH1:
            case Instruction::PUSH2:
            case Instruction::PUSH3:
            case Instruction::PUSH4:
            case Instruction::PUSH5:
            case Instruction::PUSH6:
            case Instruction::PUSH7:
            case Instruction::PUSH8:
            case Instruction::PUSH9:
            case Instruction::PUSH10:
            case Instruction::PUSH11:
            case Instruction::PUSH12:
            case Instruction::PUSH13:
            case Instruction::PUSH14:
            case Instruction::PUSH15:
            case Instruction::PUSH16:
            case Instruction::PUSH17:
            case Instruction::PUSH18:
            case Instruction::PUSH19:
            case Instruction::PUSH20:
            case Instruction::PUSH21:
            case Instruction::PUSH22:
            case Instruction::PUSH23:
            case Instruction::PUSH24:
            case Instruction::PUSH25:
            case Instruction::PUSH26:
            case Instruction::PUSH27:
            case Instruction::PUSH28:
            case Instruction::PUSH29:
            case Instruction::PUSH30:
            case Instruction::PUSH31:
            case Instruction::PUSH32:
                push();
                pc_incremented = false;
                break;
            case Instruction::ADD:
                add();
                break;
            case Instruction::MUL:
                mul();
                break;
            case Instruction::SUB:
                sub();
                break;
            case Instruction::DIV:
                div();
                break;
            case Instruction::SDIV:
                sdiv();
                break;
            case Instruction::MOD:
                mod();
                break;
            case Instruction::SMOD:
                smod();
                break;
            case Instruction::ADDMOD:
                addmod();
                break;
            case Instruction::MULMOD:
                mulmod();
                break;
            case Instruction::EXP:
                exp();
                break;
            case Instruction::SIGNEXTEND:
                signextend(); // 将x从(b+1)*8位有符号扩展为256位
                break;
            case Instruction::LT:
                lt();
                break;
            case Instruction::GT:
                gt();
                break;
            case Instruction::SLT:
                slt();
                break;
            case Instruction::SGT:
                sgt();
                break;
            case Instruction::EQ:
                eq();
                break;
            case Instruction::ISZERO:
                iszero();
                break;
            case Instruction::AND:
                and_();
                break;
            case Instruction::OR:
                or_();
                break;
            case Instruction::XOR:
                xor_();
                break;
            case Instruction::NOT:
                not_();
                break;
            case Instruction::BYTE:
                byte(); // 返回x从最高字节开始的第i个字节
                break;
            case Instruction::CALLVALUE:
                callvalue();
                break;
            case Instruction::CALLDATALOAD:
                calldataload();
                break;
            case Instruction::CALLDATASIZE:
                calldatasize();
                break;
            case Instruction::CALLDATACOPY:
                calldatacopy();
                break;
            case Instruction::CODESIZE:
                codesize();
                break;
            case Instruction::CODECOPY:
                codecopy();
                break;
            case Instruction::POP:
                pop();
                break;
            case Instruction::MLOAD:
                mload();
                break;
            case Instruction::MSTORE:
                mstore();
                break;
            case Instruction::MSTORE8:
                mstore8();
                break;
            case Instruction::SLOAD:
                sload();
                break;
            case Instruction::SSTORE:
                sstore();
                break;
            case Instruction::JUMP:
                pc_incremented = jump();
                break;
            case Instruction::JUMPI:
                pc_incremented = jumpi();
                break;
            case Instruction::PC:
                pc();
                break;
            case Instruction::MSIZE:
                msize();
                break;
            case Instruction::JUMPDEST:
                jumpdest();
                break;
            case Instruction::DUP1:
            case Instruction::DUP2:
            case Instruction::DUP3:
            case Instruction::DUP4:
            case Instruction::DUP5:
            case Instruction::DUP6:
            case Instruction::DUP7:
            case Instruction::DUP8:
            case Instruction::DUP9:
            case Instruction::DUP10:
            case Instruction::DUP11:
            case Instruction::DUP12:
            case Instruction::DUP13:
            case Instruction::DUP14:
            case Instruction::DUP15:
            case Instruction::DUP16:
                dup();
                break;
            case Instruction::SWAP1:
            case Instruction::SWAP2:
            case Instruction::SWAP3:
            case Instruction::SWAP4:
            case Instruction::SWAP5:
            case Instruction::SWAP6:
            case Instruction::SWAP7:
            case Instruction::SWAP8:
            case Instruction::SWAP9:
            case Instruction::SWAP10:
            case Instruction::SWAP11:
            case Instruction::SWAP12:
            case Instruction::SWAP13:
            case Instruction::SWAP14:
            case Instruction::SWAP15:
            case Instruction::SWAP16:
                swap();
                break;
            case Instruction::LOG0:
            case Instruction::LOG1:
            case Instruction::LOG2:
            case Instruction::LOG3:
            case Instruction::LOG4:
                log();
                break;
            case Instruction::RETURN:
                return_();
                finished = true;
                break;
        }
        if(!pc_incremented){
            pc_incremented = true;
        }else{
            evm_pc++;
        }
        update_mem_gas();
    }
    return evm_result;
}

void EVMProcessor::stop_evm(){
    evm_context = new EVMContext(evm_stack, evm_codes, evm_jumpdest,
                                 evm_input, evm_memory, evm_storage,
                                 evm_pc, evm_value, evm_last_gas, evm_result);
}

EVMResult* EVMProcessor::resume_evm(){
    return run(evm_codes, evm_input, evm_last_gas);
}

EVMContext* EVMProcessor::get_context(){
    return evm_context;
}