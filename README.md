# PArch

A MIPS assembler and simulator implemented with C/C++. Submitted as a course project in CUHK(SZ) CSC3050 Spring 2021.

---

## Introduction

In this assignment, we are required to implement an assembler that interprets MIPS assembly files into their corresponding binary executable, as well as a simulator that accepts the executable and simulates the execution of MIPS instructions.

## Implementation Illustration

![assemble_flow.png](https://github.com/Vito-Swift/Parch/raw/master/doc/asm_flow.png "Assembling Flow")
![simulation_flow.png](https://github.com/Vito-Swift/Parch/raw/master/doc/sim_flow.png "Simulation Flow")

## Kick Start

To simplify the testing flow for people who do not familiar with the implementation details but wish to test our implementation with minimal efforts, we include an executable in the cmakelists which will be built by default.

```
$ mkdir build && cd build && cmake .. && make
```

The content of this executable is shown as follows:

```c++
#include "psim.hh"

int main(int argc, char** argv) {
	Simulator simulator;
	simulator_init(&simulator, argc, argv);
	simulator_exec(&simulator);
	simulator_free(&simulator);
}
```

In general, this executable will initialize a simulator instance and accept given command line options. According to the user arguments, the program will proceed the subsequent flow of assembler and simulator. In specific, our simulator accepts following command line options:

```
Options:

  --ELF [ELF_PATH]
               Use ELF file to launch

  --input_file [INPUT_FILE]
               The input file argument specifies
               the path to a file which pre-
               stores user inputs in the simulation

  --full_flow
               Specify this option to enable
               both assembling and simulation in
               a run
               (If this option is NOT specifeed
               , the program will omit the
               simulation and only assemble the
               input ELF file)

  --output_bin [OUTPUT_FILE]
               Specify the path to the output
               binary executable of the program

  --output_stdout [OUTPUT_FILE]
               Specify the path to the standard
               output file of the program

  --verbose
               Specify this option to enable
               a detailed and informative
               logging during the program
               execution
               (default to false)

  --help
               Show this message
```

## Examples:

1. **Assemble 1.in to its binary**
```bash
$ ./simulator --ELF testfiles/ttassembler/1.in --output_bin 1.out
```
2. **Assemble and simulate a-plus-b.asm**
```bash
$ ./simulator --full_flow --ELF a-plus-b.asm --input_file a-plus-b.in --output_stdout a-plus-b.out
```

3. **Assemble and simulate a-plus-b.asm (and show the result in stdout)**
```bash
./simulator --full_flow --ELF a-plus-b.asm --input_file a-plus-b.in
```