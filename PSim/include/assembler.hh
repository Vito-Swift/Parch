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
#include <fstream>

#include "psim.hh"
#include "utils.hh"


struct Assembler {
    std::string ELF_path;
    std::vector<std::string> filein;
    std::vector<uint32_t> bin;
};

void init_assembler(Simulator *simulator, std::string ELF_path);

void exec_assembler(Simulator *as);

void free_assembler(Assembler *as);

#endif //PARCH_ASSEMBLER_HH
