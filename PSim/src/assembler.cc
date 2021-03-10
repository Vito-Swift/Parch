/**
 * @filename: assembler.cc.c
 * @author: Vito Wu <chenhaowu[at]link.cuhk.edu.cn>
 * @version: 
 * @desc:
 * @date: 2/28/2021
 */

#include "assembler.hh"

bool __assembler_exec();

bool __parse_file(Assembler *assembler) {
    std::ifstream ELF_file(assembler->ELF_path);
    std::string line;

    bool inText;
    bool inData;
    while (std::getline(ELF_file, line)) {
        PRINTF_DEBUG_VERBOSE(verbose, "%s", line.c_str());
    }

    return 1;
}

void parse_file(Assembler *assembler) {
    if (!isFileExist(assembler->ELF_path)) {
        EXIT_WITH_MSG("Specified ELF file does not exist");
    }

    if (!__parse_file(assembler)) {
        EXIT_WITH_MSG("Parse file failed");
    }
}

void assembler_init(Assembler *assembler, std::string ELF_path, bool loadFromELF) {
    if (loadFromELF) {
        PRINTF_DEBUG_VERBOSE(verbose,
                             "Read from file: %s\n",
                             assembler->ELF_path.c_str());

        assembler->ELF_path = ELF_path;
        parse_file(assembler);
    } else {

    }
}