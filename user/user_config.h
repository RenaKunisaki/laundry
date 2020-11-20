#ifndef _USER_CONFIG_H_
#define _USER_CONFIG_H_
#include "os_type.h"

#define CHANNEL_HOP_INTERVAL 20
#define SSID_LEN 32
#define MAX_CAPTURED_PROBES 256
#define PROBE_TX_INTERVAL 20
#define PROBE_TX_CHANCE ((5.0/100.0)*0x7FFFFFFF)

typedef struct {
    signed rssi:8;
    unsigned rate:4;
    unsigned is_group:1;
    unsigned:1;
    unsigned sig_mode:2;
    unsigned legacy_length:12;
    unsigned damatch0:1;
    unsigned damatch1:1;
    unsigned bssidmatch0:1;
    unsigned bssidmatch1:1;
    unsigned MCS:7;
    unsigned CWB:1;
    unsigned HT_length:16;
    unsigned Smoothing:1;
    unsigned Not_Sounding:1;
    unsigned:1;
    unsigned Aggregation:1;
    unsigned STBC:2;
    unsigned FEC_CODING:1;
    unsigned SGI:1;
    unsigned rxend_state:8;
    unsigned ampdu_cnt:8;
    unsigned channel:4;
    unsigned:12;
} RxControl;

typedef struct {
    RxControl header;
    //actual 802.11 frame
	uint16_t framectl;
	uint16_t duration;
	uint8_t rxaddr[6];
	uint8_t srcaddr[6];
	uint8_t dstaddr[6];
	uint16_t seqctl;
} wifiPacketHeader;

typedef struct {
    uint8_t id; //0 = SSID
    uint8_t length;
    uint8_t data[];
} wifiBeaconFrameElement;

typedef struct {
    uint64_t timestamp; //microseconds, AP uptime
    uint16_t interval;
    uint16_t capability;
    wifiBeaconFrameElement element[];
} wifiBeaconFrame;

typedef struct {
    char name[3];
    uint8_t firstChan;
    uint8_t numChans;
    uint8_t maxPower; //dBm
} wifiCountryElem;

enum {
    WIFI_MGMT_ASSOC_REQ    = 0,
    WIFI_MGMT_ASSOC_RESP   = 1,
    WIFI_MGMT_REASSOC_REQ  = 2,
    WIFI_MGMT_REASSOC_RESP = 3,
    WIFI_MGMT_PROBE_REQ    = 4,
    WIFI_MGMT_PROBE_RESP   = 5,
    WIFI_MGMT_BEACON       = 8,
    WIFI_MGMT_ATIM         = 9,
    WIFI_MGMT_DISASSOC     = 10,
    WIFI_MGMT_AUTH         = 11,
    WIFI_MGMT_DEAUTH       = 12,
    WIFI_MGMT_SUBTYPE_MAX  = 12,
    NUM_WIFI_MGMT_SUBTYPES = 13,
} wifiMgmtSubtype; //type 0

//util.c
void hexdump(const void *data, uint32_t len);

#endif //_USER_CONFIG_H_
