/**
 * @filename: assembler.cc.c
 * @author: Vito Wu <chenhaowu[at]link.cuhk.edu.cn>
 * @version: 
 * @desc:
 * @date: 2/28/2021
 */

#include "assembler.hh"

bool isLineALabel(const std::string &line) {
    std::regex e("(.*):$");
    return std::regex_match(line, e);
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

uint32_t __encode_reg(const token_t &reg_str) {
    token_t reg_key = reg_str;
    reg_key.erase(std::remove(reg_key.begin(), reg_key.end(), '$'), reg_key.end());
    return reg_map[reg_key];
}

uint32_t __encode_rtype(const tokens_t &tokens,
                        const uint32_t opcode,
                        const uint32_t shamt,
                        const uint32_t funct) {
    uint32_t rs = __encode_reg(tokens[1]);
    uint32_t rt = __encode_reg(tokens[2]);
    uint32_t rd = __encode_reg(tokens[3]);

    return (opcode << 26) |
           (rs << 21) |
           (rt << 16) |
           (rd << 11) |
           (shamt << 6) |
           funct;
}

uint32_t __encode_jtype(const tokens_t &tokens, const uint32_t opcode) {
    return 0;
}

uint32_t __encode_itype(const tokens_t &tokens, const uint32_t opcode, const uint32_t immediate) {
    uint32_t rs = __encode_reg(tokens[1]);
    uint32_t rt = __encode_reg(tokens[2]);

    return (opcode << 26) |
           (rs << 21) |
           (rt << 16) |
           immediate;
}

bool encode(const tokens_t &tokens, uint32_t *bin) {
    token_t opcode_string = tokens[0];
    switch (hash(opcode_string.c_str())) {

        case hash("add"):
            *bin = __encode_rtype(tokens, 0x0, 0x0, 0x20);
            break;

        case hash("addu"):
            *bin = __encode_rtype(tokens, 0x0, 0x0, 0x21);
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

        case hash("mult"):
            break;

        case hash("madd"):
            break;

        case hash("msub"):
            break;

        case hash("maddu"):
            break;

        case hash("msubu"):
            break;

        case hash("nor"):
            break;

        case hash("or"):
            break;

        case hash("ori"):
            break;

        case hash("sll"):
            break;

        case hash("sllv"):
            break;

        case hash("sra"):
            break;

        case hash("srav"):
            break;

        case hash("srl"):
            break;

        case hash("srlv"):
            break;

        case hash("sub"):
            break;

        case hash("subu"):
            break;

        case hash("xor"):
            break;

        case hash("xori"):
            break;

        case hash("lui"):
            break;

        case hash("slt"):
            break;

        case hash("sltu"):
            break;

        case hash("slti"):
            break;

        case hash("sltiu"):
            break;

        case hash("beq"):
            break;

        case hash("bgez"):
            break;

        case hash("bgezal"):
            break;

        case hash("bgtz"):
            break;

        case hash("blez"):
            break;

        case hash("bltzal"):
            break;

        case hash("bltz"):
            break;

        case hash("bne"):
            break;

        case hash("j"):
            break;

        case hash("jal"):
            break;

        case hash("jalr"):
            break;

        case hash("jr"):
            break;

        default:
            return 0;

    }

    return 1;
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

    PRINTF_DEBUG_VERBOSE(verbose, "[ASM]\tInitialize register map\n");
    reg_map = create_regparse_map();
}