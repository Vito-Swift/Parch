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
    std::cout << param.ELF_file << std::endl;
    std::cout << param.tst_file << std::endl;
}

INSTANTIATE_TEST_SUITE_P (
        InstantiateAssemblerTest,
        AssemblerTest,
        ::testing::Values(
                testparam_t("testfiles/assembler/1.in", "testfiles/assembler/1.out"),
                testparam_t("testfiles/assembler/2.in", "testfiles/assembler/2.out"),
                testparam_t("testfiles/assembler/3.in", "testfiles/assembler/3.out"),
                testparam_t("testfiles/assembler/4.in", "testfiles/assembler/4.out"),
                testparam_t("testfiles/assembler/5.in", "testfiles/assembler/5.out"),
                testparam_t("testfiles/assembler/6.in", "testfiles/assembler/6.out"),
                testparam_t("testfiles/assembler/7.in", "testfiles/assembler/7.out"),
                testparam_t("testfiles/assembler/8.in", "testfiles/assembler/8.out"),
                testparam_t("testfiles/assembler/9.in", "testfiles/assembler/9.out"),
                testparam_t("testfiles/assembler/10.in", "testfiles/assembler/10.out"),
                testparam_t("testfiles/assembler/11.in", "testfiles/assembler/11.out"),
                testparam_t("testfiles/assembler/12.in", "testfiles/assembler/12.out")
        )
);

int main(int argc, char **argv) {
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}