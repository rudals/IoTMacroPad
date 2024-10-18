/**
 * Copyright (c) 2024 rudals
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "hardware/pwm.h"
#include "hardware/pio.h"

#include "port_common.h"
#include "wizchip_conf.h"
#include "w5x00_spi.h"

#include "dhcp.h"
#include "dns.h"

#include "mqtt_interface.h"
#include "MQTTClient.h"
#include "timer.h"

#include "buzzer.h"
#include "keypad.h"
#include "encoder.h"
#include "ui.h"

#define PLL_SYS_KHZ             (133 * 1000)
#define ETHERNET_BUF_MAX_SIZE   (1024 * 2)
#define SOCKET_MQTT             0
#define SOCKET_DHCP             1
#define SOCKET_DNS              2
#define PORT_MQTT               1883
#define DEFAULT_TIMEOUT         1000
#define DNS_RETRY_COUNT         5
#define MQTT_CLIENT_ID          "pico"
#define MQTT_USERNAME           "user"
#define MQTT_PASSWORD           "user"
#define MQTT_SUB_TOPIC          "imp_sub"
#define MQTT_PUB_TOPIC          "imp_pub"
#define MQTT_PUBLISH_PERIOD     (1000 * 10)
#define MQTT_KEEP_ALIVE         60

static wiz_NetInfo g_net_info = {
    .mac = {0x00, 0x08, 0xDC, 0x12, 0x34, 0x56},
    .ip = {192, 168, 0, 30},
    .sn = {255, 255, 255, 0},
    .gw = {192, 168, 0, 1},
    .dns = {8, 8, 8, 8},
    .dhcp = NETINFO_STATIC
};

static uint8_t g_dns_target_domain[] = "192.168.0.50";
static uint8_t g_ethernet_buf[ETHERNET_BUF_MAX_SIZE] = { 0, };
static uint8_t g_mqtt_send_buf[ETHERNET_BUF_MAX_SIZE] = { 0, };
static uint8_t g_mqtt_recv_buf[ETHERNET_BUF_MAX_SIZE] = { 0, };
static uint8_t g_mqtt_broker_ip[4] = {0, 0, 0, 0};
static Network g_mqtt_network;
static MQTTClient g_mqtt_client;
static MQTTPacket_connectData g_mqtt_packet_connect_data = MQTTPacket_connectData_initializer;
static MQTTMessage g_mqtt_message;
static uint8_t g_mqtt_payload_buf[30];
static volatile uint32_t g_msec_cnt = 0;
static char last_pressed_key = 0;
static int32_t last_encoder_value = 0;

static void set_clock_khz(void)
{
    set_sys_clock_khz(PLL_SYS_KHZ, true);
    clock_configure(
        clk_peri,
        0,
        CLOCKS_CLK_PERI_CTRL_AUXSRC_VALUE_CLKSRC_PLL_SYS,
        PLL_SYS_KHZ * 1000,
        PLL_SYS_KHZ * 1000
    );
}

char** str_split(char* a_str, const char a_delim)
{
    char** result    = 0;
    size_t count     = 0;
    char* tmp        = a_str;
    char* last_comma = 0;
    char delim[2];
    delim[0] = a_delim;
    delim[1] = 0;

    while(*tmp) {
        if(a_delim == *tmp) {
            count++;
            last_comma = tmp;
        }

        tmp++;
    }

    count += last_comma < (a_str + strlen(a_str) - 1);
    count++;
    result = malloc(sizeof(char*) * count);

    if(result) {
        size_t idx  = 0;
        char* token = strtok(a_str, delim);

        while(token) {
            assert(idx < count);
            *(result + idx++) = strdup(token);
            token = strtok(0, delim);
        }

        assert(idx == count - 1);
        *(result + idx) = 0;
    }

    return result;
}

static void message_arrived(MessageData *msg_data)
{
    int i;
    char** tokens;

    MQTTMessage *message = msg_data->message;
    tokens = str_split((char*)message->payload, '|');
    if(tokens) {
        set_cmd_data(atoi(*(tokens + 0)), atoi(*(tokens + 1)), *(tokens + 2), *(tokens + 3));

        for(i = 0; * (tokens + i); i++) {
            free(*(tokens + i));
        }
        free(tokens);
    }
}

static void repeating_timer_callback(void)
{
    g_msec_cnt++;
    MilliTimer_Handler();
}

static time_t millis(void)
{
    return g_msec_cnt;
}

void process_pub()
{
    g_mqtt_message.payload = (void*)g_mqtt_payload_buf;
    g_mqtt_message.payloadlen = strlen((const char*)g_mqtt_message.payload);

    int retval = MQTTPublish(&g_mqtt_client, MQTT_PUB_TOPIC, &g_mqtt_message);
    if(retval < 0) {
        printf(" Publish failed : %d\n", retval);
    }
}

void process_keycmd(char cmd)
{
    int index = (int)(cmd - '1');

    disp_cmd(index);
    get_cmd_data(g_mqtt_payload_buf, index);
    process_pub();
}

int main()
{
    int32_t retval = 0;
    uint8_t dns_retry = 0;

    uint32_t start_ms = 0, end_ms = 0;
    uint32_t key_s_ms = 0, key_e_ms = 0;
    uint32_t enc_s_ms = 0, enc_e_ms = 0;

    set_clock_khz();
    stdio_init_all();

    disp_init();
    disp_log("IoTMacroPad Demo");

    printf("wizchip spi init");
    wizchip_spi_initialize();
    wizchip_cris_initialize();
    wizchip_reset();

    printf("wizchip init\n");
    wizchip_initialize();

    printf("wizchip check\n");
    wizchip_check();

    wizchip_1ms_timer_initialize(repeating_timer_callback);

    printf("network init\n");
    network_initialize(g_net_info);

    disp_log(" > Keypad init");
    pico_keypad_init();

    disp_log(" > Encoder init");
    encoder_init();

    disp_log(" > Sound init");
    beep_sound_play(1);

    disp_log(" > DNS init");
    DNS_init(SOCKET_DNS, g_ethernet_buf);

    while(1) {
        if(DNS_run(g_net_info.dns, g_dns_target_domain, g_mqtt_broker_ip) > 0) {
            printf(" DNS success\r\n");
            printf(" Target: %s(%d.%d.%d.%d)\r\n",
                   g_dns_target_domain,
                   g_mqtt_broker_ip[0],
                   g_mqtt_broker_ip[1],
                   g_mqtt_broker_ip[2],
                   g_mqtt_broker_ip[3]);
            break;
        } else {
            dns_retry++;
            if(dns_retry <= DNS_RETRY_COUNT) {
                printf(" DNS timeout occurred and retry %d\r\n", dns_retry);
            }
        }

        if(dns_retry > DNS_RETRY_COUNT) {
            printf(" DNS failed\r\n");
            while(1) ;
        }
    }

    print_network_information(g_net_info);
    NewNetwork(&g_mqtt_network, SOCKET_MQTT);
    retval = ConnectNetwork(&g_mqtt_network, g_mqtt_broker_ip, PORT_MQTT);

    if(retval != 1) {
        printf(" Network connect failed\n");
        while(1) ;
    }

    MQTTClientInit(&g_mqtt_client, &g_mqtt_network, DEFAULT_TIMEOUT, g_mqtt_send_buf, ETHERNET_BUF_MAX_SIZE, g_mqtt_recv_buf, ETHERNET_BUF_MAX_SIZE);

    g_mqtt_packet_connect_data.MQTTVersion = 3;
    g_mqtt_packet_connect_data.cleansession = 1;
    g_mqtt_packet_connect_data.willFlag = 0;
    g_mqtt_packet_connect_data.keepAliveInterval = MQTT_KEEP_ALIVE;
    g_mqtt_packet_connect_data.clientID.cstring = MQTT_CLIENT_ID;
    g_mqtt_packet_connect_data.username.cstring = MQTT_USERNAME;
    g_mqtt_packet_connect_data.password.cstring = MQTT_PASSWORD;
    retval = MQTTConnect(&g_mqtt_client, &g_mqtt_packet_connect_data);

    if(retval < 0) {
        printf(" MQTT connect failed : %d\n", retval);
        while(1) ;
    }

    disp_log(" > MQTT connected");
    g_mqtt_message.qos = QOS0;
    g_mqtt_message.retained = 0;
    g_mqtt_message.dup = 0;
    g_mqtt_message.payload = (void*)g_mqtt_payload_buf;
    g_mqtt_message.payloadlen = strlen((const char*)g_mqtt_message.payload);
    retval = MQTTSubscribe(&g_mqtt_client, MQTT_SUB_TOPIC, QOS0, message_arrived);

    if(retval < 0) {
        printf(" Subscribe failed : %d\n", retval);
        while(1) ;
    }

    disp_log(" > Subscribed");
    busy_wait_us(2000000);

    disp_menu();
    disp_key(last_encoder_value);

    enc_s_ms = key_s_ms = start_ms = millis();

    while(1) {
        key_e_ms = millis();

        char pressed_key = pico_keypad_get_key();
        if(pressed_key != 0 && last_pressed_key != pressed_key) {
            if(key_e_ms - key_s_ms > KEY_DEBOUNCE_TIME) {
                last_pressed_key = pressed_key;
                process_keycmd(pressed_key);
                key_s_ms = millis();
            }
        } else {
            if(key_e_ms - key_s_ms > 100) {
                last_pressed_key = 0;
                key_s_ms = millis();
            }
        }

        enc_e_ms = millis();
        int32_t encoder_value = get_encoder_value();
        if(enc_e_ms > enc_s_ms + ENC_DEBOUNCE_TIME) {
            if(last_encoder_value != encoder_value) {
                last_encoder_value = encoder_value;
                disp_key((last_encoder_value<0)?(-last_encoder_value):last_encoder_value);
            }
            enc_s_ms = millis();
        }

        if((retval = MQTTYield(&g_mqtt_client, g_mqtt_packet_connect_data.keepAliveInterval)) < 0) {
            printf(" Yield error : %d\n", retval);
            while(1) ;
        }
    }
}


