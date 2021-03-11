/**
 * @filename: assembler.cc.c
 * @author: Vito Wu <chenhaowu[at]link.cuhk.edu.cn>
 * @version: 
 * @desc:
 * @date: 2/28/2021
 */

#include "assembler.hh"

constexpr unsigned int hash(const char *s, int off = 0) {
    return !s[off] ? 5381 : (hash(s, off + 1) * 33) ^ s[off];
}

uint32_t encode_rtype(uint32_t opcode,
                      uint32_t rs,
                      uint32_t rt,
                      uint32_t rd,
                      uint32_t shamt,
                      uint32_t funct) {
    return (opcode << 26) |
           (rs << 21) |
           (rt << 16) |
           (rd << 11) |
           (shamt << 6) |
           funct;
}

uint32_t encode_jtype(uint8_t opcode,
                      uint8_t rs,
                      uint8_t rt,
                      uint16_t imme) {

}

uint32_t encode_itype(uint8_t opcode,
                      uint32_t addr) {

}

bool __assembler_exec(Assembler *assembler) {
    bool inText = false, inData = false;
    for (std::string line: assembler->content) {
        // remove leading and trailing spaces
        line = std::regex_replace(line, std::regex("^[ \t]+"), "");
        printf("%s\n", line.c_str());
        line = std::regex_replace(line, std::regex("[ \t]+$"), "");
        printf("%s\n", line.c_str());

        switch (hash(line.c_str())) {

            case hash(".text"):
                PRINTF_DEBUG_VERBOSE(verbose, "[ASM]\t\t[DT]\t\t%s\n", line.c_str());
                inText = true;
                inData = false;
                break;

            case hash(".data"):
                PRINTF_DEBUG_VERBOSE(verbose, "[ASM]\t\t[DD]\t\t%s\n", line.c_str());
                inText = false;
                inData = true;
                break;

            default:
                break;

        }

        if (inText) {
            std::string opstring = line.substr(0, line.find(' '));
        }

        if (inData) {

        }
    }

    return 1;
}

void assembler_exec(Assembler *assembler) {
    PRINTF_DEBUG_VERBOSE(verbose, "[ASM]\tStart to assembling input content:\n");
    if (!__assembler_exec(assembler)) {
        EXIT_WITH_MSG("[!] assembling failed, exit...\n");
    }
    PRINTF_DEBUG_VERBOSE(verbose, "[ASM]\tAssembling finished.\n");
}

bool __parse_file(Assembler *assembler) {
    std::ifstream ELF_file(assembler->ELF_path);
    std::string line;

    while (std::getline(ELF_file, line)) {
        PRINTF_DEBUG_VERBOSE(verbose, "[ASM]\t[ELF]\t\t%s\n", line.c_str());
        assembler->content.push_back(line);
    }

    return 1;
}

void parse_file(Assembler *assembler) {
    if (!isFileExist(assembler->ELF_path)) {
        EXIT_WITH_MSG("[!] specified ELF file does not exist, please check...\n");
    }

    if (!__parse_file(assembler)) {
        EXIT_WITH_MSG("[!] parse file failed, please verify the file is a valid MIPS asm file...\n");
    }
}

void assembler_init(Assembler *assembler, std::string ELF_path, bool loadFromELF) {
    if (loadFromELF) {
        PRINTF_DEBUG_VERBOSE(verbose,
                             "[ASM]\tRead from file: %s\n",
                             assembler->ELF_path.c_str());

        assembler->ELF_path = ELF_path;
        parse_file(assembler);
    } else {

    }
}