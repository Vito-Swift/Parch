/**
 * @filename: options.hh
 * @author: Vito Wu <chenhaowu[at]link.cuhk.edu.cn>
 * @version: 0.1
 * @desc:
 * @date: 2/20/2021
 */

#ifndef PARCH_OPTIONS_HH
#define PARCH_OPTIONS_HH

#include <stdint.h>
#include <cstdio>
#include <stdlib.h>
#include <getopt.h>
#include <string.h>
#include <time.h>
#include <stdbool.h>

#include "utils.hh"

typedef struct {
    char *ELF;
    char* ASM;
    bool from_elf;
    bool from_std_in;
    bool from_asm;
    bool full_flow;
    bool assembly_only;
    bool function_only;
    bool enable_hazard;
    bool enable_OoOE;
} Options;

extern bool verbose;

void options_init(Options *options);

void options_free(Options *options);

void options_parse(Options *options, int argc, char **argv);

#endif //PARCH_OPTIONS_HH
