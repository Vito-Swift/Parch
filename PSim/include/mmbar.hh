/**
 * @filename: mmbar.hh
 * @author: Vito Wu <chenhaowu[at]link.cuhk.edu.cn>
 * @version: 
 * @desc:
 * @date: 2/24/2021
 */

#ifndef PARCH_MMBAR_HH
#define PARCH_MMBAR_HH

#include <stdint.h>

#include "utils.hh"
#include "register.hh"
#include "options.hh"

#define MEM_SIZE 0x80000000UL
#define MEM_TEXT_START 0x400000UL
#define MEM_TEXT_END 0x10000000UL
#define MEM_STACK_START 0x7FFFFFFFUL
#define MEM_TOP MEM_STACK_START
#define MEM_DATA_START MEM_TEXT_END

struct MMBar {
    uint8_t *_memory;
    uint32_t text_end_addr;
    uint32_t static_end_addr;
    uint32_t dynamic_end_addr;
    bool initialized = false;
};

void mmbar_init(MMBar *mmBar);

void mmbar_load_text(MMBar *mmBar, std::vector<std::uint32_t> bin);

void mmbar_free(MMBar *mmBar);

bool mmbar_write(MMBar* mmBar, uint32_t addr);

uint8_t mmbar_read(MMBar* mmBar, uint32_t addr);

uint32_t mmbar_readu32(MMBar *mmBar, uint32_t addr);

bool mmbar_writeu32(MMBar *mmBar, uint32_t addr, uint32_t e);

#endif //PARCH_MMBAR_HH
