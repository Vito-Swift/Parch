/**
 * @filename: assembler.hh.h
 * @author: Vito Wu <chenhaowu[at]link.cuhk.edu.cn>
 * @version: 
 * @desc:
 * @date: 2/28/2021
 */

#ifndef PARCH_ASSEMBLER_HH
#define PARCH_ASSEMBLER_HH

#include <string>
#include <vector>


struct assembler {
    std::string ELF_path;
    std::vector<std::string> filein;
    std::vector<uint32_t> bin;
};

void init_assembler(assembler *as, std::string ELF_path);

void exec_assembler(assembler *as);

void free_assembler(assembler *as);

#endif //PARCH_ASSEMBLER_HH
