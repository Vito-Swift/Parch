/**
 * @filename: assembler.cc.c
 * @author: Vito Wu <chenhaowu[at]link.cuhk.edu.cn>
 * @version: 
 * @desc:
 * @date: 2/28/2021
 */

#include "assembler.hh"

uint32_t __encode_rtype(const tokens_t &tokens) {
}

uint32_t __encode_jtype(const tokens_t &tokens) {

}

uint32_t __encode_itype(const tokens_t &tokens) {

}

tokens_t tokenize_str(const std::string &line) {
    char delimiters[] = "\t ,";
    char *token;

    uint32_t n = line.length();
    char line_str[n + 1];
    strcpy(line_str, line.c_str());

    token = strtok(line_str, delimiters);
    tokens_t tokens;

    while (token != NULL) {
        tokens.push_back(token_t(token));
        token = strtok(NULL, delimiters);
    }

    return tokens;
}

bool encode(const tokens_t &tokens, uint32_t* bin) {
    token_t opcode_string = tokens[0];
    switch (hash(opcode_string.c_str())) {

        case hash("add"):
            break;

        case hash("addu"):
            break;

        case hash("addi"):
            break;

        case hash("addiu"):
            break;

        case hash("and"):
            break;

        case hash("andi"):
            break;

        case hash("clo"):
            break;

        case hash("clz"):
            break;

        case hash("div"):
            break;

        case hash("divu"):
            break;

        default:
            return 0;

    }

    return 1;
}


bool isLineALabel(const std::string &line) {
    std::regex e("(.*):$");
    return std::regex_match(line, e);
}

bool __assembler_exec(Assembler *assembler) {
    bool inText = false, inData = false;
    for (std::string line: assembler->content) {
        // remove leading and trailing spaces
        line = std::regex_replace(line, std::regex("^[ \t]+"), "");
        line = std::regex_replace(line, std::regex("[ \t]+$"), "");

        switch (hash(line.c_str())) {

            case hash(".text"):
                PRINTF_DEBUG_VERBOSE(verbose, "[ASM]\t[DT]\t\t%s\n", line.c_str());
                inText = true;
                inData = false;
                break;

            case hash(".data"):
                PRINTF_DEBUG_VERBOSE(verbose, "[ASM]\t[DD]\t\t%s\n", line.c_str());
                inText = false;
                inData = true;
                break;

            default:
                break;

        }

        if (inText) {
            if (isLineALabel(line)) {
                std::string label = line.substr(0, line.find(":"));
                PRINTF_DEBUG_VERBOSE(verbose, "[ASM]\t[LABEL]\t\t%s\n", label.c_str());

                // do something here

            } else {
                tokens_t tokens = tokenize_str(line);
                uint32_t bin_line;

                if (!encode(tokens, &bin_line)) {
                    return 0;
                }
            }
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