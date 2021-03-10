/**
 * @filename: assembler.cc.c
 * @author: Vito Wu <chenhaowu[at]link.cuhk.edu.cn>
 * @version: 
 * @desc:
 * @date: 2/28/2021
 */

#include "assembler.hh"

bool __exec_assembler();

bool __parse_file(Simulator *simulator) {
    Assembler* as = &(simulator->assembler);
    std::ifstream ELF_file(as->ELF_path);
    std::string line;

    bool inText;
    bool inData;
    while (std::getline(ELF_file, line)) {
        PRINTF_DEBUG_VERBOSE(simulator, "%s", line.c_str());
    }

    return 1;
}

void parse_file(Simulator *simulator) {
    PRINTF_DEBUG_VERBOSE(simulator,
                         "Read from file: %s\n",
                         simulator->assembler->ELF_path);

    if (!isFileExist(simulator->assembler->ELF_path)) {
        EXIT_WITH_MSG("Specified ELF file does not exist");
    }

    if (!__parse_file(simulator->assembler)) {
        EXIT_WITH_MSG("Parse file failed");
    }
}

void init_assembler(Simulator *simulator, std::string ELF_path) {
    simulator->assembler->ELF_path = ELF_path;
    parse_file(simulator);
}