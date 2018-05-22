#include <Arduino.h>
#include <Bounce2.h>

#include "ssid_broadcast.h"
#include "probe.h"

extern "C" {
  #include "user_interface.h"
}

#define CHANNEL_HOP_INTERVAL_MS 1000
#define BUTTON_PIN 12

static os_timer_t channelHop_timer;
static os_timer_t info_timer;

Bounce button = Bounce();

void channelHop() {
  uint8_t new_channel = wifi_get_channel() + 1;
  if (new_channel > 14) {
    new_channel = 1;
  }
  wifi_set_channel(new_channel);
}

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


void probe_repeater_setup() {
  wifi_set_promiscuous_rx_cb(sniffer_callback);
  wifi_promiscuous_enable(1);

  // setup the channel hoping callback timer
  os_timer_setfn(&channelHop_timer, (os_timer_func_t *) channelHop, NULL);
  os_timer_arm(&channelHop_timer, CHANNEL_HOP_INTERVAL_MS, 1);
  os_timer_setfn(&info_timer, (os_timer_func_t *) printInfo, NULL);
  os_timer_arm(&info_timer, 5000, 1);
}

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
}

void probe_repeater_teardown() {
  os_timer_disarm(&channelHop_timer);
  os_timer_disarm(&info_timer);

  wifi_promiscuous_enable(0);
  wifi_set_promiscuous_rx_cb(NULL);
}

void beacon_spam_setup() {

}

void beacon_spam_loop() {
  for (int i = 0; i < 100; i++) {
    // Randomize channel
    uint8_t channel = wifi_get_channel();
    char ssid[10];
    sprintf(ssid, "DEFAULT%02d", i);
    ssid[9] = 0;
    send_beacon_frame(ssid, channel);
    delay(1);
  }
}

void beacon_spam_teardown() {

}

int active_mode = 0;

void active_mode_setup() {
  switch (active_mode) {
    case 0:
      return probe_repeater_setup();
    case 1:
      return beacon_spam_setup();
  }
}

void active_mode_loop() {
  switch (active_mode) {
    case 0:
      return probe_repeater_loop();
    case 1:
      return beacon_spam_loop();
  }
}

void active_mode_teardown() {
  switch (active_mode) {
    case 0:
      return probe_repeater_teardown();
    case 1:
      return beacon_spam_teardown();
  }
}

void setup() {
  Serial.begin(115200);
  wifi_set_opmode(STATION_MODE);

  pinMode(BUTTON_PIN, INPUT_PULLUP);
  button.attach(BUTTON_PIN);
  button.interval(100);

  active_mode_setup();
}

void loop() {
  button.update();

  if (button.fell()) {
    active_mode_teardown();
    active_mode = (active_mode + 1) % 2;
    Serial.printf("Active mode %d\n", active_mode);
    active_mode_setup();
  }

  active_mode_loop();
}
