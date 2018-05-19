#include <ESP8266WiFi.h>

extern "C" {
  #include "user_interface.h"
}


// Beacon Packet buffer
uint8_t header[] = {
  0x80, 0x00,
  0x00, 0x00,
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, // Destination address
  0x01, 0x02, 0x03, 0x04, 0x05, 0x06, // Src address
  0x01, 0x02, 0x03, 0x04, 0x05, 0x06, // BSSID
  0xc0, 0x6c, //Seq-ctl
  0x83, 0x51, 0xf7, 0x8f, 0x0f, 0x00, 0x00, 0x00, // Timestamp
  0xff, 0x00, // Beacon interval
  0x01, 0x04, // Capability info
  0x00, 0x00, // SSID Length
};

uint8_t connection_info[] = {
  0x01, 0x08, 0x82, 0x84,
  0x8b, 0x96, 0x24, 0x30, 0x48, 0x6c, 0x03, 0x01,
};


void send_packet(const char * ssid, uint8_t channel) {
    uint8_t pkt_buffer[128];
    uint16_t ssid_len = strlen(ssid);

    // Copy Segments
    memset(pkt_buffer, 0x00, sizeof pkt_buffer);
    memcpy(pkt_buffer, header, sizeof header);
    memcpy(pkt_buffer + sizeof header, ssid, ssid_len);
    memcpy(pkt_buffer + sizeof header + ssid_len, connection_info, sizeof connection_info);

    // Set BSSID to random
    pkt_buffer[10] = pkt_buffer[16] = random(256);
    pkt_buffer[11] = pkt_buffer[17] = random(256);
    pkt_buffer[12] = pkt_buffer[18] = random(256);
    pkt_buffer[13] = pkt_buffer[19] = random(256);
    pkt_buffer[14] = pkt_buffer[20] = random(256);
    pkt_buffer[15] = pkt_buffer[21] = random(256);

    // SSID length
    pkt_buffer[37] = ssid_len;

    // Set Channel
    pkt_buffer[sizeof header + ssid_len + sizeof connection_info] = channel;

    uint16_t packet_size = sizeof header + ssid_len + sizeof connection_info + 1;
    wifi_set_channel(channel);
    wifi_send_pkt_freedom(pkt_buffer, packet_size, 0);
    wifi_send_pkt_freedom(pkt_buffer, packet_size, 0);
    wifi_send_pkt_freedom(pkt_buffer, packet_size, 0);
}

void setup() {
  Serial.begin(115200);
  wifi_set_opmode(STATION_MODE);
  wifi_promiscuous_enable(1);
}

void loop() {
    // Randomize channel //


    for (int i = 0; i < 100; i++) {
        uint8_t channel = random(1,12);
        char ssid[10];
        sprintf(ssid, "DEFAULT%02d", i);
        ssid[9] = 0;
        send_packet(ssid, channel);
        delay(1);
    }
}
