/**
 * @filename: options.cc
 * @author: Vito Wu <chenhaowu[at]link.cuhk.edu.cn>
 * @version: 0.1
 * @desc:
 * @date: 2/20/2021
 */

#include "options.hh"

static void print_usage(char *prg_name) {

}

enum opt_types {
    OP_ELF = 1,
    OP_VERBOSE,
    OP_FUNCTION_ONLY,
    OP_HAZARD,
    OP_OOOE
};

static struct option parch_long_opts[] = {
        {"ELF",           required_argument, 0, OP_ELF},
        {"verbose",       no_argument,       0, OP_VERBOSE},
        {"function_only", no_argument,       0, OP_FUNCTION_ONLY},
        {"hazard_sim",    no_argument,       0, OP_HAZARD},
        {"OoOE_sim",      no_argument,       0, OP_OOOE}
};

void options_init(Options *options) {

}

void options_free(Options *options) {

}

void options_parse(Options *options, int argc, char **argv) {
    int c, opt_idx;
    if (argc == 1) {
        print_usage(argv[0]);
        exit(0);
    }

}
