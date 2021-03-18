/**
 * @filename: mmbar.cc.c
 * @author: Vito Wu <chenhaowu[at]link.cuhk.edu.cn>
 * @version: 
 * @desc:
 * @date: 2/24/2021
 */

#include "mmbar.hh"

void __reset_mmcounters(MMBar *mmBar) {
    mmBar->text_end_addr = MEM_TEXT_START;
    mmBar->static_end_addr = MEM_DATA_START;
    mmBar->dynamic_end_addr = MEM_DATA_START;
}

bool mmbar_write(MMBar* mmBar, uint32_t addr, uint8_t c) {
    if (!mmBar->initialized) {
        PRINTF_DEBUG_VERBOSE(verbose,
                             "[MMBAR]\t\tMemory has not been initialized yet!\n");
        return false;
    }

    if (addr >= MEM_SIZE) {
        PRINTF_DEBUG_VERBOSE(verbose,
                             "[MMBAR]\t\tWrite address index out of range: %d\n",
                             addr);
        return false;
    }

    mmBar->_memory[addr] = c;
    return true;
}

uint8_t mmbar_read(MMBar* mmBar, uint32_t addr) {
    if (!mmBar->initialized) {
        PRINTF_DEBUG_VERBOSE(verbose,
                             "[MMBAR]\t\tMemory has not been initialized yet!\n");
        return 0x0;
    }

    if (addr >= MEM_SIZE) {
        PRINTF_DEBUG_VERBOSE(verbose,
                             "[MMBAR]\t\tWrite address index out of range: %d\n",
                             addr);
        return 0x0;
    }

    return mmBar->_memory[addr];
}

void mmbar_load_text(MMBar *mmBar, std::vector<std::uint32_t> bin) {
#define HIHI_MASK 0xFF000000UL
#define HILO_MASK 0xFF0000UL
#define LOHI_MASK 0xFF00UL
#define LOLO_MASK 0xFFUL

}

void mmbar_init(MMBar *mmBar) {
    mmBar->_memory = SMALLOC(uint8_t, MEM_SIZE);
    mmBar->initialized = true;
    __reset_mmcounters(mmBar);
}

void mmbar_free(MMBar *mmBar) {
    mmBar->initialized = false;
    SFREE(mmBar->_memory);
    __reset_mmcounters(mmBar);
}
