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
#define get_funct(bin) (bin & 0x1F)

void __decode_itype(uint32_t bin, uint32_t *rs, uint32_t *rt, uint32_t *imm) {

}

void __decode_rtype(uint32_t bin, uint32_t *rs, uint32_t *rt, uint32_t *rd, uint32_t *shamt) {

}

void __decode_jtype(uint32_t bin, uint32_t *offset) {

}

bool __decode_rcluster(Simulator *simulator, uint32_t bin) {
    uint32_t funct = get_funct(bin);

    switch (funct) {

        case 0: {
            // sll
            break;
        }

        case 2: {
            // srl
            break;
        }

        case 3: {
            // sra
            break;
        }

        case 4: {
            // sllv
            break;
        }

        case 6: {
            // srlv
            break;
        }

        case 7: {
            // srav
            break;
        }

        case 8: {
            // jr
            break;
        }

        case 9: {
            // jalr
            break;
        }

        case 12: {
            // syscall
            break;
        }

        case 16: {
            // mfhi
            break;
        }

        case 17: {
            // mthi
            break;
        }

        case 18: {
            // mflo
            break;
        }

        case 19: {
            // mtlo
            break;
        }

        case 24: {
            // mult
            break;
        }

        case 25: {
            // multu
            break;
        }

        case 26: {
            // div
            break;
        }

        case 27: {
            // divu
            break;
        }

        case 32: {
            // add
            break;
        }

        case 33: {
            // addu
            break;
        }

        case 34: {
            // sub
            break;
        }

        case 35: {
            // sub
            break;
        }

        case 36: {
            // subu
            break;
        }

        case 37: {
            // or
            break;
        }

        case 38: {
            // xor
            break;
        }

        case 39: {
            // nor
            break;
        }

        case 42: {
            // slt
            break;
        }

        case 43: {
            // sltu
            break;
        }

        case 48: {
            // tge
            break;
        }

        case 50: {
            // tlt
            break;
        }

        case 51: {
            // tltu
            break;
        }

        case 52: {
            // teq
            break;
        }

        case 54: {
            // tne
            break;
        }

        default: {
            PRINTF_ERR_STAMP("[SIM]\t[RC]\tUnrecognized funct domain: %d\n", funct);
            return 0;
        }

    }

    return 1;
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