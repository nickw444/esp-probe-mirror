#ifndef _PROBE_H
#define _PROBE_H

#include <Arduino.h>

#define NUM_NETWORKS 50
#define MAX_SSID_LENGTH 20

void ICACHE_FLASH_ATTR sniffer_callback(uint8_t *buffer, uint16_t length);
extern char seen_networks[NUM_NETWORKS][MAX_SSID_LENGTH];

#endif
