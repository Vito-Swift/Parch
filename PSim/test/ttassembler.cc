/**
 * @filename: ttassembler.cc.c
 * @author: Vito Wu <chenhaowu[at]link.cuhk.edu.cn>
 * @version: 
 * @desc:
 * @date: 2/28/2021
 */

#include <gtest/gtest.h>
#include <string>
#include <iostream>
#include <vector>

#include "assembler.hh"
#include "utils.hh"

struct testparam_t {
    testparam_t(std::string elf, std::string tst) {
        ELF_file = elf;
        tst_file = tst;
    }

    std::string ELF_file;
    std::string tst_file;
};

class AssemblerTest : public ::testing::TestWithParam<testparam_t> {
};

TEST_P(AssemblerTest, HandleTrueReturn) {
    testparam_t param = GetParam();

    std::string path;
    currentPath(path);
    PRINTF_STAMP("Test execution path: %s\n", path.c_str());
    PRINTF_STAMP("ELF: %s\n", param.ELF_file.c_str());
    PRINTF_STAMP("TST: %s\n", param.tst_file.c_str());

    std::vector<uint32_t> tst_bin;
    ASSERT_TRUE(isFileExist(param.tst_file));
    std::ifstream infile(param.tst_file);
    std::string line;
    while (std::getline(infile, line)) {
        uint32_t bin = std::stoi(line, nullptr, 2);
        tst_bin.push_back(bin);
    }

    Assembler assembler;
    assembler_init(&assembler, param.ELF_file, true);
    assembler_exec(&assembler);

    if (tst_bin.size() != assembler.bin.size()) {

    }
    ASSERT_EQ(tst_bin.size(), assembler.bin.size());

    for (uint32_t i = 0; i < tst_bin.size(); i++) {
        EXPECT_EQ(tst_bin[i], assembler.bin[i]);
    }

    assembler_free(&assembler);
}

INSTANTIATE_TEST_SUITE_P (
        InstantiateAssemblerTest,
        AssemblerTest,
        ::testing::Values(
                testparam_t("testfiles/ttassembler/1.in", "testfiles/ttassembler/1.out"),
                testparam_t("testfiles/ttassembler/2.in", "testfiles/ttassembler/2.out"),
                testparam_t("testfiles/ttassembler/3.in", "testfiles/ttassembler/3.out"),
                testparam_t("testfiles/ttassembler/4.in", "testfiles/ttassembler/4.out"),
                testparam_t("testfiles/ttassembler/5.in", "testfiles/ttassembler/5.out"),
                testparam_t("testfiles/ttassembler/6.in", "testfiles/ttassembler/6.out"),
                testparam_t("testfiles/ttassembler/7.in", "testfiles/ttassembler/7.out"),
                testparam_t("testfiles/ttassembler/8.in", "testfiles/ttassembler/8.out"),
                testparam_t("testfiles/ttassembler/9.in", "testfiles/ttassembler/9.out"),
                testparam_t("testfiles/ttassembler/10.in", "testfilestt/assembler/10.out"),
                testparam_t("testfiles/ttassembler/11.in", "testfilestt/assembler/11.out"),
                testparam_t("testfiles/ttassembler/12.in", "testfilestt/assembler/12.out")
        )
);

int main(int argc, char **argv) {
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}