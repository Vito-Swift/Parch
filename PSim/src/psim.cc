/**
 * @filename: psim.cc.c
 * @author: Vito Wu <chenhaowu[at]link.cuhk.edu.cn>
 * @version: 
 * @desc:
 * @date: 2/20/2021
 */

#include "psim.hh"

void load_input(Simulator *simulator) {
    if (!isFileExist(simulator->user_options.input_file)) {
        EXIT_WITH_MSG("[SIM]\tFailed to read input file\n");
    }

    std::ifstream infile(simulator->user_options.input_file);
    std::string line;
    while (std::getline(infile, line)) {
        simulator->inputs.push_back(line);
    }
}

void simulator_init(Simulator *simulator, int argc, char **argv) {
    options_init(&simulator->user_options);
    options_parse(&simulator->user_options, argc, argv);

    mmbar_init(&simulator->mmBar);

    assembler_init(&simulator->assembler,
                   std::string(simulator->user_options.ELF),
                   simulator->user_options.from_elf);
    simulator->assembler.user_options = &simulator->user_options;
    simulator->assembler.mmBar = &simulator->mmBar;

    if (simulator->user_options.input_from_file) {
        load_input(simulator);
    }

    simulator->current_input = 0;
}

#define get_opcode(bin) (bin >> 26)

std::string get_input(Simulator *simulator) {
    if (simulator->current_input >= simulator->inputs.size()) {
        EXIT_WITH_MSG("[SIM]\tFailed to get input from file\n");
    }
    return simulator->inputs[simulator->current_input++];
}

int32_t art_rshift(int x, int n) {
    if (x < 0 && n > 0)
        return (x >> n) | ~(~0U >> n);
    else
        return x >> n;
}

void syscall(Simulator *simulator) {
    PRINTF_DEBUG_VERBOSE(verbose,
                         "[SIM]\tInvoking system call!\n");

    switch (register_file[v0]) {

        case 1: {
            // print int
            PRINTF_DEBUG_VERBOSE(verbose,
                                 "[SIM]\t[SYSCALL]\tprint int\n");
            printf("%d", register_file[a0]);
            break;
        }

        case 4: {
            // print string
            PRINTF_DEBUG_VERBOSE(verbose,
                                 "[SIM]\t[SYSCALL]\tprint string\n");
            uint32_t _s = register_file[a0];
            char c;
            while ((c = mmbar_read(&simulator->mmBar, _s++)) != '\0') {
                printf("%c", c);
            }
            break;
        }

        case 5: {
            // read int
            if (simulator->user_options.input_from_file) {
                PRINTF_DEBUG_VERBOSE(verbose, "[SIM]\t[SYSCALL]\tread int (from input file)\n");
                int n = arbstoi(get_input(simulator));
                register_file[v0] = n;
            } else {
                PRINTF_DEBUG_VERBOSE(verbose, "[SIM]\t[SYSCALL]\tread int (from stdin)\n");
                int n;
                std::cin >> n;
                register_file[v0] = n;
            }

            break;
        }

        case 8: {
            // read string
            if (simulator->user_options.input_from_file) {
                PRINTF_DEBUG_VERBOSE(verbose, "[SIM]\t[SYSCALL]\tread string (from input file)\n");
                std::string s = get_input(simulator);
                char cs[s.length()];
                strcpy(cs, s.c_str());
                if (register_file[a1] <= s.length())
                    memcpy((char *) &simulator->mmBar._memory[register_file[a0]], cs, register_file[a1]);
                else
                    memcpy((char *) &simulator->mmBar._memory[register_file[a0]], cs, s.length());
            } else {
                PRINTF_DEBUG_VERBOSE(verbose, "[SIM]\t[SYSCALL]\tread string (from stdin)\n");
                fgets((char *) &simulator->mmBar._memory[register_file[a0]], register_file[a1], stdin);
            }
            break;
        }

        case 9: {
            // sbrk
            register_file[v0] = mmbar_allocate(&simulator->mmBar, register_file[a0]);
            PRINTF_DEBUG_VERBOSE(verbose, "[SIM]\t[SYSCALL]\tAllocate memory: %d\n",
                                 register_file[v0]);
            break;
        }

        case 10: {
            // exit
            PRINTF_DEBUG_VERBOSE(verbose,
                                 "[SIM]\t[SYSCALL]\texit\n");
            exit(0);
        }

        case 11: {
            // print char
            PRINTF_DEBUG_VERBOSE(verbose, "[SIM]\t[SYSCALL]\tprint char");
            printf("%c", (char) register_file[a0]);
            break;
        }

        case 12: {
            // read char
            if (simulator->user_options.input_from_file) {
                PRINTF_DEBUG_VERBOSE(verbose,
                                     "[SIM]\t[SYSCALL]\tread char (from file)\n");
                char gc = get_input(simulator).c_str()[0];
                register_file[v0] = gc;
            } else {
                PRINTF_DEBUG_VERBOSE(verbose,
                                     "[SIM]\t[SYSCAL]\tread char (from stdin)\n");
                register_file[v0] = getchar();
            }

            break;
        }

        case 13: {
            // open
            PRINTF_DEBUG_VERBOSE(verbose,
                                 "[SIM]\t[SYSCALL]\topen file: %s\n", (char *) register_file[a0]);
            register_file[a0] = open((char *) register_file[a0], register_file[a1], register_file[a2]);
            break;
        }

        case 14: {
            // read
            PRINTF_DEBUG_VERBOSE(verbose, "[SIM]\t[SYSCALL]\tread file\n");
            register_file[a0] = read(register_file[a0], (void *) register_file[a1], register_file[a2]);
            break;
        }

        case 15: {
            // write
            PRINTF_DEBUG_VERBOSE(verbose, "[SIM]\t[SYSCALL]\twrite file\n");
            register_file[a0] = write(register_file[a0], (void *) register_file[a1], register_file[a2]);
            break;
        }

        case 16: {
            // close
            PRINTF_DEBUG_VERBOSE(verbose, "[SIM]\t[SYSCALL]\tclose file\n");
            close(register_file[a0]);
            break;
        }

        case 17: {
            // exit2
            PRINTF_DEBUG_VERBOSE(verbose,
                                 "[SIM]\t[SYSCALL]\texit with signal: %d\n", register_file[a0]);
            exit(register_file[a0]);
        }

        default: {
            PRINTF_ERR_STAMP("[SIM]\t[SYSCALL]\tUnknown system call: %d\n", register_file[v0]);
            break;
        }

    }
}

bool __decode_rcluster(Simulator *simulator, uint32_t bin) {
#define get_funct(bin) (bin & 0x3F)
#define get_rs(bin) ((bin >> 21) & 0x1F)
#define get_rt(bin) ((bin >> 16) & 0x1F)
#define get_rd(bin) ((bin >> 11) & 0x1F)
#define get_shamt(bin) ((bin >> 6) & 0x1F)

    uint32_t funct = get_funct(bin);
    uint32_t rs = get_rs(bin);
    uint32_t rt = get_rt(bin);
    uint32_t rd = get_rd(bin);
    uint32_t shamt = get_shamt(bin);

    switch (funct) {

        case 0: {
            // sll
            register_file[rd] = (unsigned) register_file[rt] << shamt;
            PRINTF_DEBUG_VERBOSE(verbose,
                                 "[SIM]\t[R]\tExecution: sll %d, %d, %d\n",
                                 rd, rt, shamt);
            break;
        }

        case 2: {
            // srl
            register_file[rd] = (unsigned) register_file[rt] >> shamt;
            PRINTF_DEBUG_VERBOSE(verbose,
                                 "[SIM]\t[R]\tExecution: srl %d, %d, %d\n",
                                 rd, rt, shamt);
            break;
        }

        case 3: {
            // sra
            register_file[rd] = art_rshift(register_file[rt], shamt);
            PRINTF_DEBUG_VERBOSE(verbose,
                                 "[SIM]\t[R]\tExecution: sra %d, %d, %d\n",
                                 rd, rt, shamt);
            break;
        }

        case 4: {
            // sllv
            register_file[rd] = (unsigned) register_file[rt] << register_file[rs];
            PRINTF_DEBUG_VERBOSE(verbose,
                                 "[SIM]\t[R]\tExecution: sllv %d, %d, %d\n",
                                 rd, rt, rs);
            break;
        }

        case 6: {
            // srlv
            register_file[rd] = (unsigned) register_file[rt] >> register_file[rs];
            PRINTF_DEBUG_VERBOSE(verbose,
                                 "[SIM]\t[R]\tExecution srlv %d, %d, %d\n",
                                 rd, rt, rs);
            break;
        }

        case 7: {
            // srav
            register_file[rd] = art_rshift(register_file[rt], register_file[rs]);
            PRINTF_DEBUG_VERBOSE(verbose,
                                 "[SIM]\t[R]\tExecution: srav %d, %d, %d(%d)\n",
                                 rd, rt, rs, register_file[rs]);
            break;
        }

        case 8: {
            // jr
            simulator->pc = ((register_file[rs] - 1) << 2);
            PRINTF_DEBUG_VERBOSE(verbose,
                                 "[SIM]\t[R]\tExecution: jr %d(%d)\n", rs, register_file[rs] << 2);
            break;
        }

        case 9: {
            // jalr
            register_file[rd] = (simulator->pc + 4) >> 2;
            simulator->pc = (register_file[rs] << 2) - 4;
            PRINTF_DEBUG_VERBOSE(verbose,
                                 "[SIM]\t[R]\tExecution: jalr %d(%d), %d(%d)\n",
                                 rs, register_file[rs] << 2, rd, register_file[rd]);
            break;
        }

        case 12: {
            // syscall
            syscall(simulator);
            break;
        }

        case 16: {
            // mfhi
            register_file[rd] = register_file[HI];
            PRINTF_DEBUG_VERBOSE(verbose,
                                 "[SIM]\t[R]\tExecution: mfhi %d\n", rd);
            break;
        }

        case 17: {
            // mthi
            register_file[HI] = register_file[rs];
            PRINTF_DEBUG_VERBOSE(verbose,
                                 "[SIM]\t[R]\tExecution: mthi %d(%d)\n", rs, register_file[rs]);
            break;
        }

        case 18: {
            // mflo
            register_file[rd] = register_file[LO];
            PRINTF_DEBUG_VERBOSE(verbose,
                                 "[SIM]\t[R]\tExecution: mflo %d\n", rd);
            break;
        }

        case 19: {
            // mtlo
            register_file[LO] = register_file[rs];
            PRINTF_DEBUG_VERBOSE(verbose,
                                 "[SIM]\t[R]\tExecution: mtlo %d(%d)\n", rs, register_file[rs]);
            break;
        }

        case 24: {
            // mult
            int64_t result = (int32_t) register_file[rs] * (int32_t) register_file[rt];
            register_file[HI] = result >> 32;
            register_file[LO] = result & 0xFFFFFFFF;
            PRINTF_DEBUG_VERBOSE(verbose,
                                 "[SIM]\t[R]\tExecution: mult %d(%d), %d(%d)\n",
                                 rs, register_file[rs], rt, register_file[rt]);
            break;
        }

        case 25: {
            // multu
            int64_t result = (uint32_t) register_file[rs] * (uint32_t) register_file[rt];
            register_file[HI] = result >> 32;
            register_file[LO] = result & 0xFFFFFFFF;
            PRINTF_DEBUG_VERBOSE(verbose,
                                 "[SIM]\t[R]\tExecution: multu %d(%d), %d(%d)\n",
                                 rs, register_file[rs], rt, register_file[rt]);
            break;
        }

        case 26: {
            // div
            if (register_file[rt] == 0) {
                EXIT_WITH_MSG("OVERFLOW: division result overflow!\n");
            }

            int32_t c = (int32_t) register_file[rs] / (int32_t) register_file[rt];
            int32_t d = (int32_t) register_file[rs] % (int32_t) register_file[rt];
            register_file[HI] = c;
            register_file[LO] = d;
            PRINTF_DEBUG_VERBOSE(verbose,
                                 "[SIM]\t[R]\tExecution: div %d(%d), %d(%d)\n",
                                 rs, register_file[rs], rt, register_file[rt]);
            break;
        }

        case 27: {
            // divu
            if (register_file[rt] == 0) {
                EXIT_WITH_MSG("OVERFLOW: division result overflow!\n");
            }

            uint32_t c = (uint32_t) register_file[rs] / (uint32_t) register_file[rt];
            uint32_t d = (uint32_t) register_file[rs] % (uint32_t) register_file[rt];
            register_file[HI] = c;
            register_file[LO] = d;
            PRINTF_DEBUG_VERBOSE(verbose,
                                 "[SIM]\t[R]\tExecution: divu %d(%d), %d(%d)\n",
                                 rs, register_file[rs], rt, register_file[rt]);
            break;
        }

        case 32: {
            // add
            int64_t result = register_file[rs] + register_file[rt];
            if ((result & ~(0xFFFFFFFF)) != 0) {
                EXIT_WITH_MSG("OVERFLOW: addition result overflow!\n");
            }
            PRINTF_DEBUG_VERBOSE(verbose,
                                 "[SIM]\t[R]\tExecution: add %d(%d), %d(%d), %d(%d)\n",
                                 rd, result, rs, register_file[rs], rt, register_file[rt]);
            register_file[rd] = (int32_t) result;
            break;
        }

        case 33: {
            // addu
            register_file[rd] = register_file[rs] + register_file[rt];
            PRINTF_DEBUG_VERBOSE(verbose,
                                 "[SIM]\t[R]\tExecution: addu %d, %d(%d), %d(%d)\n",
                                 rd, rs, register_file[rs], rt, register_file[rt]);
            break;
        }

        case 34: {
            // sub
            int64_t result = register_file[rs] - register_file[rt];
            if ((result & ~(0xFFFFFFFF)) != 0) {
                EXIT_WITH_MSG("OVERFLOW: subtraction result overflow!\n");
            }

            register_file[rd] = (int32_t) result;
            PRINTF_DEBUG_VERBOSE(verbose,
                                 "[SIM]\t[R]\tExecution: sub %d, %d(%d), %d(%d)\n",
                                 rd, rs, register_file[rs], rt, register_file[rt]);
            break;
        }

        case 35: {
            // subu
            register_file[rd] = register_file[rs] - register_file[rt];
            PRINTF_DEBUG_VERBOSE(verbose,
                                 "[SIM]\t[R]\tExecution: subu %d, %d(%d), %d(%d)\n",
                                 rd, rs, register_file[rs], rt, register_file[rt]);
            break;
        }

        case 36: {
            // and
            register_file[rd] = register_file[rs] & register_file[rt];
            PRINTF_DEBUG_VERBOSE(verbose,
                                 "[SIM]\t[R]\tExecution: and %d, %d(%d), %d(%d)\\n\",\n",
                                 rd, rs, register_file[rs], rt, register_file[rt]);
            break;
        }

        case 37: {
            // or
            register_file[rd] = register_file[rs] | register_file[rt];
            PRINTF_DEBUG_VERBOSE(verbose,
                                 "[SIM]\t[R]\tExecution: or %d, %d(%d), %d(%d)\\n\",\n",
                                 rd, rs, register_file[rs], rt, register_file[rt]);
            break;
        }

        case 38: {
            // xor
            register_file[rd] = register_file[rs] ^ register_file[rt];
            PRINTF_DEBUG_VERBOSE(verbose,
                                 "[SIM]\t[R]\tExecution: xor %d, %d(%d), %d(%d)\\n\",\n",
                                 rd, rs, register_file[rs], rt, register_file[rt]);
            break;
        }

        case 39: {
            // nor
            register_file[rd] = ~(register_file[rs] | register_file[rt]);
            PRINTF_DEBUG_VERBOSE(verbose,
                                 "[SIM]\t[R]\tExecution: nor %d, %d(%d), %d(%d)\\n\",\n",
                                 rd, rs, register_file[rs], rt, register_file[rt]);
            break;
        }

        case 42: {
            // slt
            if (register_file[rs] < register_file[rt])
                register_file[rd] = 1;
            PRINTF_DEBUG_VERBOSE(verbose,
                                 "[SIM]\t[R]\tExecution: slt %d, %d(%d), %d(%d)\\n\",\n",
                                 rd, rs, register_file[rs], rt, register_file[rt]);
            break;
        }

        case 43: {
            // sltu
            if ((uint32_t) register_file[rs] < (uint32_t) register_file[rt])
                register_file[rd] = 1;
            PRINTF_DEBUG_VERBOSE(verbose,
                                 "[SIM]\t[R]\tExecution: sltu %d, %d(%d), %d(%d)\\n\",\n",
                                 rd, rs, register_file[rs], rt, register_file[rt]);
            break;
        }

        case 48: {
            // tge
            if (register_file[rs] >= register_file[rt])
                EXIT_WITH_MSG("TRAP: tge %d(%d) %d(%d)\n",
                              rs, register_file[rs], rt, register_file[rt]);
            break;
        }

        case 49: {
            // tltu
            if ((uint32_t) register_file[rs] >= (uint32_t) register_file[rt])
                EXIT_WITH_MSG("TRAP: tgeu %d(%d) %d(%d)\n",
                              rs, register_file[rs], rt, register_file[rt]);
            break;
        }

        case 50: {
            // tlt
            if (register_file[rs] < register_file[rt])
                EXIT_WITH_MSG("TRAP: tlt %d(%d) %d(%d)\n",
                              rs, register_file[rs], rt, register_file[rt]);
            break;
        }

        case 51: {
            // tltu
            if ((uint32_t) register_file[rs] < (uint32_t) register_file[rt])
                EXIT_WITH_MSG("TRAP: tltu %d(%d) %d(%d)\n",
                              rs, register_file[rs], rt, register_file[rt]);
            break;
        }

        case 52: {
            // teq
            if (register_file[rs] == register_file[rt])
                EXIT_WITH_MSG("TRAP: teq %d(%d) %d(%d)\n",
                              rs, register_file[rs], rt, register_file[rt]);
            break;
        }

        case 54: {
            // tne
            if (register_file[rs] != register_file[rt])
                EXIT_WITH_MSG("TRAP: tne %d(%d) %d(%d)\n",
                              rs, register_file[rs], rt, register_file[rt]);
            break;
        }

        default: {
            PRINTF_ERR_STAMP("[SIM]\t[R]\tUnrecognized funct domain: %d\n", funct);
            return 0;
        }

    }

#undef get_funct
#undef get_rs
#undef get_rt
#undef get_rd
#undef get_shamt

    return 1;
}

void debug_dump_registers() {
    std::map<std::string, uint32_t> rgm = create_regparse_map();
    PRINTF_DEBUG_VERBOSE(verbose, "\t\tREG(");
    uint32_t i = 0;
    for (std::map<std::string, uint32_t>::reverse_iterator it = rgm.rbegin(); it != rgm.rend(); it++) {
        i++;
        printf("%s(%d) = %d, ", it->first.c_str(), it->second, register_file[it->second]);
        if (i % 8 == 0) {
            printf("\n");
            PRINTF_DEBUG_VERBOSE(verbose, "\t\t\t");
        }
    }
    printf(")\n");
    PRINTF_DEBUG_VERBOSE(verbose, "\n");
}

bool __decode_branch_trap(Simulator *simulator, uint32_t bin) {
#define get_rs(bin) ((bin >> 21) & 0x1F)
#define get_rt(bin) ((bin >> 16) & 0x1F)
#define get_imm(bin) ((int16_t)(bin & 0xFFFF))

    int16_t imm = get_imm(bin);
    uint32_t rs = get_rs(bin);
    uint32_t rt = get_rt(bin);

    switch (rt) {

        case 0: {
            // bltz
            if (register_file[rs] < 0)
                simulator->pc += 4 * imm;
            PRINTF_DEBUG_VERBOSE(verbose,
                                 "[SIM]\t[RBT]\tExecution: bltz %d(%d), %d\n",
                                 rs, register_file[rs], imm);
            break;
        }

        case 1: {
            // bgez
            if (register_file[rs] >= 0)
                simulator->pc += 4 * imm;
            PRINTF_DEBUG_VERBOSE(verbose,
                                 "[SIM]\t[RBT]\tExecution: bgez %d(%d), %d\n",
                                 rs, register_file[rs], imm);
            break;
        }

        case 8: {
            // tgei
            if (register_file[rs] >= imm)
                EXIT_WITH_MSG("TRAP: tgei %d(%d), %d\n",
                              rs, register_file[rs], imm);
            break;
        }

        case 9: {
            // tgeiu
            if ((uint32_t) register_file[rs] >= (uint16_t) imm)
                EXIT_WITH_MSG("TRAP: tgeiu %d(%d), %d\n",
                              rs, register_file[rs], imm);
            break;
        }

        case 10: {
            // tlti
            if (register_file[rs] < imm)
                EXIT_WITH_MSG("TRAP: tlti %d(%d), %d\n",
                              rs, register_file[rs], imm);
            break;
        }

        case 11: {
            // tltiu
            if ((uint32_t) register_file[rs] < (uint16_t) imm)
                EXIT_WITH_MSG("TRAP: tltiu %d(%d), %d\n",
                              rs, register_file[rs], imm);
            break;
        }

        case 14: {
            // tnei
            if (register_file[rs] != imm)
                EXIT_WITH_MSG("TRAP: tnei %d(%d), %d\n",
                              rs, register_file[rs], imm);
            break;
        }

        case 16: {
            // bltzal
            register_file[ra] = (simulator->pc + 4) >> 2;
            if (register_file[rs] < 0)
                simulator->pc += 4 * imm;
            PRINTF_DEBUG_VERBOSE(verbose,
                                 "[SIM]\t[RBT]\tExecution: bltzal %d(%d), %d\n",
                                 rs, register_file[rs], imm);
            break;
        }

        case 17: {
            // bgezal
            register_file[ra] = (simulator->pc + 4) >> 2;
            if (register_file[rs] >= 0)
                simulator->pc += 4 * imm;
            PRINTF_DEBUG_VERBOSE(verbose,
                                 "[SIM]\t[RBT]\tExecution: bgezal %d(%d), %d\n",
                                 rs, register_file[rs], imm);
            break;
        }

        default: {
            PRINTF_ERR_STAMP("[SIM]\t[RBT]\tUnrecognized imm domain: %d\n", imm);
            break;
        }
    }

#undef get_rs
#undef get_rt
#undef get_imm

    return 1;
}

bool decode(Simulator *simulator, uint32_t b) {
    switch (get_opcode(b)) {

        case 0x0: {
            return __decode_rcluster(simulator, b);
        }

        case 0x1: {
            return __decode_branch_trap(simulator, b);
        }

        case 0x2: {
            // j target
            uint32_t offset = b & 0x3FFFFFF;
            simulator->pc = (offset << 2) - 4;
            PRINTF_DEBUG_VERBOSE(verbose,
                                 "[SIM]\t[D]\tExecution: j %d\n", offset << 2);
            break;
        }

        case 0x3: {
            // jal target
            register_file[ra] = (simulator->pc >> 2) + 1;
            uint32_t offset = b & 0x3FFFFFF;
            simulator->pc = (offset << 2) - 4;
            PRINTF_DEBUG_VERBOSE(verbose,
                                 "[SIM]\t[D]\tExecution: jal %d\n", offset << 2);
            break;
        }

#define get_rs(bin) ((bin >> 21) & 0x1F)
#define get_rt(bin) ((bin >> 16) & 0x1F)
#define get_imm(bin) ((int16_t)(bin & 0xFFFF))

        case 0x4: {
            // beq rs, rt, offset
            uint32_t rs = get_rs(b);
            uint32_t rt = get_rt(b);
            int16_t imm = get_imm(b);

            if (register_file[rs] == register_file[rt])
                simulator->pc += 4 * imm;

            PRINTF_DEBUG_VERBOSE(verbose,
                                 "[SIM]\t[D]\tExecution: beq %d(%d), %d(%d), %d\n",
                                 rs, register_file[rs], rt, register_file[rt], imm);
            break;
        }

        case 0x5: {
            // bne, rs, rt, offset
            uint32_t rs = get_rs(b);
            uint32_t rt = get_rt(b);
            int16_t imm = get_imm(b);

            if (register_file[rs] != register_file[rt])
                simulator->pc += 4 * imm;

            PRINTF_DEBUG_VERBOSE(verbose,
                                 "[SIM]\t[D]\tExecution: bne %d(%d), %d(%d), %d\n",
                                 rs, register_file[rs], rt, register_file[rt], imm);

            break;
        }

        case 0x6: {
            // blez, rs, 0x0, offset
            uint32_t rs = get_rs(b);
            int16_t imm = get_imm(b);

            if (register_file[rs] <= 0)
                simulator->pc += 4 * imm;

            PRINTF_DEBUG_VERBOSE(verbose,
                                 "[SIM]\t[D]\tExecution: blez %d(%d), %d\n",
                                 rs, register_file[rs], imm);
            break;
        }

        case 0x7: {
            // bgtz, rs, 0x0, offset
            uint32_t rs = get_rs(b);
            int16_t imm = get_imm(b);

            if (register_file[rs] > 0)
                simulator->pc += 4 * imm;

            PRINTF_DEBUG_VERBOSE(verbose,
                                 "[SIM]\t[D]\tExecution: bgtz %d(%d), %d\n",
                                 rs, register_file[rs], imm);
            break;
        }

        case 0x8: {
            // addi, rs, rt, imm
            uint32_t rs = get_rs(b);
            uint32_t rt = get_rt(b);
            int16_t imm = get_imm(b);

            int64_t result = register_file[rs] + imm;
            if ((result & ~(0xFFFFFFFF)) != 0) {
                EXIT_WITH_MSG("OVERFLOW: addition result overflow!\n");
            }
            register_file[rt] = (int32_t) result;

            PRINTF_DEBUG_VERBOSE(verbose,
                                 "[SIM]\t[D]\tExecution: addi %d, %d(%d), %d\n",
                                 rt, rs, register_file[rs], imm);
            break;
        }

        case 0x9: {
            // addiu rs, rt, imm
            uint32_t rs = get_rs(b);
            uint32_t rt = get_rt(b);
            int16_t imm = get_imm(b);

            register_file[rt] = register_file[rs] + imm;

            PRINTF_DEBUG_VERBOSE(verbose,
                                 "[SIM]\t[D]\tExecution: addiu %d, %d(%d), %d\n",
                                 rt, rs, register_file[rs], imm);
            break;
        }

        case 0xa: {
            // slti rs, rt, imm
            uint32_t rs = get_rs(b);
            uint32_t rt = get_rt(b);
            int16_t imm = get_imm(b);

            if (register_file[rs] < imm)
                register_file[rt] = 1;
            else
                register_file[rt] = 0;

            PRINTF_DEBUG_VERBOSE(verbose,
                                 "[SIM]\t[D]\tExecution: slti %d(%d), %d(%d), %d\n",
                                 rt, register_file[rt], rs, register_file[rs], imm);
            break;
        }

        case 0xb: {
            // sltiu rs, rt, imm
            uint32_t rs = get_rs(b);
            uint32_t rt = get_rt(b);
            uint16_t imm = (uint16_t) get_imm(b);

            if ((uint32_t) register_file[rs] < (uint16_t) imm)
                register_file[rt] = 1;
            else
                register_file[rt] = 0;

            PRINTF_DEBUG_VERBOSE(verbose,
                                 "[SIM]\t[D]\tExecution: sltiu %d, %d(%d), %d\n",
                                 rt, rs, register_file[rs], imm);
            break;
        }

        case 0xc: {
            // andi rs, rt, imm
            uint32_t rs = get_rs(b);
            uint32_t rt = get_rt(b);
            uint16_t imm = (uint16_t) get_imm(b);

            register_file[rt] = register_file[rs] & imm;

            PRINTF_DEBUG_VERBOSE(verbose,
                                 "[SIM]\t[D]\tExecution: andi %d, %d(%d), %d\n",
                                 rt, rs, register_file[rs], imm);
            break;
        }

        case 0xd: {
            // ori rs, rt, imm
            uint32_t rs = get_rs(b);
            uint32_t rt = get_rt(b);
            uint16_t imm = (uint16_t) get_imm(b);

            register_file[rt] = register_file[rs] | imm;

            PRINTF_DEBUG_VERBOSE(verbose,
                                 "[SIM]\t[D]\tExecution: ori %d, %d(%d), %d\n",
                                 rt, rs, register_file[rs], imm);
            break;
        }

        case 0xe: {
            // xori rs, rt, imm
            uint32_t rs = get_rs(b);
            uint32_t rt = get_rt(b);
            uint16_t imm = (uint16_t) get_imm(b);

            register_file[rt] = register_file[rs] ^ imm;

            PRINTF_DEBUG_VERBOSE(verbose,
                                 "[SIM]\t[D]\tExecution: xori %d, %d(%d), %d\n",
                                 rt, rs, register_file[rs], imm);
            break;
        }

        case 0xf: {
            // lui 0x0, rt, imm
            uint32_t rt = get_rt(b);
            uint16_t imm = (uint16_t) get_imm(b);

            register_file[rt] = (imm << 16);

            PRINTF_DEBUG_VERBOSE(verbose,
                                 "[SIM]\t[D]\tExecution: lui %d, %d\n",
                                 rt, imm);
            break;
        }

        case 0x20: {
            // lb
            uint32_t rs = get_rs(b);
            uint32_t rt = get_rt(b);
            int16_t imm = get_imm(b);

            register_file[rt] = (int8_t) mmbar_read(&simulator->mmBar, imm + register_file[rs]);

            PRINTF_DEBUG_VERBOSE(verbose,
                                 "[SIM]\t[D]\tExecution: lb %d, %d[%d(%d)]\n",
                                 rt, imm, rs, register_file[rs]);
            break;
        }

        case 0x21: {
            // lh
            uint32_t rs = get_rs(b);
            uint32_t rt = get_rt(b);
            int16_t imm = get_imm(b);

            register_file[rt] = (int16_t) mmbar_readu16(&simulator->mmBar, imm + register_file[rs]);

            PRINTF_DEBUG_VERBOSE(verbose,
                                 "[SIM]\t[D]\tExecution: lh %d, %d[%d(%d)]\n",
                                 rt, imm, rs, register_file[rs]);
            break;
        }

        case 0x22: {
            // lwl
            uint32_t rs = get_rs(b);
            uint32_t rt = get_rt(b);
            int16_t imm = get_imm(b);
            uint16_t addr = imm + register_file[rs];

            uint32_t mmc = 4 - addr % 4;
            uint32_t result = 0x0;
            uint32_t mask = 0x0;
            for (uint32_t i = 0; i < mmc; i++) {
                result |= (mmbar_read(&simulator->mmBar, addr + i) << (i * 8));
                mask |= 0xFF << (i * 8);
            }
            register_file[rt] = (result) & (~mask & register_file[rt]);

            PRINTF_DEBUG_VERBOSE(verbose,
                                 "[SIM]\t[D]\tExecution: lwl %d, %d[%d(%d)]\n",
                                 rt, imm, rs, register_file[rs]);
            break;
        }

        case 0x23: {
            // lw
            uint32_t rs = get_rs(b);
            uint32_t rt = get_rt(b);
            int16_t imm = get_imm(b);

            register_file[rt] = (int32_t) mmbar_readu32(&simulator->mmBar, imm + register_file[rs]);
            PRINTF_DEBUG_VERBOSE(verbose,
                                 "[SIM]\t[D]\tExecution: lw %d, %d[%d(%d)], (%d)\n",
                                 rt, imm, rs, register_file[rs], register_file[rt]);
            break;
        }

        case 0x24: {
            // lbu
            uint32_t rs = get_rs(b);
            uint32_t rt = get_rt(b);
            int16_t imm = get_imm(b);

            register_file[rt] = (unsigned) mmbar_readu16(&simulator->mmBar, imm + register_file[rs]);
            PRINTF_DEBUG_VERBOSE(verbose,
                                 "[SIM]\t[D]\tExecution: lbu %d, %d[%d(%d)]\n",
                                 rt, imm, rs, register_file[rs]);
            break;
        }

        case 0x25: {
            // lhu
            uint32_t rs = get_rs(b);
            uint32_t rt = get_rt(b);
            int16_t imm = get_imm(b);

            register_file[rt] = (uint16_t) mmbar_readu16(&simulator->mmBar, imm + register_file[rs]);

            PRINTF_DEBUG_VERBOSE(verbose,
                                 "[SIM]\t[D]\tExecution: lhu %d, %d[%d(%d)]\n",
                                 rt, imm, rs, register_file[rs]);
            break;
        }

        case 0x26: {
            // lwr
            uint32_t rs = get_rs(b);
            uint32_t rt = get_rt(b);
            int16_t imm = get_imm(b);
            uint16_t addr = imm + register_file[rs];

            uint32_t mmc = addr % 4;
            uint32_t result = 0x0;
            uint32_t mask = 0x0;
            for (uint32_t i = 0; i < mmc + 1; i++) {
                result |= (mmbar_read(&simulator->mmBar, addr - i) << ((mmc - i) * 8));
                mask |= 0xFF << ((mmc - i) * 8);
            }
            register_file[rt] = (result) & (~mask & register_file[rt]);

            PRINTF_DEBUG_VERBOSE(verbose,
                                 "[SIM]\t[D]\tExecution: lwr %d, %d[%d(%d)]\n",
                                 rt, imm, rs, register_file[rs]);
            break;
        }

        case 0x28: {
            // sb
            uint32_t rs = get_rs(b);
            uint32_t rt = get_rt(b);
            int16_t imm = get_imm(b);
            mmbar_write(&simulator->mmBar, imm + register_file[rs], 0xFF & register_file[rt]);

            PRINTF_DEBUG_VERBOSE(verbose,
                                 "[SIM]\t[D]\tExecution: sb %d(%d), %d[%d(%d)]\n",
                                 rt, register_file[rt], imm, rs, register_file[rs]);
            break;
        }

        case 0x29: {
            // sh
            uint32_t rs = get_rs(b);
            uint32_t rt = get_rt(b);
            int16_t imm = get_imm(b);
            mmbar_writeu16(&simulator->mmBar, imm + register_file[rs], 0xFFFF & register_file[rt]);

            PRINTF_DEBUG_VERBOSE(verbose,
                                 "[SIM]\t[D]\tExecution: sh %d(%d), %d[%d(%d)]\n",
                                 rt, register_file[rt], imm, rs, register_file[rs]);
            break;
        }

        case 0x2a: {
            // swl
            uint32_t rs = get_rs(b);
            uint32_t rt = get_rt(b);
            int16_t imm = get_imm(b);
            uint16_t addr = imm + register_file[rs];

            uint32_t mmc = 4 - addr % 4;
            for (uint32_t i = 0; i < mmc; i++) {
                mmbar_write(&simulator->mmBar,
                            addr + i,
                            0xFF & (register_file[rt] >> (((addr + i) % 4) * 8)));
            }
            PRINTF_DEBUG_VERBOSE(verbose,
                                 "[SIM]\t[D]\tExecution: swl %d(%d), %d[%d(%d)]\n",
                                 rt, register_file[rt], imm, rs, register_file[rs]);
            break;
        }

        case 0x2b: {
            // sw
            uint32_t rs = get_rs(b);
            uint32_t rt = get_rt(b);
            int16_t imm = get_imm(b);
            mmbar_writeu32(&simulator->mmBar, imm + register_file[rs], (uint32_t) register_file[rt]);

            PRINTF_DEBUG_VERBOSE(verbose,
                                 "[SIM]\t[D]\tExecution: sw %d(%d), %d[%d(%d)]\n",
                                 rt, register_file[rt], imm, rs, register_file[rs]);
            break;
        }

        case 0x2e: {
            // swr
            uint32_t rs = get_rs(b);
            uint32_t rt = get_rt(b);
            int16_t imm = get_imm(b);
            uint16_t addr = imm + register_file[rs];

            uint32_t mmc = addr % 4;
            for (uint32_t i = 0; i < mmc + 1; i++) {
                mmbar_write(&simulator->mmBar,
                            addr - i,
                            0xFF & (register_file[rt] >> (((addr - i) % 4) * 8)));
            }
            PRINTF_DEBUG_VERBOSE(verbose,
                                 "[SIM]\t[D]\tExecution: swr %d(%d), %d[%d(%d)]\n",
                                 rt, register_file[rt], imm, rs, register_file[rs]);
            break;
        }

#undef get_rs
#undef get_rt
#undef get_imm

        default: {
            PRINTF_ERR_STAMP("[SIM]\tUnrecognized opcode: %d\n", get_opcode(b));
            return 0;
        }

    }
    return 1;
}

void __simulator_exec_init(Simulator *simulator) {
    simulator->pc = MEM_TEXT_START;
    mmbar_load_text(&simulator->mmBar, simulator->bin);
    memset(register_file, 0, sizeof(uint32_t) * REG_NUM);
    register_file[sp] = 0x1000000;
}

void __simulator_exec_run(Simulator *simulator) {
    while (simulator->pc != simulator->mmBar.text_end_addr) {
        uint32_t b = mmbar_readu32(&simulator->mmBar, simulator->pc);

//        PRINTF_DEBUG_VERBOSE(verbose, "[SIM]\tInstruction fetch: %d\n", simulator->pc);

        if (!decode(simulator, b)) {
            EXIT_WITH_MSG("[SIM]\tfailed to decode instruction: %s\n\t\texit...\n",
                          std::bitset<32>(b).to_string().c_str());
        }

        if (verbose) {
//            debug_dump_registers();
        }

        simulator->pc += 4;
    }
}

void __simulator_exec_finalize(Simulator *simulator) {
    mmbar_free(&simulator->mmBar);
}

void simulator_exec(Simulator *simulator) {
    if (!simulator->user_options.from_asm) {
        assembler_exec(&simulator->assembler);
        simulator->bin = simulator->assembler.bin;
    }

    if (simulator->user_options.full_flow) {
        __simulator_exec_init(simulator);
        __simulator_exec_run(simulator);
        __simulator_exec_finalize(simulator);
    }
}

void simulator_free(Simulator *simulator) {
    options_free(&simulator->user_options);
    assembler_free(&simulator->assembler);
}