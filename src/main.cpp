#include <Arduino.h>
#include <Bounce2.h>

#include "ssid_broadcast.h"
#include "probe.h"
#include "config.h"

extern "C" {
  #include "user_interface.h"
}

#if ENABLE_CHANNEL_HOP
#define CHANNEL_HOP_INTERVAL_MS 10000
static os_timer_t channelHop_timer;
#endif

#if ENABLE_PERIODIC_PRINT
static os_timer_t info_timer;
#endif

#if ENABLE_ADVERTISEMENT
const char * advertisement[] = {
  "_0|  Some advertisement",
  "_1|  can be placed",
  "_2|  here",
};
#endif

#if ENABLE_CHANNEL_HOP
void channelHop() {
uint8_t new_channel = wifi_get_channel() + 1;
  if (new_channel >= 14) {
    new_channel = 1;
  }
  wifi_set_channel(new_channel);
}
#endif


#if ENABLE_PERIODIC_PRINT
void printInfo() {
  Serial.println("++ Beacons: ++");
  for (int i = 0; i < NUM_NETWORKS; i++) {
    char * ssid = seen_networks[i];
    if (ssid[0] == 0) {
      continue;
    }
    Serial.printf("%d: %s\n", i, ssid);
  }
}
#endif

void probe_repeater_setup() {
  wifi_set_promiscuous_rx_cb(sniffer_callback);
  wifi_promiscuous_enable(1);

  // setup the channel hoping callback timer
  #if ENABLE_CHANNEL_HOP
  os_timer_setfn(&channelHop_timer, (os_timer_func_t *) channelHop, NULL);
  os_timer_arm(&channelHop_timer, CHANNEL_HOP_INTERVAL_MS, 1);
  #endif

  #if ENABLE_PERIODIC_PRINT
  os_timer_setfn(&info_timer, (os_timer_func_t *) printInfo, NULL);
  os_timer_arm(&info_timer, 5000, 1);
  #endif
}

#if ENABLE_REBROADCAST
void probe_repeater_loop() {
  int channel = wifi_get_channel();
  for (int i = 0; i < NUM_NETWORKS; i++) {
    char * ssid = seen_networks[i];
    if (ssid[0] == 0) {
      continue;
    }
    send_beacon_frame(ssid, channel);
    delay(1);
  }

  #if ENABLE_ADVERTISEMENT
  for (int i = 0; i < 4; i++) {
    send_beacon_frame(advertisement[i], channel);
    delay(1);
  }
  #endif
}
#endif

void setup() {
  Serial.begin(115200);
  wifi_set_opmode(STATION_MODE);

  wifi_set_channel(13);
  probe_repeater_setup();
}

void loop() {
  #if ENABLE_REBROADCAST
  probe_repeater_loop();
  #endif
}
