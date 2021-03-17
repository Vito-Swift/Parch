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
#include <string.h>
#include <map>
#include <bitset>

#include "utils.hh"
#include "options.hh"
#include "opcode.h"
#include "register.hh"

struct Assembler {
    std::string ELF_path;
    std::vector<std::string> content;
    std::vector<std::string> text_section;
    std::vector<uint32_t> bin;
    std::map<std::string, uint32_t> label_map;
};

void assembler_init(Assembler *assembler, std::string ELF_path, bool loadFromELF);

void assembler_exec(Assembler *assembler);

void assembler_free(Assembler *assembler);

typedef std::vector<std::string> tokens_t;
typedef std::string token_t;

#endif //PARCH_ASSEMBLER_HH
