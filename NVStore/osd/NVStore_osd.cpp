#include "mbed.h"
#include <FlashIAPBlockDevice.h>
#include <TDBStore.h>

#include <flash_data.h>

#include "NVStore.h"

#define FIRST_DB_BLOCK (ADDR_FLASH_SECTOR_6)

#define VIRTUAL_SECTOR_SIZE  (1024)
#define VIRTUAL_ERASE_SUB_SIZE (64) // Must divide VIRTUAL_SECTOR_SIZE (and be compatible with program)

class _MyBlockDevice: public SlicingBlockDevice {
public:
  _MyBlockDevice(BlockDevice *bd, bd_addr_t start, bd_addr_t end=0): SlicingBlockDevice(bd, start, end) { }
  virtual int erase(bd_addr_t addr, bd_size_t size);
  virtual bd_size_t get_erase_size () const { return VIRTUAL_SECTOR_SIZE; }
  virtual bd_size_t get_erase_size (bd_addr_t addr) const;
  //virtual int get_erase_value () const;
  virtual bool is_valid_erase (bd_addr_t addr, bd_size_t size) const {
    return size % VIRTUAL_SECTOR_SIZE == 0 && is_valid_program(addr, size);
  }
};

static uint8_t _eraseBlock[VIRTUAL_ERASE_SUB_SIZE] = { 1, 2 }; // Differe

int _MyBlockDevice::erase(bd_addr_t addr, bd_size_t size) {
  if (!is_valid_erase(addr, size)) return -1;
  if (_eraseBlock[0] != _eraseBlock[1]) { // erase block non initialized
    uint8_t erase_value = get_erase_value();
    for(uint8_t *p = _eraseBlock, n = VIRTUAL_ERASE_SUB_SIZE; n; --n) *p++ = erase_value;
  }
  for(; size > 0; addr += VIRTUAL_ERASE_SUB_SIZE, size -= VIRTUAL_ERASE_SUB_SIZE) {
    program(_eraseBlock, addr, VIRTUAL_ERASE_SUB_SIZE);
  }
  return 0;
}

bd_size_t _MyBlockDevice::get_erase_size(bd_addr_t addr) const {
  return is_valid_program(addr, VIRTUAL_SECTOR_SIZE) ? VIRTUAL_SECTOR_SIZE : 0;
}

//static FlashIAPBlockDevice full_flash(FIRST_DB_BLOCK, 256 * 1024);
static FlashIAPBlockDevice full_flash(ADDR_FLASH_SECTOR_7, 128 * 1024);
static _MyBlockDevice flash(&full_flash, 0, 128 * 1024);
TDBStore nvStore(&flash);

extern uint32_t __etext;

void NVStore_init() {
  if (uint32_t(&__etext) >= FIRST_DB_BLOCK) error(".text smash FLASH db !");
  
  printf("\tetext %lx (%d bytes left)\n", 
	 (uint32_t)&__etext, int(FIRST_DB_BLOCK - (uint32_t)&__etext));
  flash.init();
  int res = nvStore.init();
  printf("nvStore up error=%d\n", res);
  KVStore::iterator_t i;
  char key[16];
  uint32_t U;
  for(nvStore.iterator_open(&i); nvStore.iterator_next(i, key, 16) == MBED_SUCCESS;) {
    printf("\t%s\t", key);
    switch(key[0]) {
    case 'i':
      printf("%d", NV<int16_t>::get(key));
      break;

    case 'u':
      printf("%d", NV<uint16_t>::get(key));
      break;

    case 'U':
      U = NV<uint32_t>::get(key);
      printf("%ld %lxd", U, U);
      break;

    case 'f':
      printf("%f", NV<float>::get(key));
      break;

    default:
      break;
    }
    putchar('\n');
  }
  nvStore.iterator_close(i);
}
