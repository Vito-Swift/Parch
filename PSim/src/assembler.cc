/**
 * @filename: assembler.cc.c
 * @author: Vito Wu <chenhaowu[at]link.cuhk.edu.cn>
 * @version: 
 * @desc:
 * @date: 2/28/2021
 */

#include "assembler.hh"


bool __parse_file(Assembler *as) {
    std::ifstream ELF_file(as->ELF_path);
    std::string line;

    bool inText;
    bool inData;
    while (std::getline(ELF_file, line)) {
        printf("%s", line.c_str());
    }
    return 1;
}

void parse_file(Assembler *as) {
    if (!isFileExist(as->ELF_path)) {
        PRINTF_ERR_STAMP("Specified ELF file does not exist");
        exit(0);
    }

    if (!__parse_file(as)) {
        PRINTF_ERR_STAMP("Parse file failed");
        exit(0);
    }
}

void init_assembler(Assembler *as, std::string ELF_path) {
    as->ELF_path = ELF_path;
    parse_file(as);
}