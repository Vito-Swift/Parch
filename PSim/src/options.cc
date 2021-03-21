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
           "  --ELF [ELF_PATH]                             \n"
           "               Use ELF file to launch          \n"
           "                                               \n"
           "  --input_file [INPUT_FILE]                    \n"
           "               The input file argument specifies\n"
           "               the path to a file which pre-    \n"
           "               stores user inputs in the simulation\n"
           "                                               \n"
           "  --full_flow                                  \n"
           "               Specify this option to enable   \n"
           "               both assembling and simulation in\n"
           "               a run                           \n"
           "               (If this option is NOT specifeed\n"
           "               , the program will omit the     \n"
           "               simulation and only assemble the\n"
           "               input ELF file)                 \n"
           "                                               \n"
           "  --output_bin [OUTPUT_FILE]                   \n"
           "               Specify the path to the output  \n"
           "               binary executable of the program\n"
           "                                               \n"
           "  --output_stdout [OUTPUT_FILE]                \n"
           "               Specify the path to the standard\n"
           "               output file of the program      \n"
           "                                               \n"
           "  --verbose                                    \n"
           "               Specify this option to enable   \n"
           "               a detailed and informative      \n"
           "               logging during the program      \n"
           "               execution                       \n"
           "               (default to false)              \n"
           "                                               \n"
           "  --help                                       \n"
           "               Show this message               \n"
           "                                               \n"
           "                                               \n"
           "Example:                                       \n"
           "   1) Assemble testfiles/ttassembler/1.in to its binary:\n"
           "       > ./%s --ELF testfiles/ttassembler/1.in --output_bin 1.out\n"
           "                                               \n"
           "   2) Assemble and simulate a-plus-b.asm       \n"
           "       > ./%s --ELF a-plus-b.asm --input_file a-plus-b.in\n"
           "            --output_stdout a-plus-b.out       \n"
           "                                               \n"
           "   3) Assemble and simualte a-plus-b.asm       \n"
           "      (and show the result in stdout)          \n"
           "       > ./%s --ELF a-plus-b.asm --input_file a-plus-b.in\n"
           "                                               \n",
           prg_name, prg_name, prg_name, prg_name);
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
    OP_OUTPUT_BIN,
    OP_OUTPUT_STDOUT
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
        {"output_bin", required_argument, 0, OP_OUTPUT_BIN},
        {"output_stdout", required_argument, 0, OP_OUTPUT_STDOUT},
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
    options->require_output_bin = false;
    options->require_output_stdout = false;
}

void options_free(Options *options) {
//    if (options->ELF)
//        free(options->ELF);
//
//    if (options->ASM)
//        free(options->ASM);
//
//    if (options->input_file)
//        free(options->input_file);
//
//    if (options->output_bin)
//        free(options->output_bin);
//
//    if (options->output_stdout)
//        free(options->output_stdout);
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

            case OP_OUTPUT_BIN:
                options->require_output_bin = true;
                copy_opt(&options->output_bin, optarg);
                break;

            case OP_OUTPUT_STDOUT:
                options->require_output_stdout = true;
                copy_opt(&options->output_stdout, optarg);
                break;

            case '?':
                break;

            default:
                EXIT_WITH_MSG("[!] unknown error, exit...\n");
        }
    }

    options_validate(options);
}
