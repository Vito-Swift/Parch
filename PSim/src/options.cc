/**
 * @filename: options.cc
 * @author: Vito Wu <chenhaowu[at]link.cuhk.edu.cn>
 * @version: 0.1
 * @desc:
 * @date: 2/20/2021
 */

#include "options.hh"

bool verbose = false;

static void print_usage(char *prg_name) {
    printf("\n"
           "Usage: %s [OPTIONS]                \n"
           "\n"
           "\n"
           "Options:                                       \n"
           "                                               \n"
           "  --ELF [ELF_PATH] or --from_std_in            \n"
           "               Use ELF file to launch or       \n"
           "               read asm from standard input    \n"
           "               (default to read from stdin)    \n"
           "                                               \n"
           "    or --ASM [ASM_PATH]                        \n"
           "               Execute assembled binary        \n"
           "                                               \n"
           "  --full_flow                                  \n"
           "               If ELF file is specified, proceed\n"
           "               the assembling and execution in \n"
           "               one step                        \n"
           "                                               \n"
           "  --verbose                                    \n"
           "               Enable DEBUG logging            \n"
           "               (default to false)              \n"
           "                                               \n"
           "                                               \n"
           "Example:                                       \n"
           "  ./PSim --ELF test/testfiles/ttsimulator/a-plus-b.asm \n"
           "        --verbose                              \n"
           "                                               \n",
           prg_name);
}

static inline void copy_opt(char **str, char *optarg) {
    if (NULL == ((*str) = strndup(optarg, 1024))) {
        PRINTF_ERR("[!] invalid input parameter\n");
    }
}

enum opt_types {
    OP_ELF = 1,
    OP_INPUT_FILE,
    OP_VERBOSE,
    OP_FUNCTION_ONLY,
    OP_HAZARD,
    OP_OOOE,
    OP_STDIN,
    OP_FULL_FLOW,
};

static struct option parch_long_opts[] = {
        {"ELF", required_argument, 0, OP_ELF},
        {"input_file", required_argument, 0, OP_INPUT_FILE},
        {"verbose", no_argument, 0, OP_VERBOSE},
        {"function_only", no_argument, 0, OP_FUNCTION_ONLY},
        {"hazard_sim", no_argument, 0, OP_HAZARD},
        {"OoOE_sim", no_argument, 0, OP_OOOE},
        {"from_std_in", no_argument, 0, OP_STDIN},
        {"full_flow", no_argument, 0, OP_FULL_FLOW},
};

void options_init(Options *options) {
    options->ELF = NULL;
    options->from_elf = false;
    options->from_std_in = false;
    options->full_flow = false;
    options->from_asm = false;
    options->function_only = false;
    options->enable_OoOE = false;
    options->enable_hazard = false;
}

void options_free(Options *options) {

}

bool options_validate(Options *options) {
    PRINTF_DEBUG_VERBOSE(verbose, "validate");

    if (!(options->from_elf) && !(options->from_std_in) && !(options->from_asm)) {
        EXIT_WITH_MSG("[!] neither ELF, stdin or asm file is specified, please specify...\n");
    }

    if ((options->from_elf || options->from_std_in) && options->from_asm) {
        EXIT_WITH_MSG("[!] cannot read asm and bin simultaneously, exit\n");
    }

    if (options->from_elf && options->from_std_in) {
        EXIT_WITH_MSG("[!] both ELF and stdin are specified, please only choose one mode...\n");
    }

    if (options->enable_OoOE || options->enable_hazard || options->function_only) {
        PRINTF_ERR_STAMP("--enable_OoOE or --enable_hazard is specified\n");
        PRINTF_ERR_STAMP("  but PSim does not seem to support these options in this version.\n");
    }

    if (options->from_elf) {
        PRINTF_DEBUG_VERBOSE(verbose, "[OPT]\tOption enabled: read from ELF\n");
    }

    if (options->from_std_in) {
        PRINTF_DEBUG_VERBOSE(verbose, "[OPT]\tOption enabled: read from stdin\n");
    }

    if (options->from_asm) {
        PRINTF_DEBUG_VERBOSE(verbose, "[OPT]\tOption enabled: read from assembled file\n");
    }

    if (options->assembly_only) {
        PRINTF_DEBUG_VERBOSE(verbose, "[OPT]\tOption enabled: only assembling into binary executable\n");
    }

    return 0;
}

void options_parse(Options *options, int argc, char **argv) {
    int c, opt_idx;
    if (argc == 1) {
        print_usage(argv[0]);
        exit(0);
    }
    while (-1 != (c = getopt_long(argc, argv, "h", parch_long_opts, &opt_idx))) {
        switch (c) {
            case 0:
                if (parch_long_opts[opt_idx].flag == 0) {
                    PRINTF_STAMP("\toption %s: %s\n", parch_long_opts[opt_idx].name,
                                 optarg ? optarg : "null");
                }
                break;

            case 'h':
                print_usage(argv[0]);
                options_free(options);
                exit(0);

            case OP_ELF:
                copy_opt(&options->ELF, optarg);
                options->from_elf = true;
                break;

            case OP_INPUT_FILE:
                copy_opt(&options->input_file, optarg);
                options->input_from_file = true;
                break;

            case OP_STDIN:
                options->from_std_in = true;
                break;

            case OP_VERBOSE:
                verbose = true;
                PRINTF_DEBUG_VERBOSE(verbose, "[OPT]\tOption enabled: verbose\n");
                break;

            case OP_FUNCTION_ONLY:
                options->function_only = true;
                break;

            case OP_FULL_FLOW:
                options->full_flow = true;
                break;

            case '?':
                break;

            default:
                EXIT_WITH_MSG("[!] unknown error, exit...\n");
        }
    }

    options_validate(options);
}
