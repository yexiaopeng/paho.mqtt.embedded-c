    #ifndef _W5500_MQTT_W5500MQTT_H
    #define _W5500_MQTT_W5500MQTT_H

    #include "stdint.h"


    extern int mqtt_subscrib(uint8_t * serverip,char *pTopic,char *pMessage
    ,unsigned short keepAliveInterval);
    extern int mqtt_publish(uint8_t * serverip,char *pTopic,char *pMessage);


    #endif

