#ifndef SUSPICIOUS_H
#define SUSPICIOUS_H

#include <linux/types.h>

void suspicious(uint32_t src_ip, uint32_t dest_ip);
void init_suspicious(void);
void exit_suspicious(void);

#endif // SUSPICIOUS_H
