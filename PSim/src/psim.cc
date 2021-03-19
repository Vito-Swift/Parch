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

void __simulator_exec_init(Simulator *simulator) {

}

#define get_opcode(bin) (bin >> 26)

void __decode_itype(uint32_t bin, uint32_t *rs, uint32_t *rt, uint32_t *imm) {

}

void __decode_rtype(uint32_t bin, uint32_t *rs, uint32_t *rt, uint32_t *rd, uint32_t *shamt) {

}

void __decode_jtype(uint32_t bin, uint32_t *offset) {

}

void __decode_rcluster() {

}

void __decode_branch_trap() {

}

bool __simulator_exec_run(Simulator *simulator) {
    for (uint32_t b: simulator->bin) {

        if (b == 0xc) {
            // syscall
            continue;
        }

        switch (get_opcode(b)) {

            case 0x0: {
                // r type
            }

            case 0x1: {
                // branch / trap
            }

            case 0x2: {
                // j target
            }

            case 0x3: {
                // jal target
            }

            case 0x4: {
                // beq rs, rt, offset
            }

            case 0x5: {
                // bne, rs, rt, offset
            }

            case 0x6: {
                // blez, rs, 0x0, offset
            }

            case 0x7: {
                // bgtz, rs, 0x0, offset
            }

            case 0x8: {
                // addi, rs, rt, imm
            }

            case 0x9: {
                // addiu rs, rt, imm
            }

            case 0xa: {
                // slti rs, rt, imm
            }

            case 0xb: {
                // sltiu rs, rt, imm
            }

            case 0xc: {
                // andi rs, rt, imm
            }

            case 0xd: {
                // ori rs, rt, imm
            }

            case 0xe: {
                // xori rs, rt, imm
            }

            case 0xf: {
                // lui 0x0, rt, imm
            }

            case 0x20: {
                // lb
            }

            case 0x21: {
                // lh
            }

            case 0x22: {
                // lwl
            }

            case 0x23: {
                // lw
            }

            case 0x24: {
                // lbu
            }

            case 0x25: {
                // lhu
            }

            case 0x26: {
                // lwr
            }

            case 0x28: {
                // sb
            }

            case 0x29: {
                // sh
            }

            case 0x2a: {
                // swl
            }

            case 0x2b: {
                // sw
            }

            default: {
                PRINTF_ERR_STAMP("[SIM]\t\tUnrecognized opcode: %d\n", get_opcode(b));
                return 0;
            }
        }
    }

    return 1;
}

void __simulator_exec_finalize(Simulator *simulator) {

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