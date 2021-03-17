/**
 * @filename: ttintegration.cc.c
 * @author: Vito Wu <chenhaowu[at]link.cuhk.edu.cn>
 * @version:
 * @desc:
 * @date: 3/9/2021
 */

#include "psim.hh"

int main(int argc, char** argv) {
    Simulator simulator;
    simulator_init(&simulator, argc, argv);
    simulator_exec(&simulator);
    simulator_free(&simulator);
}
