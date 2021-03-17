/**
 * @filename: assembler.cc.c
 * @author: Vito Wu <chenhaowu[at]link.cuhk.edu.cn>
 * @version: 
 * @desc:
 * @date: 2/28/2021
 */

#include "assembler.hh"
#include <iostream>

std::map<std::string, uint32_t> reg_map;

bool isLineALabel(const std::string &line) {
    std::regex e("(.*):$");
    return std::regex_match(line, e);
}

std::string removeComments(const std::string &line) {
    std::string ret = line.substr(0, line.find("#"));
    return ret;
}

bool isLineEmpty(const std::string &line) {
    for (uint32_t index = 0; index < line.length(); index++) {
        if (!std::isspace(line[index]))
            return false;
    }
    return true;
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

void __encode_address(const token_t &addr_str, uint32_t *rt, uint32_t *offset) {
    std::regex rt_rgx("\\d+\\(\\$(\\w\\d)\\)");
    std::regex offset_rgx("(\\d+)\\(\\$\\w\\d\\)");
    std::smatch match;

    // search for offset in the token
    std::regex_search(addr_str, match, offset_rgx);
    *offset = std::stoi(match[1]);

    // search for rt register in the token
    std::regex_search(addr_str, match, rt_rgx);
    *rt = __encode_reg(match[1]);
}

std::string __encode_label(Assembler *assembler, const token_t &label_str) {
    if (assembler->label_map.find(label_str) == assembler->label_map.end())
        EXIT_WITH_MSG("Assembly contains not defined label: %s\n", label_str.c_str());
    std::stringstream sstream;
    sstream << std::hex << assembler->label_map[label_str];
    return sstream.str();
}

uint32_t __encode_rtype(const tokens_t &tokens,
                        const uint32_t opcode,
                        const uint32_t funct) {

    // Encode format:
    // 0                   1                   2                   3
    // 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
    //+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
    //|   opcode  |    rs   |    rt   |    rd   |  shamt  |   funct   |
    //+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

    uint32_t rs = __encode_reg(tokens[1]);
    uint32_t rt = __encode_reg(tokens[2]);
    uint32_t rd = __encode_reg(tokens[3]);
    uint32_t shamt = std::stoi(tokens[4]);
    uint32_t bin = (opcode << 26) | (rs << 21) | (rt << 16) |
                   (rd << 11) | (shamt << 6) | funct;
    PRINTF_DEBUG_VERBOSE(verbose,
                         "[ASM]\t[ENCODE]\tInstruction: %s  %s,%s,%s [%d]"
                         "\t\t----->\t\t%s\n",
                         tokens[0].c_str(),
                         tokens[1].c_str(),
                         tokens[2].c_str(),
                         tokens[3].c_str(),
                         shamt,
                         std::bitset<32>(bin).to_string().c_str());

    return bin;
}

uint32_t __encode_jtype(const tokens_t &tokens, const uint32_t opcode) {
    // Encode format:
    // 0                   1                   2                   3
    // 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
    //+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
    //|   opcode  |                      address                      |
    //+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
    uint32_t address = std::stoi(tokens[1]);
    uint32_t bin = (opcode << 26) | address;
    PRINTF_DEBUG_VERBOSE(verbose,
                         "[ASM]\t[ENCODE]\tInstruction: %s  %s"
                         "\t\t----->\t\t%s\n",
                         tokens[0].c_str(),
                         tokens[1].c_str(),
                         std::bitset<32>(bin).to_string().c_str());
    return bin;
}

uint32_t __encode_itype(const tokens_t &tokens, const uint32_t opcode) {

    // Encode format:
    // 0                   1                   2                   3
    // 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
    //+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
    //|   opcode  |    rs   |    rt   |              imm              |
    //+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

    uint32_t rs = __encode_reg(tokens[1]);
    uint32_t rt = __encode_reg(tokens[2]);
    uint32_t immediate = std::stoi(tokens[3]);
    uint32_t bin = (opcode << 26) | (rs << 21) | (rt << 16) | immediate;
    PRINTF_DEBUG_VERBOSE(verbose,
                         "[ASM]\t[ENCODE]\tInstruction: %s  %s,%s"
                         "\t\t----->\t\t%s\n",
                         tokens[0].c_str(),
                         tokens[1].c_str(),
                         tokens[2].c_str(),
                         std::bitset<32>(bin).to_string().c_str());
    return bin;
}

uint32_t __encode_ls(const tokens_t &tokens, const uint32_t opcode) {
    uint32_t rt, offset;
    __encode_address(tokens[2], &rt, &offset);
    tokens_t _tokens{tokens[0], tokens[1], std::to_string(rt), std::to_string(offset)};
    return __encode_itype(_tokens, opcode);
}

bool encode(Assembler *assembler, tokens_t &tokens, uint32_t *bin) {
    token_t opcode_string = tokens[0];
    switch (hash(opcode_string.c_str())) {

        case hash("add"):
            tokens.push_back("0x0");
            *bin = __encode_rtype(tokens, 0x0, 0x20);
            break;

        case hash("addu"):
            tokens.push_back("0x0");
            *bin = __encode_rtype(tokens, 0x0, 0x21);
            break;

        case hash("addi"):
            *bin = __encode_itype(tokens, 0x8);
            break;

        case hash("addiu"):
            *bin = __encode_itype(tokens, 0x9);
            break;

        case hash("and"):
            tokens.push_back("0x0");
            *bin = __encode_rtype(tokens, 0x0, 0x24);
            break;

        case hash("andi"):
            *bin = __encode_itype(tokens, 0xc);
            break;

        case hash("clo"):
            tokens.push_back("0x0");
            *bin = __encode_rtype(tokens, 0x1c, 0x21);
            break;

        case hash("clz"):
            tokens.push_back("0x0");
            *bin = __encode_rtype(tokens, 0x1c, 0x20);
            break;

        case hash("div"):
            tokens.push_back(token_t("0x1a"));
            *bin = __encode_itype(tokens, 0x0);
            break;

        case hash("divu"):
            tokens.push_back(token_t("0x1b"));
            *bin = __encode_itype(tokens, 0x0);
            break;

        case hash("mult"):
            tokens.push_back(token_t("0x18"));
            *bin = __encode_itype(tokens, 0x0);
            break;

        case hash("madd"):
            tokens.push_back(token_t("0x0"));
            *bin = __encode_itype(tokens, 0x1c);
            break;

        case hash("msub"):
            tokens.push_back(token_t("0x4"));
            *bin = __encode_itype(tokens, 0x1c);
            break;

        case hash("maddu"):
            tokens.push_back(token_t("0x1"));
            *bin = __encode_itype(tokens, 0x1c);
            break;

        case hash("msubu"):
            tokens.push_back(token_t("0x5"));
            *bin = __encode_itype(tokens, 0x1c);
            break;

        case hash("nor"):
            tokens.push_back("0x0");
            *bin = __encode_rtype(tokens, 0x0, 0x27);
            break;

        case hash("or"):
            tokens.push_back("0x0");
            *bin = __encode_rtype(tokens, 0x0, 0x25);
            break;

        case hash("ori"):
            *bin = __encode_itype(tokens, 0xd);
            break;

        case hash("sll"):
            *bin = __encode_rtype(tokens, 0x0, 0x0);
            break;

        case hash("sllv"):
            tokens.push_back("0x0");
            *bin = __encode_rtype(tokens, 0x0, 0x4);
            break;

        case hash("sra"):
            *bin = __encode_rtype(tokens, 0x0, 0x3);
            break;

        case hash("srav"):
            tokens.push_back("0x0");
            *bin = __encode_rtype(tokens, 0x0, 0x7);
            break;

        case hash("srl"):
            *bin = __encode_rtype(tokens, 0x0, 0x2);
            break;

        case hash("srlv"):
            tokens.push_back("0x0");
            *bin = __encode_rtype(tokens, 0x0, 0x6);
            break;

        case hash("sub"):
            tokens.push_back("0x0");
            *bin = __encode_rtype(tokens, 0x0, 0x22);
            break;

        case hash("subu"):
            tokens.push_back("0x0");
            *bin = __encode_rtype(tokens, 0x0, 0x23);
            break;

        case hash("xor"):
            tokens.push_back("0x0");
            *bin = __encode_rtype(tokens, 0x0, 0x26);
            break;

        case hash("xori"):
            *bin = __encode_itype(tokens, 0xe);
            break;

        case hash("lui"):
            tokens.insert(tokens.begin() + 1, "0x0");
            *bin = __encode_itype(tokens, 0xf);
            break;

        case hash("slt"):
            tokens.push_back("0x0");
            *bin = __encode_rtype(tokens, 0x0, 0x2a);
            break;

        case hash("sltu"):
            tokens.push_back("0x0");
            *bin = __encode_rtype(tokens, 0x0, 0x2b);
            break;

        case hash("slti"):
            *bin = __encode_itype(tokens, 0xa);
            break;

        case hash("sltiu"):
            *bin = __encode_itype(tokens, 0xb);
            break;

        case hash("beq"):
            tokens[3] = __encode_label(assembler, tokens[3]);
            *bin = __encode_itype(tokens, 0x4);
            break;

        case hash("bgez"):
            tokens.insert(tokens.begin() + 2, "0x1");
            tokens[3] = __encode_label(assembler, tokens[3]);
            *bin = __encode_itype(tokens, 0x1);
            break;

        case hash("bgezal"):
            tokens.insert(tokens.begin() + 2, "0x11");
            tokens[3] = __encode_label(assembler, tokens[3]);
            *bin = __encode_itype(tokens, 0x1);
            break;

        case hash("bgtz"):
            tokens.insert(tokens.begin() + 2, "0x0");
            tokens[3] = __encode_label(assembler, tokens[3]);
            *bin = __encode_itype(tokens, 0x7);
            break;

        case hash("blez"):
            tokens.insert(tokens.begin() + 2, "0x0");
            tokens[3] = __encode_label(assembler, tokens[3]);
            *bin = __encode_itype(tokens, 0x6);
            break;

        case hash("bltzal"):
            tokens.insert(tokens.begin() + 2, "0x10");
            tokens[3] = __encode_label(assembler, tokens[3]);
            *bin = __encode_itype(tokens, 0x1);
            break;

        case hash("bltz"):
            tokens.insert(tokens.begin() + 2, "0x0");
            tokens[3] = __encode_label(assembler, tokens[3]);
            *bin = __encode_itype(tokens, 0x1);
            break;

        case hash("bne"):
            tokens[3] = __encode_label(assembler, tokens[3]);
            *bin = __encode_itype(tokens, 0x5);
            break;

        case hash("j"):
            tokens[1] = __encode_label(assembler, tokens[1]);
            *bin = __encode_jtype(tokens, 0x2);
            break;

        case hash("jal"):
            tokens[1] = __encode_label(assembler, tokens[1]);
            *bin = __encode_jtype(tokens, 0x3);
            break;

        case hash("jalr"):
            tokens.insert(tokens.begin() + 2, "0x0");
            tokens.insert(tokens.begin() + 4, "0x0");
            *bin = __encode_jtype(tokens, 0x9);
            break;

        case hash("jr"):
            tokens.push_back("0x0");
            tokens.push_back("0x8");
            *bin = __encode_itype(tokens, 0x0);
            break;

        case hash("teq"):
            tokens.push_back("0x0");
            *bin = __encode_rtype(tokens, 0x0, 0x34);
            break;

        case hash("teqi"):
            tokens.insert(tokens.begin() + 2, "0xc");
            *bin = __encode_itype(tokens, 0x1);
            break;

        case hash("tne"):
            tokens.push_back("0x0");
            *bin = __encode_rtype(tokens, 0x0, 0x36);
            break;

        case hash("tnei"):
            tokens.insert(tokens.begin() + 2, "0xe");
            *bin = __encode_itype(tokens, 0x1);
            break;

        case hash("tge"):
            tokens.push_back("0x0");
            *bin = __encode_rtype(tokens, 0x0, 0x30);
            break;

        case hash("tgeu"):
            tokens.push_back("0x0");
            *bin = __encode_rtype(tokens, 0x0, 0x31);
            break;

        case hash("tgei"):
            tokens.insert(tokens.begin() + 2, "0x8");
            *bin = __encode_itype(tokens, 0x1);
            break;

        case hash("tgeiu"):
            tokens.insert(tokens.begin() + 2, "0x9");
            *bin = __encode_itype(tokens, 0x1);
            break;

        case hash("tlt"):
            tokens.push_back("0x0");
            *bin = __encode_rtype(tokens, 0x0, 0x32);
            break;

        case hash("tltu"):
            tokens.push_back("0x0");
            *bin = __encode_rtype(tokens, 0x0, 0x33);
            break;

        case hash("tlti"):
            tokens.insert(tokens.begin() + 2, "0xa");
            *bin = __encode_itype(tokens, 0x1);
            break;

        case hash("tltiu"):
            tokens.insert(tokens.begin() + 2, "0xb");
            *bin = __encode_itype(tokens, 0x1);
            break;

        case hash("lb"):
            *bin = __encode_ls(tokens, 0x20);
            break;

        case hash("lbu"):
            *bin = __encode_ls(tokens, 0x24);
            break;

        case hash("lh"):
            *bin = __encode_ls(tokens, 0x21);
            break;

        case hash("lhu"):
            *bin = __encode_ls(tokens, 0x25);
            break;

        case hash("lw"):
            *bin = __encode_ls(tokens, 0x23);
            break;

        case hash("lwl"):
            *bin = __encode_ls(tokens, 0x22);
            break;

        case hash("lwr"):
            *bin = __encode_ls(tokens, 0x26);
            break;

        case hash("ll"):
            *bin = __encode_ls(tokens, 0x30);
            break;

        case hash("sb"):
            *bin = __encode_ls(tokens, 0x28);
            break;

        case hash("sh"):
            *bin = __encode_ls(tokens, 0x29);
            break;

        case hash("sw"):
            *bin = __encode_ls(tokens, 0x2b);
            break;

        case hash("swl"):
            *bin = __encode_ls(tokens, 0x2a);
            break;

        case hash("swr"):
            *bin = __encode_ls(tokens, 0x2e);
            break;

        case hash("sc"):
            *bin = __encode_ls(tokens, 0x38);
            break;

        case hash("mfhi"): {
            tokens_t _t{tokens[0], "0x0", tokens[1], "0x0"};
            *bin = __encode_rtype(tokens, 0x0, 0x10);
        }
            break;

        case hash("mflo"): {
            tokens_t _t{tokens[0], "0x0", tokens[1], "0x0"};
            *bin = __encode_rtype(tokens, 0x0, 0x12);
        }
            break;

        case hash("mthi"): {
            tokens_t _t{tokens[0], "0x0", tokens[1], "0x0"};
            *bin = __encode_rtype(tokens, 0x0, 0x11);
        }
            break;

        case hash("mtlo"): {
            tokens_t _t{tokens[0], "0x0", tokens[1], "0x0"};
            *bin = __encode_rtype(tokens, 0x0, 0x13);
        }
            break;

        case hash("syscall"):
            *bin = 0xc;
            break;

        default:
            PRINTF_ERR_STAMP("[!] unrecognized instruction: %s\n", opcode_string.c_str());
            return 0;

    }

    return 1;
}

bool __catalyze_content(Assembler *assembler) {
    bool inText = false, inData = false;
    uint32_t pointat = 0x10000;

    for (std::string line: assembler->content) {
        // remove leading and trailing spaces
        line = std::regex_replace(line, std::regex("^[ \t]+"), "");
        line = std::regex_replace(line, std::regex("[ \t]+$"), "");
        line = removeComments(line);

        if (isLineEmpty(line))
            continue;

        switch (hash(line.c_str())) {

            case hash(".text"):
                PRINTF_DEBUG_VERBOSE(verbose, "[ASM]\t[DT]\t\t%s\n", line.c_str());
                inText = true;
                inData = false;
                continue;

            case hash(".data"):
                PRINTF_DEBUG_VERBOSE(verbose, "[ASM]\t[DD]\t\t%s\n", line.c_str());
                inText = false;
                inData = true;
                continue;

            default:
                break;

        }

        if (inText) {
            if (isLineALabel(line)) {
                std::string label = line.substr(0, line.find(":"));
                PRINTF_DEBUG_VERBOSE(verbose, "[ASM]\t[LABEL]\t\t%s\t----->\tpoint_at: 0x%X\n", label.c_str(), pointat);
                assembler->label_map[label] = pointat;
            } else {
                assembler->text_section.push_back(line);
                pointat += 1;
            }
        }
    }

    return 1;
}

bool __parse(Assembler *assembler) {
    for (auto line: assembler->text_section) {
        tokens_t tokens = tokenize_str(line);
        uint32_t bin_line;

        if (!encode(assembler, tokens, &bin_line)) {
            return 0;
        }
        assembler->bin.push_back(bin_line);
    }
    return true;
}

bool __assembler_exec(Assembler *assembler) {
    bool inText = false, inData = false;
    uint32_t pointat = 0x10000;
    uint32_t ret = 1;

    ret = !(ret ^ __catalyze_content(assembler));
    ret = !(ret ^ __parse(assembler));

    return ret;
}

void assembler_exec(Assembler *assembler) {
    PRINTF_DEBUG_VERBOSE(verbose, "[ASM]\tStart to assembling input content:\n");
    if (!__assembler_exec(assembler)) {
        EXIT_WITH_MSG("[!] assembling failed, exit...\n");
    }
    PRINTF_DEBUG_VERBOSE(verbose, "[ASM]\tAssembling finished.\n");
}

void assembler_free(Assembler *assembler) {

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