/**
 * @filename: register.hh.h
 * @author: Vito Wu <chenhaowu[at]link.cuhk.edu.cn>
 * @version: 
 * @desc:
 * @date: 2/24/2021
 */

#ifndef PARCH_REGISTER_HH
#define PARCH_REGISTER_HH

enum register_type {
    zero,                                   // constant 0
    at,                                     // reserved for assembler
    v0, v1,                                 // expression evaluation and results of a function
    a0, a1, a2, a3,                         // arguments
    t0, t1, t2, t3, t4, t5, t6, t7,         // temporary registers
    s0, s1, s2, s3, s4, s5, s6, s7,         // saved temporary registers
    t8, t9,                                 // temporary registers
    k0, k1,                                 // preserved for OS kernel
    gp,                                     // pointer to global area
    sp,                                     // stack pointer
    fp,                                     // frame pointer
    ra,                                     // return address
};

#endif //PARCH_REGISTER_HH
