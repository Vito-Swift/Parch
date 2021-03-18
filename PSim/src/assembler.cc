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
    std::regex e("(.+):(.*)");
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
    return 0x1F & reg_map[reg_key];
}

void __encode_address(const token_t &addr_str, std::string *rs, uint32_t *offset) {
    std::regex rt_rgx("\\-?\\d+\\(\\$(\\w+\\d?)\\)");
    std::regex offset_rgx("(\\-?\\d+)\\(\\$\\w+\\d?\\)");
    std::smatch match;

    // search for offset in the token
    std::regex_search(addr_str, match, offset_rgx);
    *offset = 0xFFFF & int16_t(arbstoi(match[1]));

    // search for rt register in the token
    std::regex_search(addr_str, match, rt_rgx);
    *rs = match[1];
}

std::string __encode_label(Assembler *assembler, const token_t &label_str) {
    if (assembler->label_map.find(label_str) == assembler->label_map.end())
        EXIT_WITH_MSG("Assembly contains not defined label: %s\n", label_str.c_str());
    return std::to_string(assembler->label_map[label_str]);
}

std::string __encode_label_to_offset(Assembler *assembler, const token_t &label_str, uint32_t pointat) {
    if (assembler->label_map.find(label_str) == assembler->label_map.end())
        EXIT_WITH_MSG("Assembly contains not defined label: %s\n", label_str.c_str());
    int32_t offset = (assembler->label_map[label_str] - pointat);
    return std::to_string(offset);
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
    uint32_t shamt = 0x1F & (arbstoi(tokens[4]));
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
    uint32_t address = 0x3FFFFFF & arbstoi(tokens[1]);
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
    uint32_t immediate = 0xFFFF & int16_t(arbstoi(tokens[3]));
    uint32_t bin = (opcode << 26) | (rs << 21) | (rt << 16) | immediate;
    PRINTF_DEBUG_VERBOSE(verbose,
                         "[ASM]\t[ENCODE]\tInstruction: %s  %s,%s,%s"
                         "\t\t----->\t\t%s\n",
                         tokens[0].c_str(),
                         tokens[1].c_str(),
                         tokens[2].c_str(),
                         tokens[3].c_str(),
                         std::bitset<32>(bin).to_string().c_str());
    return bin;
}

uint32_t __encode_ls(const tokens_t &tokens, const uint32_t opcode) {
    // l/s rt, address -> rs, rt, offset
    uint32_t offset;
    std::string rs;
    __encode_address(tokens[2], &rs, &offset);
    tokens_t _tokens{tokens[0], rs, tokens[1], std::to_string(offset)};
    return __encode_itype(_tokens, opcode);
}

bool encode(Assembler *assembler, tokens_t &tokens, uint32_t *bin, uint32_t pointat) {
    token_t opcode_string = tokens[0];
    switch (hash(opcode_string.c_str())) {

        case hash("add"): {
            // add rd, rs, rt -> rs, rt, rd, 0x0
            tokens_t _t{tokens[0], tokens[2], tokens[3], tokens[1], "0x0"};
            *bin = __encode_rtype(_t, 0x0, 0x20);
            break;
        }

        case hash("addu"): {
            // addu rd, rs, rt -> rs, rt, rd, 0x0
            tokens_t _t{tokens[0], tokens[2], tokens[3], tokens[1], "0x0"};
            *bin = __encode_rtype(_t, 0x0, 0x21);
            break;
        }

        case hash("addi"): {
            // addi rt, rs, imm -> rs, rt, imm
            tokens_t _t{tokens[0], tokens[2], tokens[1], tokens[3]};
            *bin = __encode_itype(_t, 0x8);
            break;
        }

        case hash("addiu"): {
            // addiu rt, rs, imm -> rs, rt, imm
            tokens_t _t{tokens[0], tokens[2], tokens[1], tokens[3]};
            *bin = __encode_itype(_t, 0x9);
            break;
        }

        case hash("and"): {
            // and rd, rs, rt -> rs, rt, rd, 0x0
            tokens_t _t{tokens[0], tokens[2], tokens[3], tokens[1], "0x0"};
            *bin = __encode_rtype(_t, 0x0, 0x24);
            break;
        }

        case hash("andi"): {
            // andi rt, rs, imm -> rs, rt, imm
            tokens_t _t{tokens[0], tokens[2], tokens[1], tokens[3]};
            *bin = __encode_itype(_t, 0xc);
            break;
        }

        case hash("clo"): {
            // clo rd, rs -> rs, 0x0, rd, 0x0
            tokens_t _t{tokens[0], tokens[2], "0x0", tokens[1], "0x0"};
            *bin = __encode_rtype(_t, 0x1c, 0x21);
            break;
        }

        case hash("clz"): {
            // clz rd, rs -> rs, 0x0, rd, 0x0
            tokens_t _t{tokens[0], tokens[2], "0x0", tokens[1], "0x0"};
            *bin = __encode_rtype(_t, 0x1c, 0x20);
            break;
        }

        case hash("div"): {
            // div rs, rt -> rs, rt, 0x1a
            tokens_t _t{tokens[0], tokens[1], tokens[2], "0x1a"};
            *bin = __encode_itype(_t, 0x0);
            break;
        }

        case hash("divu"): {
            // divu rs, rt -> rs, rt, 0x1b
            tokens.push_back(token_t("0x1b"));
            *bin = __encode_itype(tokens, 0x0);
            break;
        }

        case hash("mult"): {
            // mult rs, rt -> rs, rt, 0x18
            tokens.push_back(token_t("0x18"));
            *bin = __encode_itype(tokens, 0x0);
            break;
        }

        case hash("madd"): {
            // madd rs, rt -> rs, rt, 0x0
            tokens.push_back(token_t("0x0"));
            *bin = __encode_itype(tokens, 0x1c);
            break;
        }

        case hash("msub"): {
            // msub rs, rt -> rs, rt, 0x4
            tokens.push_back(token_t("0x4"));
            *bin = __encode_itype(tokens, 0x1c);
            break;
        }

        case hash("maddu"): {
            // maddu rs, rt -> rs, rt, 0x1
            tokens.push_back(token_t("0x1"));
            *bin = __encode_itype(tokens, 0x1c);
            break;
        }

        case hash("msubu"): {
            // msubu rs, rt -> rs, rt, 0x4
            tokens.push_back(token_t("0x5"));
            *bin = __encode_itype(tokens, 0x1c);
            break;
        }

        case hash("nor"): {
            // nor rd, rs, rt -> rs, rt, rd, 0x0
            tokens_t _t{tokens[0], tokens[2], tokens[3], tokens[1], "0x0"};
            *bin = __encode_rtype(_t, 0x0, 0x27);
            break;
        }

        case hash("or"): {
            // or rd, rs, rt -> rs, rt, rd, 0x0
            tokens_t _t{tokens[0], tokens[2], tokens[3], tokens[1], "0x0"};
            *bin = __encode_rtype(_t, 0x0, 0x25);
            break;
        }

        case hash("ori"): {
            // ori rt, rs, imm -> rs, rt, imm
            tokens_t _t{tokens[0], tokens[2], tokens[1], tokens[3]};
            *bin = __encode_itype(_t, 0xd);
            break;
        }

        case hash("sll"): {
            // sll rd, rt, shamt -> rs, rt, rd, shamt
            tokens_t _t{tokens[0], "$zero", tokens[2], tokens[1], tokens[3]};
            *bin = __encode_rtype(_t, 0x0, 0x0);
            break;
        }

        case hash("sllv"): {
            // sllv rd, rt, rs -> rs, rt, rd, 0x0
            tokens_t _t{tokens[0], tokens[3], tokens[2], tokens[1], "0x0"};
            *bin = __encode_rtype(_t, 0x0, 0x4);
            break;
        }

        case hash("sra"): {
            // sra rd, rt, shamt -> rs, rt, rd, shamt
            tokens_t _t{tokens[0], "$zero", tokens[2], tokens[1], tokens[3]};
            *bin = __encode_rtype(_t, 0x0, 0x3);
            break;
        }

        case hash("srav"): {
            // srav rd, rt, rs -> rs, rt, rd, 0x0
            tokens_t _t{tokens[0], tokens[3], tokens[2], tokens[1], "0x0"};
            *bin = __encode_rtype(_t, 0x0, 0x7);
            break;
        }

        case hash("srl"): {
            // srl rd, rt, shamt -> rs, rt, rd, shamt
            tokens_t _t{tokens[0], "$zero", tokens[2], tokens[1], tokens[3]};
            *bin = __encode_rtype(_t, 0x0, 0x2);
            break;
        }

        case hash("srlv"): {
            // srlv rd, rt, rs -> rs, rt, rd, 0x0
            tokens_t _t{tokens[0], tokens[3], tokens[2], tokens[1], "0x0"};
            *bin = __encode_rtype(_t, 0x0, 0x6);
            break;
        }

        case hash("sub"): {
            // sub rd, rs, rt -> rs, rt, rd, 0x0
            tokens_t _t{tokens[0], tokens[2], tokens[3], tokens[1], "0x0"};
            *bin = __encode_rtype(_t, 0x0, 0x22);
            break;
        }

        case hash("subu"): {
            // subu rd, rs, rt -> rs, rt, rd, 0x0
            tokens_t _t{tokens[0], tokens[2], tokens[3], tokens[1], "0x0"};
            *bin = __encode_rtype(_t, 0x0, 0x23);
            break;
        }

        case hash("xor"): {
            // xor rd, rs, rt -> rs, rt, rd, 0x0
            tokens_t _t{tokens[0], tokens[2], tokens[3], tokens[1], "0x0"};
            *bin = __encode_rtype(_t, 0x0, 0x26);
            break;
        }

        case hash("xori"): {
            // xori rt, rs, imm -> rs, rt, imm
            tokens_t _t{tokens[0], tokens[2], tokens[1], tokens[3]};
            *bin = __encode_itype(_t, 0xe);
            break;
        }

        case hash("lui"): {
            // lui rt, imm -> 0x0, rt, imm
            tokens_t _t{tokens[0], "0x0", tokens[1], tokens[2]};
            *bin = __encode_itype(_t, 0xf);
            break;
        }

        case hash("slt"): {
            // slt rd, rs, rt -> rs, rt, rd, 0x0
            tokens_t _t{tokens[0], tokens[2], tokens[3], tokens[1], "0x0"};
            *bin = __encode_rtype(_t, 0x0, 0x2a);
            break;
        }

        case hash("sltu"): {
            // sltu rd, rs, rt -> rs, rt, rd, 0x0
            tokens_t _t{tokens[0], tokens[2], tokens[3], tokens[1], "0x0"};
            *bin = __encode_rtype(_t, 0x0, 0x2b);
            break;
        }

        case hash("slti"): {
            // slti rt, rs, imm -> rs, rt, imm
            tokens_t _t{tokens[0], tokens[2], tokens[1], tokens[3]};
            *bin = __encode_itype(_t, 0xa);
            break;
        }

        case hash("sltiu"): {
            // sltiu rt, rs, imm -> rs, rt, imm
            tokens_t _t{tokens[0], tokens[2], tokens[1], tokens[3]};
            *bin = __encode_itype(_t, 0xb);
            break;
        }

        case hash("beq"): {
            // beq rs, rt, offset
            tokens[3] = __encode_label_to_offset(assembler, tokens[3], pointat);
            *bin = __encode_itype(tokens, 0x4);
            break;
        }

        case hash("bgez"): {
            // bgez rs, label -> rs, 0x1, offset
            tokens_t _t{tokens[0], tokens[1], "0x1", __encode_label_to_offset(assembler, tokens[2], pointat)};
            *bin = __encode_itype(_t, 0x1);
            break;
        }

        case hash("bgezal"): {
            // bgezal rs, label-> rs, 0x11, offset
            tokens_t _t{tokens[0], tokens[1], "0x11", __encode_label_to_offset(assembler, tokens[2], pointat)};
            *bin = __encode_itype(_t, 0x1);
            break;
        }

        case hash("bgtz"): {
            // bgtz rs, label -> rs, 0x0, offset
            tokens_t _t{tokens[0], tokens[1], "0x0", __encode_label_to_offset(assembler, tokens[2], pointat)};
            *bin = __encode_itype(_t, 0x7);
            break;
        }

        case hash("blez"): {
            // blez rs, label -> rs, 0x0, offset
            tokens_t _t{tokens[0], tokens[1], "0x0", __encode_label_to_offset(assembler, tokens[2], pointat)};
            *bin = __encode_itype(_t, 0x6);
            break;
        }

        case hash("bltzal"): {
            // bltzal rs, label -> rs, 0x10, offset
            tokens_t _t{tokens[0], tokens[1], "0x10", __encode_label_to_offset(assembler, tokens[2], pointat)};
            *bin = __encode_itype(_t, 0x1);
            break;
        }

        case hash("bltz"): {
            // bltz rs, label -> rs, 0x0, offset
            tokens_t _t{tokens[0], tokens[1], "0x0", __encode_label_to_offset(assembler, tokens[2], pointat)};
            *bin = __encode_itype(_t, 0x1);
            break;
        }

        case hash("bne"): {
            // bne rs, rt, label -> rs, rt, offset
            tokens[3] = __encode_label_to_offset(assembler, tokens[3], pointat);
            *bin = __encode_itype(tokens, 0x5);
            break;
        }

        case hash("j"): {
            // j target
            tokens[1] = __encode_label(assembler, tokens[1]);
            *bin = __encode_jtype(tokens, 0x2);
            break;
        }

        case hash("jal"): {
            // jal target
            tokens[1] = __encode_label(assembler, tokens[1]);
            *bin = __encode_jtype(tokens, 0x3);
            break;
        }

        case hash("jalr"): {
            // jalr rs, rd -> rs, 0x0, rd, 0x0
            tokens_t _t{tokens[0], tokens[1], "0x0", tokens[2], "0x0"};
            *bin = __encode_jtype(_t, 0x9);
            break;
        }

        case hash("jr"): {
            // jr rs -> rs, 0x0, 0x8
            tokens_t _t{tokens[0], tokens[1], "0x0", "0x8"};
            *bin = __encode_itype(_t, 0x0);
            break;
        }

        case hash("teq"): {
            // teq rs, rt -> rs, rt, 0x0
            tokens.push_back("0x0");
            *bin = __encode_rtype(tokens, 0x0, 0x34);
            break;
        }

        case hash("teqi"): {
            // teqi rs, imm -> rs, 0xc, imm
            tokens_t _t{tokens[0], tokens[1], "0xc", tokens[2]};
            *bin = __encode_itype(_t, 0x1);
            break;
        }

        case hash("tne"): {
            // tne rs, rt -> rs, rt, 0x36
            tokens_t _t{tokens[0], tokens[1], tokens[2], "0x36"};
            *bin = __encode_itype(_t, 0x0);
            break;
        }

        case hash("tnei"): {
            // tnei rs, imm -> rs, 0xe, imm
            tokens_t _t{tokens[0], tokens[1], "0xe", tokens[2]};
            *bin = __encode_itype(_t, 0x1);
            break;
        }

        case hash("tge"): {
            // tge rs, rt -> rs, rt, 0x30
            tokens_t _t{tokens[0], tokens[1], tokens[2], "0x30"};
            *bin = __encode_itype(_t, 0x0);
            break;
        }

        case hash("tgeu"): {
            // tgeu rs, rt -> rs, rt, 0x31
            tokens_t _t{tokens[0], tokens[1], tokens[2], "0x31"};
            *bin = __encode_itype(_t, 0x0);
            break;
        }

        case hash("tgei"): {
            // tgei rs, imm -> rs, 0x8, imm
            tokens_t _t{tokens[0], tokens[1], "0x8", tokens[2]};
            *bin = __encode_itype(_t, 0x1);
            break;
        }

        case hash("tgeiu"): {
            // tgeiu rs, imm -> rs, 0x9, imm
            tokens_t _t{tokens[0], tokens[1], "0x9", tokens[2]};
            *bin = __encode_itype(_t, 0x1);
            break;
        }

        case hash("tlt"): {
            // tlt rs, rt -> rs, rt, 0x32
            tokens_t _t{tokens[0], tokens[1], tokens[2], "0x32"};
            *bin = __encode_itype(_t, 0x0);
            break;
        }

        case hash("tltu"): {
            // tltu rs, rt -> rs, rt, 0x33
            tokens_t _t{tokens[0], tokens[1], tokens[2], "0x33"};
            *bin = __encode_itype(_t, 0x0);
            break;
        }

        case hash("tlti"): {
            // tlti rs, imm -> rs, 0xa, imm
            tokens_t _t{tokens[0], tokens[1], "0xa", tokens[2]};
            *bin = __encode_itype(_t, 0x1);
            break;
        }

        case hash("tltiu"): {
            // tltiu rs, imm -> rs, 0xb, imm
            tokens_t _t{tokens[0], tokens[1], "0xb", tokens[2]};
            *bin = __encode_itype(_t, 0x1);
            break;
        }

        case hash("lb"): {
            *bin = __encode_ls(tokens, 0x20);
            break;
        }

        case hash("lbu"): {
            *bin = __encode_ls(tokens, 0x24);
            break;
        }

        case hash("lh"): {
            *bin = __encode_ls(tokens, 0x21);
            break;
        }

        case hash("lhu"): {
            *bin = __encode_ls(tokens, 0x25);
            break;
        }

        case hash("lw"): {
            *bin = __encode_ls(tokens, 0x23);
            break;
        }

        case hash("lwl"): {
            *bin = __encode_ls(tokens, 0x22);
            break;
        }

        case hash("lwr"): {
            *bin = __encode_ls(tokens, 0x26);
            break;
        }

        case hash("ll"): {
            *bin = __encode_ls(tokens, 0x30);
            break;
        }

        case hash("sb"): {
            *bin = __encode_ls(tokens, 0x28);
            break;
        }

        case hash("sh"): {
            *bin = __encode_ls(tokens, 0x29);
            break;
        }

        case hash("sw"): {
            *bin = __encode_ls(tokens, 0x2b);
            break;
        }

        case hash("swl"): {
            *bin = __encode_ls(tokens, 0x2a);
            break;
        }

        case hash("swr"): {
            *bin = __encode_ls(tokens, 0x2e);
            break;
        }

        case hash("sc"): {
            *bin = __encode_ls(tokens, 0x38);
            break;
        }

        case hash("mfhi"): {
            // mfhi rd -> 0x0, 0x0, rd, 0x0
            tokens_t _t{tokens[0], "$zero", "$zero", tokens[1], "0x0"};
            *bin = __encode_rtype(_t, 0x0, 0x10);
        }
            break;

        case hash("mflo"): {
            // mflo rd -> 0x0, rd, 0x0
            tokens_t _t{tokens[0], "$zero", "$zero", tokens[1], "0x0"};
            *bin = __encode_rtype(_t, 0x0, 0x12);
        }
            break;

        case hash("mthi"): {
            // mthi rs -> rs, 0x0, 0x11
            tokens_t _t{tokens[0], tokens[1], "$zero", "0x11"};
            *bin = __encode_itype(_t, 0x0);
        }
            break;

        case hash("mtlo"): {
            tokens_t _t{tokens[0], tokens[1], "$zero", "0x13"};
            *bin = __encode_itype(_t, 0x0);
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
    bool contentAllText = true;
    bool inText = false, inData = false;
    uint32_t pointat = 0x100000;

    for (std::string line: assembler->content) {
        line = std::regex_replace(line, std::regex("^[ \t]+"), "");
        line = std::regex_replace(line, std::regex("[ \t]+$"), "");
        line = removeComments(line);

        if (isLineEmpty(line))
            continue;

        if (hash(line.c_str()) == hash(".text")) {
            contentAllText = false;
            break;
        }
    }

    for (std::string line: assembler->content) {
        // remove leading and trailing spaces
        line = std::regex_replace(line, std::regex("^[ \t]+"), "");
        line = std::regex_replace(line, std::regex("[ \t]+$"), "");
        line = removeComments(line);

        if (isLineEmpty(line))
            continue;

        if (!contentAllText) {
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
        }

        if (inText || contentAllText) {
            if (isLineALabel(line)) {
                std::regex label_rgx("(.+):.*");
                std::regex remainder_rgx(".+:\\s*(.*)");
                std::smatch match;

                // search for label in the line
                std::regex_search(line, match, label_rgx);
                std::string label = match[1];

                // search for remaining content in the line
                std::regex_search(line, match, remainder_rgx);
                std::string line_remainder = match[1];

                PRINTF_DEBUG_VERBOSE(verbose, "[ASM]\t[LABEL]\t\t%s\t----->\tpoint_at: 0x%X\n", label.c_str(), pointat);
                assembler->label_map[label] = pointat;

                if (!line_remainder.empty()) {
                    assembler->text_section.push_back(line_remainder);
                    pointat += 1;
                }
            } else {
                assembler->text_section.push_back(line);
                pointat += 1;
            }
        }
    }

    return 1;
}

bool __parse(Assembler *assembler) {
    uint32_t pointat = 0x100000;
    for (auto line: assembler->text_section) {
        tokens_t tokens = tokenize_str(line);
        uint32_t bin_line;
        pointat += 1;

        if (!encode(assembler, tokens, &bin_line, pointat)) {
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
                             ELF_path.c_str());

        assembler->ELF_path = ELF_path;
        parse_file(assembler);
    } else {

    }

    PRINTF_DEBUG_VERBOSE(verbose, "[ASM]\tInitialize register map\n");
    reg_map = create_regparse_map();
}