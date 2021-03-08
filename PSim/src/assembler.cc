/**
 * @filename: assembler.cc.c
 * @author: Vito Wu <chenhaowu[at]link.cuhk.edu.cn>
 * @version: 
 * @desc:
 * @date: 2/28/2021
 */

#include "assembler.hh"

void parse_file(assembler *as) {

}

void init_assembler(assembler *as, std::string ELF_path) {
    as->ELF_path = ELF_path;
    parse_file(as);
}