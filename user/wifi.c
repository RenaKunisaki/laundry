#include "main.h"

void ICACHE_FLASH_ATTR state_send_beacon() {
    //send a wifi beacon
    // beacon frame definition
    static uint8_t beaconPacket[] = {
        // 0-3 Type/Subtype: managment beacon frame
        0x80, 0x00, 0x00, 0x00,
        // 4-9 Destination: broadcast
        0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
        // 10-15 Source
        0x75, 0x11, 'R', 'e', 'n', 'a',
        // 16-21 Source
        0x75, 0x11, 'R', 'e', 'n', 'a',

        // Fixed parameters
        // 22-23 Fragment & sequence number (will be done by the SDK)
        0x00, 0x00,
        // 24-31 Timestamp
        0x83, 0x51, 0xf7, 0x8f, 0x0f, 0x00, 0x00, 0x00,
         // 32-33 Interval: 0x64, 0x00 => every 100ms - 0xe8, 0x03 => every 1s
        0xe8, 0x03,
        // 34-35 capabilities Tnformation
        0x31, 0x00,

        // Tagged parameters

        // Current Channel
        0x03, 0x01, // 36-37 Channel set, length
        0x0B, // 38: Current Channel

        // SSID parameters
        // 39-40 Tag: Set SSID length, Tag length: 32
        0x00, 0x20,
        // 41-73: SSID
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,

        // Supported Rates
        // 70-71 Tag: Supported Rates, Tag length: 8
        /* 0x01, 0x08,
        0x82, //72: 1(B)
        0x84, //73: 2(B)
        0x8b, //74: 5.5(B)
        0x96, //75: 11(B)
        0x24, //76: 18
        0x30, //77: 24
        0x48, //78: 36
        0x6c, //79: 54 */

        // RSN information
        /* 0x30, 0x18, //  83 -  84
        0x01, 0x00, //  85 -  86
        0x00, 0x0f, 0xac, 0x02, //  87 -  90
        0x02, 0x00, //  91 -  92
        // Fix: changed 0x02(TKIP) to 0x04(CCMP) is default.
        // WPA2 with TKIP not supported by many devices
        0x00, 0x0f, 0xac, 0x04, 0x00, 0x0f, 0xac, 0x04, //  93 - 100
        0x01, 0x00, // 101 - 102
        0x00, 0x0f, 0xac, 0x02, // 103 - 106
        0x00, 0x00 // 107 - 108
        */
    };

    //set params from config
    memcpy(&beaconPacket[10], g_config.wifiMac1, 6);
    memcpy(&beaconPacket[16], g_config.wifiMac2, 6);
    beaconPacket[38] = g_config.wifiChannel;
    beaconPacket[40] = strlen(g_config.wifiSsid);
    memcpy(&beaconPacket[41], g_config.wifiSsid, 32);
    wifi_set_opmode_current(STATION_MODE);
    wifi_set_channel(g_config.wifiChannel);

    //encode info in timestamp
    if(g_config.ledBoardFlip) {
        beaconPacket[28] = g_ledVal[NUM_LEDS - LED_ROOM] / 4;
    }
    else beaconPacket[28] = g_ledVal[LED_ROOM] / 4;
    beaconPacket[29] = (int)(g_temperature * 2.0);
    beaconPacket[30] = g_washState;
    beaconPacket[31] = g_dryState;
    gpio_write(4, 1); //LED off
    int r = wifi_send_pkt_freedom(beaconPacket, sizeof(beaconPacket), 0);
    gpio_write(4, r); //LED on if success (active low)
    if(r) os_printf("Send beacon failed\r\n");
    //else os_printf("Send beacon OK\r\n");
    g_state = STATE_IDLE;
}
