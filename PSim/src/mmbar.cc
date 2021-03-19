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

bool mmbar_write(MMBar *mmBar, uint32_t addr, uint8_t c) {
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

bool mmbar_writeu16(MMBar *mmBar, uint32_t addr, uint16_t e) {
    if (!mmBar->initialized) {
        PRINTF_DEBUG_VERBOSE(verbose,
                             "[MMBAR]\t\tMemory has not been initialized yet!\n");
        return false;
    }

    if (addr + 2 >= MEM_SIZE) {
        PRINTF_DEBUG_VERBOSE(verbose,
                             "[MMBAR]\t\tWrite u16 address index out of range: %d\n",
                             addr);
        return false;
    }

#define LOLO_MASK 0xFFUL
    uint8_t bllh = (e >> 8) & LOLO_MASK;
    uint8_t blll = e & LOLO_MASK;

    mmBar->_memory[addr] = blll;
    mmBar->_memory[addr + 1] = bllh;

    return 1;
}

bool mmbar_writeu32(MMBar *mmBar, uint32_t addr, uint32_t e) {
    if (!mmBar->initialized) {
        PRINTF_DEBUG_VERBOSE(verbose,
                             "[MMBAR]\t\tMemory has not been initialized yet!\n");
        return false;
    }

    if (addr + 4 >= MEM_SIZE) {
        PRINTF_DEBUG_VERBOSE(verbose,
                             "[MMBAR]\t\tWrite u32 address index out of range: %d\n",
                             addr);
        return false;
    }

#define LOLO_MASK 0xFFUL
    uint8_t blhh = e >> 24;
    uint8_t blhl = (e >> 16) & LOLO_MASK;
    uint8_t bllh = (e >> 8) & LOLO_MASK;
    uint8_t blll = e & LOLO_MASK;

    mmBar->_memory[addr] = blll;
    mmBar->_memory[addr + 1] = bllh;
    mmBar->_memory[addr + 2] = blhl;
    mmBar->_memory[addr + 3] = blhh;

    return 1;
}

uint8_t mmbar_read(MMBar *mmBar, uint32_t addr) {

    if (!mmBar->initialized) {
        PRINTF_DEBUG_VERBOSE(verbose,
                             "[MMBAR]\t\tMemory has not been initialized yet!\n");
        return 0x0;
    }

    if (addr >= MEM_SIZE) {
        PRINTF_DEBUG_VERBOSE(verbose,
                             "[MMBAR]\t\tRead address index out of range: %d\n",
                             addr);
        return 0x0;
    }

    return mmBar->_memory[addr];
}

uint16_t mmbar_readu16(MMBar *mmBar, uint32_t addr) {
    if (!mmBar->initialized) {
        PRINTF_DEBUG_VERBOSE(verbose,
                             "[MMBAR]\t\tMemory has not been initialized yet!\n");
        return 0x0;
    }

    if (addr + 2 >= MEM_SIZE) {
        PRINTF_DEBUG_VERBOSE(verbose,
                             "[MMBAR]\t\tWrite u16 address index out of range: %d\n",
                             addr);
        return 0x0;
    }

    return (mmBar->_memory[addr + 1] << 8) |
           (mmBar->_memory[addr]);
}

uint32_t mmbar_readu32(MMBar *mmBar, uint32_t addr) {
    if (!mmBar->initialized) {
        PRINTF_DEBUG_VERBOSE(verbose,
                             "[MMBAR]\t\tMemory has not been initialized yet!\n");
        return 0x0;
    }

    if (addr + 4 >= MEM_SIZE) {
        PRINTF_DEBUG_VERBOSE(verbose,
                             "[MMBAR]\t\tWrite u32 address index out of range: %d\n",
                             addr);
        return 0x0;
    }

    return (mmBar->_memory[addr + 3] << 24) |
           (mmBar->_memory[addr + 2] << 16) |
           (mmBar->_memory[addr + 1] << 8) |
           (mmBar->_memory[addr]);
}

void mmbar_load_text(MMBar *mmBar, std::vector<std::uint32_t> bin) {
    for (uint32_t bl: bin) {
        mmbar_writeu32(mmBar, mmBar->text_end_addr, bl);
        mmBar->text_end_addr += 4;
    }
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
