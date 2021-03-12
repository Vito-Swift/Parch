//
// Created by Vito Wu on 2021/3/10.
//

#ifndef PARCH_OPCODE_H
#define PARCH_OPCODE_H

#include <map>

std::map<std::string, std::vector<uint32_t>> rtype_opcode;
std::map<std::string, std::vector<uint32_t>> itype_opcode;

void init_opcode_map();


#endif //PARCH_OPCODE_H
