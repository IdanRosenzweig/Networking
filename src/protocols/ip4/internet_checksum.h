#ifndef SERVERCLIENT_INTERNET_CHECKSUM_H
#define SERVERCLIENT_INTERNET_CHECKSUM_H

#include <stdint.h>

unsigned short internet_checksum(const uint16_t * buf, int no_bytes);

#endif //SERVERCLIENT_INTERNET_CHECKSUM_H