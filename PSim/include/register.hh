/**
 * @filename: register.hh.h
 * @author: Vito Wu <chenhaowu[at]link.cuhk.edu.cn>
 * @version: 
 * @desc:
 * @date: 2/24/2021
 */

#ifndef PARCH_REGISTER_HH
#define PARCH_REGISTER_HH

#include <map>

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

inline std::map<std::string, uint32_t> create_regparse_map() {
    std::map<std::string, uint32_t> rgm;
    rgm["zero"] = zero;
    rgm["at"] = at;
    rgm["a0"] = a0, rgm["a1"] = a1, rgm["a2"] = a2, rgm["a3"] = a3;
    rgm["t0"] = t0, rgm["t1"] = t1, rgm["t2"] = t2, rgm["t3"] = t3, rgm["t4"] = t4, rgm["t5"] = t5;
    rgm["t6"] = t6, rgm["t7"] = t7, rgm["t8"] = t8, rgm["t9"] = t9;
    rgm["s0"] = s0, rgm["s1"] = s1, rgm["s2"] = s2, rgm["s3"] = s3, rgm["s4"] = s4, rgm["s5"] = s5;
    rgm["s6"] = s6, rgm["s7"] = s7;
    rgm["k0"] = k0, rgm["k1"] = k1;
    rgm["gp"] = gp;
    rgm["sp"] = sp;
    rgm["fp"] = fp;
    rgm["ra"] = ra;
    return rgm;
}

#endif //PARCH_REGISTER_HH
