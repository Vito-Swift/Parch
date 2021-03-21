/**
 * @filename: psim.hh.h
 * @author: Vito Wu <chenhaowu[at]link.cuhk.edu.cn>
 * @version: 
 * @desc:
 * @date: 2/20/2021
 */

#ifndef PARCH_PSIM_HH
#define PARCH_PSIM_HH

#include <iostream>
#include <utility>
#include <fcntl.h>

#include "assembler.hh"
#include "options.hh"
#include "mmbar.hh"

struct Simulator {
    Assembler assembler;
    MMBar mmBar;
    Options user_options;
    std::vector<std::string> inputs;
    uint32_t current_input;
    std::vector<uint32_t> bin;
    uint32_t pc;
};

void simulator_init(Simulator *simulator, int argc, char **argv);

void simulator_exec(Simulator *simulator);

void simulator_free(Simulator *simulator);

#endif //PARCH_PSIM_HH
