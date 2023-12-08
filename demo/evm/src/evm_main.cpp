#include <vector>
#include <string>
#include "../include/processor.h"

extern "C" void eapp_print(const char*s, ...);
extern "C" int evm_main(uint8_t* evm_code, uint64_t evm_code_size,
                        uint8_t* evm_param, uint8_t evm_param_size,
                        uint8_t* evm_output, uint8_t* evm_output_size,
                        uint8_t* evm_log, uint64_t* evm_log_size,
                        uint8_t* evm_value, uint64_t evm_gas,
                        uint64_t* evm_last_gas);

int evm_main(uint8_t* evm_code, uint64_t evm_code_size,
             uint8_t* evm_param, uint8_t evm_param_size,
             uint8_t* evm_output, uint8_t* evm_output_size,
             uint8_t* evm_log, uint64_t* evm_log_size,
             uint8_t* evm_value, uint64_t evm_gas,
             uint64_t* evm_last_gas) {
    std::vector<uint8_t> codes;
    for(int i = 0; i < evm_code_size; i++){
        codes.push_back(evm_code[i]);
    }
    std::vector<uint8_t> params;
    for(int i = 0; i < evm_param_size; i++){
        params.push_back(evm_param[i]);
    }
    uint256_t value = uint8_vector_to_uint256(evm_value, 32);

    EVMProcessor* processor = new EVMProcessor(value);
    EVMResult* result = processor->run(codes, params, evm_gas);

    if(result == nullptr){
        EVMProcessor* processor2 = new EVMProcessor(processor->get_context());
        result = processor2->resume_evm();
    }

    for(int i = 0; i < result->output.size(); i++){
        evm_output[i] = result->output[i];
    }
    *evm_output_size = result->output.size();
    for(int i = 0; i < result->log.size(); i++){
        evm_log[i] = result->log[i];
    }
    *evm_log_size = result->log.size();
    *evm_last_gas = result->evm_last_gas;
    eapp_print("evm_log_size = %d\n", (int)result->log.size());

    return 0;
}