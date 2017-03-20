#ifndef CHECKSUM_FLETCHER16__H
#define CHECKSUM_FLETCHER16__H

#include <stdint.h>

uint16_t checksum_fletcher16( uint8_t const *data, uint16_t bytes );

#endif // CHECKSUM_FLETCHER16__H