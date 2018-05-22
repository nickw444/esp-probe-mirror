#ifndef _SSID_BROADCAST_H
#define _SSID_BROADCAST_H
#include <Arduino.h>

void send_beacon_frame(const char * ssid, uint8_t channel);

#endif
