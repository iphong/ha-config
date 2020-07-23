//
// Author: Phong Vu
//
#ifndef ESP_NOW_CONNECT_H
#define ESP_NOW_CONNECT_H

#include <esphome.h>
#include <ESP8266WiFi.h>
#include <espnow.h>

uint8_t broadcast[6] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};

typedef void (*cmd_handler_cb_t)();

struct cmd_cb_t {
    const char *command;
    cmd_handler_cb_t handler;
};

cmd_cb_t handlers[20] = {};
uint8_t handler_count = 0;
String msg;

struct {
    void begin(uint8_t channel = 1) {
        WiFi.mode(WIFI_AP_STA);
        if (esp_now_init() == OK) {
            esp_now_set_self_role(ESP_NOW_ROLE_COMBO);
            if (esp_now_is_peer_exist(broadcast))
                esp_now_del_peer(broadcast);
            esp_now_add_peer(broadcast, ESP_NOW_ROLE_COMBO, channel, 0, 0);
            esp_now_register_recv_cb([](uint8_t *mac, uint8_t *payload, uint8_t size) {
                msg = "";
                for (size_t i=0; i<size; i++) {
                    msg.concat((const char)payload[i]);
                }
                for (size_t i=0; i<handler_count; i++) {
                    cmd_cb_t cb = handlers[i];
                    if (msg.equals(cb.command)) {
                        cb.handler();
                    }
                }
                ESP_LOGD("espnow", "%02X:%02X:%02X:%02X:%02X:%02X >>> %s", mac[0], mac[1], mac[2], mac[3], mac[4], mac[5], msg.c_str()); 
            });
        }
    }
    void send(String msg) {
        esp_now_send(broadcast, (uint8_t *)msg.c_str(), (uint8_t)msg.length());
    }
    void on(const char * command, cmd_handler_cb_t handler) {
        handlers[handler_count++] = {command, handler};
    }
} ESPNow;

#endif //ESP_NOW_CONNECT_H
