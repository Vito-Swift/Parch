/**
 * @filename: psim.cc.c
 * @author: Vito Wu <chenhaowu[at]link.cuhk.edu.cn>
 * @version: 
 * @desc:
 * @date: 2/20/2021
 */

#include "psim.hh"

void simulator_init(Simulator *simulator, int argc, char **argv) {
    options_init(&simulator->user_options);
    options_parse(&simulator->user_options, argc, argv);

    assembler_init(&simulator->assembler,
                   std::string(simulator->user_options.ELF),
                   simulator->user_options.from_elf);
    simulator->assembler.user_options = &simulator->user_options;
}

#define get_opcode(bin) (bin >> 26)

int32_t art_rshift(int x, int n) {
    if (x < 0 && n > 0)
        return (x >> n) | ~(~0U >> n);
    else
        return x >> n;
}

void __decode_itype(uint32_t bin, uint32_t *rs, uint32_t *rt, uint32_t *imm) {

}

void __decode_rtype(uint32_t bin, uint32_t *rs, uint32_t *rt, uint32_t *rd, uint32_t *shamt) {

}

void __decode_jtype(uint32_t bin, uint32_t *offset) {

}

bool __decode_rcluster(Simulator *simulator, uint32_t bin) {
#define get_funct(bin) (bin & 0x1F)
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
                                 "[SIM]\t\t[RC]\tExecution: sll %d, %d, %d\n",
                                 rd, rt, shamt);
            break;
        }

        case 2: {
            // srl
            register_file[rd] = (unsigned) register_file[rt] >> shamt;
            PRINTF_DEBUG_VERBOSE(verbose,
                                 "[SIM]\t\t[RC]\tExecution: srl %d, %d, %d\n",
                                 rd, rt, shamt);
            break;
        }

        case 3: {
            // sra
            register_file[rd] = art_rshift(register_file[rt], shamt);
            PRINTF_DEBUG_VERBOSE(verbose,
                                 "[SIM]\t\t[RC]\tExecution: sra %d, %d, %d\n",
                                 rd, rt, shamt);
            break;
        }

        case 4: {
            // sllv
            register_file[rd] = (unsigned) register_file[rt] << register_file[rs];
            PRINTF_DEBUG_VERBOSE(verbose,
                                 "[SIM]\t\t[RC]\tExecution: sllv %d, %d, %d\n",
                                 rd, rt, rs);
            break;
        }

        case 6: {
            // srlv
            register_file[rd] = (unsigned) register_file[rt] >> register_file[rs];
            PRINTF_DEBUG_VERBOSE(verbose,
                                 "[SIM]\t\t[RC]\tExecution srlv %d, %d, %d\n",
                                 rd, rt, rs);
            break;
        }

        case 7: {
            // srav
            register_file[rd] = art_rshift(register_file[rt], register_file[rs]);
            PRINTF_DEBUG_VERBOSE(verbose,
                                 "[SIM]\t\t[RC]\tExecution: srav %d, %d, %d(%d)\n",
                                 rd, rt, rs);
            break;
        }

        case 8: {
            // jr
            simulator->pc = register_file[rs];
            PRINTF_DEBUG_VERBOSE(verbose,
                                 "[SIM]\t\t[RC]\tExecution: jr %d(%d)\n", rs, register_file[rs]);
            break;
        }

        case 9: {
            // jalr
            register_file[rd] = simulator->pc + 4;
            simulator->pc = register_file[rs];
            break;
        }

        case 12: {
            // syscall
            // TODO: to implement
            break;
        }

        case 16: {
            // mfhi
            register_file[rd] = register_file[HI];
            PRINTF_DEBUG_VERBOSE(verbose,
                                 "[SIM]\t\t[RC]\tExecution: mfhi %d\n", rd);
            break;
        }

        case 17: {
            // mthi
            register_file[HI] = register_file[rs];
            PRINTF_DEBUG_VERBOSE(verbose,
                                 "[SIM]\t\t[RC]\tExecution: mthi %d(%d)\n", rs, register_file[rs]);
            break;
        }

        case 18: {
            // mflo
            register_file[rd] = register_file[LO];
            PRINTF_DEBUG_VERBOSE(verbose,
                                 "[SIM]\t\t[RC]\tExecution: mflo %d\n", rd);
            break;
        }

        case 19: {
            // mtlo
            register_file[LO] = register_file[rs];
            PRINTF_DEBUG_VERBOSE(verbose,
                                 "[SIM]\t\t[RC]\tExecution: mtlo %d(%d)\n", rs, register_file[rs]);
            break;
        }

        case 24: {
            // mult
            int64_t result = (int32_t) register_file[rs] * (int32_t) register_file[rt];
            register_file[HI] = result >> 32;
            register_file[LO] = result & 0xFFFFFFFF;
            PRINTF_DEBUG_VERBOSE(verbose,
                                 "[SIM]\t\t[RC]\tExecution: mult %d(%d), %d(%d)\n",
                                 rs, register_file[rs], rt, register_file[rt]);
            break;
        }

        case 25: {
            // multu
            int64_t result = (uint32_t) register_file[rs] * (uint32_t) register_file[rt];
            register_file[HI] = result >> 32;
            register_file[LO] = result & 0xFFFFFFFF;
            PRINTF_DEBUG_VERBOSE(verbose,
                                 "[SIM]\t\t[RC]\tExecution: multu %d(%d), %d(%d)\n",
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
                                 "[SIM]\t\t[RC]\tExecution: div %d(%d), %d(%d)\n",
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
                                 "[SIM]\t\t[RC]\tExecution: divu %d(%d), %d(%d)\n",
                                 rs, register_file[rs], rt, register_file[rt]);
            break;
        }

        case 32: {
            // add
            int64_t result = register_file[rs] + register_file[rt];
            if ((result & ~(0xFFFFFFFF)) != 0) {
                EXIT_WITH_MSG("OVERFLOW: addition result overflow!\n");
            }

            register_file[rd] = (int32_t) result;
            PRINTF_DEBUG_VERBOSE(verbose,
                                 "[SIM]\t\t[RC]\tExecution: add %d, %d(%d), %d(%d)\n",
                                 rd, rs, register_file[rs], rt, register_file[rt]);
            break;
        }

        case 33: {
            // addu
            register_file[rd] = register_file[rs] + register_file[rt];
            PRINTF_DEBUG_VERBOSE(verbose,
                                 "[SIM]\t\t[RC]\tExecution: addu %d, %d(%d), %d(%d)\n",
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
                                 "[SIM]\t\t[RC]\tExecution: sub %d, %d(%d), %d(%d)\n",
                                 rd, rs, register_file[rs], rt, register_file[rt]);
            break;
        }

        case 35: {
            // subu
            register_file[rd] = register_file[rs] - register_file[rt];
            PRINTF_DEBUG_VERBOSE(verbose,
                                 "[SIM]\t\t[RC]\tExecution: subu %d, %d(%d), %d(%d)\n",
                                 rd, rs, register_file[rs], rt, register_file[rt]);
            break;
        }

        case 36: {
            // and
            register_file[rd] = register_file[rs] & register_file[rt];
            PRINTF_DEBUG_VERBOSE(verbose,
                                 "[SIM]\t\t[RC]\tExecution: and %d, %d(%d), %d(%d)\\n\",\n",
                                 rd, rs, register_file[rs], rt, register_file[rt]);
            break;
        }

        case 37: {
            // or
            register_file[rd] = register_file[rs] | register_file[rt];
            PRINTF_DEBUG_VERBOSE(verbose,
                                 "[SIM]\t\t[RC]\tExecution: or %d, %d(%d), %d(%d)\\n\",\n",
                                 rd, rs, register_file[rs], rt, register_file[rt]);
            break;
        }

        case 38: {
            // xor
            register_file[rd] = register_file[rs] ^ register_file[rt];
            PRINTF_DEBUG_VERBOSE(verbose,
                                 "[SIM]\t\t[RC]\tExecution: xor %d, %d(%d), %d(%d)\\n\",\n",
                                 rd, rs, register_file[rs], rt, register_file[rt]);
            break;
        }

        case 39: {
            // nor
            register_file[rd] = ~(register_file[rs] | register_file[rt]);
            PRINTF_DEBUG_VERBOSE(verbose,
                                 "[SIM]\t\t[RC]\tExecution: nor %d, %d(%d), %d(%d)\\n\",\n",
                                 rd, rs, register_file[rs], rt, register_file[rt]);
            break;
        }

        case 42: {
            // slt
            if (register_file[rs] < register_file[rt])
                register_file[rd] = 1;
            PRINTF_DEBUG_VERBOSE(verbose,
                                 "[SIM]\t\t[RC]\tExecution: slt %d, %d(%d), %d(%d)\\n\",\n",
                                 rd, rs, register_file[rs], rt, register_file[rt]);
            break;
        }

        case 43: {
            // sltu
            if ((uint32_t) register_file[rs] < (uint32_t) register_file[rt])
                register_file[rd] = 1;
            PRINTF_DEBUG_VERBOSE(verbose,
                                 "[SIM]\t\t[RC]\tExecution: sltu %d, %d(%d), %d(%d)\\n\",\n",
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
            PRINTF_ERR_STAMP("[SIM]\t[RC]\tUnrecognized funct domain: %d\n", funct);
            return 0;
        }

    }

    return 1;
#undef get_funct
#undef get_rs
#undef get_rt
#undef get_rd
#undef get_shamt
}

bool __decode_branch_trap(Simulator *simulator, uint32_t bin) {

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
            break;
        }

        case 0x3: {
            // jal target
            break;
        }

        case 0x4: {
            // beq rs, rt, offset
            break;
        }

        case 0x5: {
            // bne, rs, rt, offset
            break;
        }

        case 0x6: {
            // blez, rs, 0x0, offset
            break;
        }

        case 0x7: {
            // bgtz, rs, 0x0, offset
            break;
        }

        case 0x8: {
            // addi, rs, rt, imm
            break;
        }

        case 0x9: {
            // addiu rs, rt, imm
            break;
        }

        case 0xa: {
            // slti rs, rt, imm
            break;
        }

        case 0xb: {
            // sltiu rs, rt, imm
            break;
        }

        case 0xc: {
            // andi rs, rt, imm
            break;
        }

        case 0xd: {
            // ori rs, rt, imm
            break;
        }

        case 0xe: {
            // xori rs, rt, imm
            break;
        }

        case 0xf: {
            // lui 0x0, rt, imm
            break;
        }

        case 0x20: {
            // lb
            break;
        }

        case 0x21: {
            // lh
            break;
        }

        case 0x22: {
            // lwl
            break;
        }

        case 0x23: {
            // lw
            break;
        }

        case 0x24: {
            // lbu
            break;
        }

        case 0x25: {
            // lhu
            break;
        }

        case 0x26: {
            // lwr
            break;
        }

        case 0x28: {
            // sb
            break;
        }

        case 0x29: {
            // sh
            break;
        }

        case 0x2a: {
            // swl
            break;
        }

        case 0x2b: {
            // sw
            break;
        }

        default: {
            PRINTF_ERR_STAMP("[SIM]\t\tUnrecognized opcode: %d\n", get_opcode(b));
            return 0;
        }
    }
    return 1;
}

void __simulator_exec_init(Simulator *simulator) {
    simulator->pc = MEM_TEXT_START;
    mmbar_init(&simulator->mmBar);
    mmbar_load_text(&simulator->mmBar, simulator->bin);
    memset(register_file, 0, sizeof(uint32_t) * REG_NUM);
}

void __simulator_exec_run(Simulator *simulator) {
    while (simulator->pc != simulator->mmBar.text_end_addr) {
        uint32_t b = mmbar_readu32(&simulator->mmBar, simulator->pc);

        if (!decode(simulator, b)) {
            EXIT_WITH_MSG("[SIM]\t\tfailed to decode instruction: %s\n\t\texit...\n",
                          std::bitset<32>(b).to_string().c_str());
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