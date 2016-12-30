#ifndef __CRC32__
#define __CRC32__ 1

#include <sys/types.h>

extern uint32_t crc32_init(void);
extern uint32_t crc32_update(uint32_t current, void *buffer, size_t size);
extern uint32_t crc32_finish(uint32_t current);

extern uint32_t crc32_calc(void *buffer, size_t size);

#endif /* __CRC32__ */
