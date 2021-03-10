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
#include <sstream>
#include <regex>

#include "utils.hh"
#include "options.hh"
#include "opcode.h"

struct Assembler {
    std::string ELF_path;
    std::vector<std::string> content;
    std::vector<std::string> text_section;
    std::vector<std::string> data_section;
    std::vector<uint32_t> bin;
};

void assembler_init(Assembler *assembler, std::string ELF_path, bool loadFromELF);

void assembler_exec(Assembler *assembler);

void assembler_free(Assembler *assembler);

#endif //PARCH_ASSEMBLER_HH
