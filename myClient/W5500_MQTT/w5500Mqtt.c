    #include "w5500Mqtt.h"


    #include "bsp_usart_1_debug.h"
    #include "NT_SysTick.h"
    #include "bsp_spi.h"
    #include <stdlib.h>
    #include <stdint.h>
    #include <string.h>
    #include "MQTTPacket.h"
    #include "socket.h"
    #include "Include.h"

    #include "transport.h"

    extern int mqtt_publish(uint8_t * serverip,char *pTopic,char *pMessage)
    {
        int32_t len,rc;
        MQTTPacket_connectData data = MQTTPacket_connectData_initializer;
        unsigned char buf[200];
        MQTTString topicString = MQTTString_initializer;
        int msglen = strlen(pMessage);
        int buflen = sizeof(buf);

        data.clientID.cstring = "me";
        data.keepAliveInterval = 5;
        data.cleansession = 1;
        len = MQTTSerialize_connect(buf, buflen, &data); /* 1 */

        topicString.cstring = pTopic;
        len += MQTTSerialize_publish(buf + len, buflen - len, 0, 0, 0, 0, topicString, (unsigned char*)pMessage, msglen); /* 2 */

        len += MQTTSerialize_disconnect(buf + len, buflen - len); /* 3 */
        transport_open(serverip);	
        rc = transport_sendPacketBuffer(buf,len);
        transport_close();
        if (rc == len){
        //printf("Successfully published\n\r");
            }
        else
        printf("Publish failed\n\r");
        return 0;
    }
    /**
    * @brief  �����������һ����Ϣ���ú�������ΪTCP�������ݺ���������
    * @param  pTopic ��Ϣ���⣬����
    * @param  pMessage ��Ϣ���ݣ�����
    * @retval С��0��ʾ������Ϣʧ��
    */
    extern int mqtt_subscrib(uint8_t * serverip,char *pTopic,char *pMessage,
                unsigned short keepAliveInterval)
    {
        MQTTPacket_connectData data = MQTTPacket_connectData_initializer;
        int rc = 0;
        unsigned char buf[200];
        int buflen = sizeof(buf);
        int msgid = 1;
        MQTTString topicString = MQTTString_initializer;
        int req_qos = 0;

        int len = 0;
        rc = transport_open(serverip);
        if(rc < 0){
        printf("transport_open error\n\r");
        return rc;
        }

        data.clientID.cstring = "";
        data.keepAliveInterval = keepAliveInterval;//��������������ʱ�䣬������ʱ��󣬷������������Ͽ����ӣ���λΪ��
        data.cleansession = 1;
        data.username.cstring = "";
        data.password.cstring = "";

        len = MQTTSerialize_connect(buf, buflen, &data);
        rc = transport_sendPacketBuffer(buf, len);
        if(rc != len){
            printf("connect transport_sendPacketBuffer error\n\r");
            goto exit;
        }
        /* wait for connack */
        if (MQTTPacket_read(buf, buflen, transport_getdata) == CONNACK)
        {
        unsigned char sessionPresent, connack_rc;

        if (MQTTDeserialize_connack(&sessionPresent, &connack_rc, buf, buflen) != 1 || connack_rc != 0)
        {
        	printf("Unable to connect, return code %d\n\r", connack_rc);
        	goto exit;
        }
        }else{
            printf("MQTTPacket_read error\n\r");
            goto exit;
        }

        /* subscribe */
        topicString.cstring = pTopic;
        len = MQTTSerialize_subscribe(buf, buflen, 0, msgid, 1, &topicString, &req_qos);

        rc = transport_sendPacketBuffer(buf, len);
        if(rc != len){
        printf("connect transport_sendPacketBuffer error\n\r");
        goto exit;
        }
        if (MQTTPacket_read(buf, buflen, transport_getdata) == SUBACK) 	/* wait for suback */
        {
        unsigned short submsgid;
        int subcount;
        int granted_qos;

        rc = MQTTDeserialize_suback(&submsgid, 1, &subcount, &granted_qos, buf, buflen);
        if (granted_qos != 0)
        {
        	printf("granted qos != 0, %d\n\r", granted_qos);
        	goto exit;
        }
        }
        else
        goto exit;

        /* loop getting msgs on subscribed topic */
        topicString.cstring = pTopic;
        memset(buf,0,buflen);
        //transport_getdata�������ݻ����������Ƿ������Ͽ����Ӻ�ŷ���
        if (MQTTPacket_read(buf, buflen, transport_getdata) == PUBLISH){
        unsigned char dup;
        int qos;
        unsigned char retained;
        unsigned short msgid;
        int payloadlen_in;
        unsigned char* payload_in;
        MQTTString receivedTopic;

        rc = MQTTDeserialize_publish(&dup, &qos, &retained, &msgid, &receivedTopic,
        &payload_in, &payloadlen_in, buf, buflen);
        //printf("message arrived %d: %s\n\r", payloadlen_in, payload_in);
        strcpy(pMessage,(const char *)payload_in);
        }
        //printf("disconnecting\n\r");
        len = MQTTSerialize_disconnect(buf, buflen);
        rc = transport_sendPacketBuffer(buf, len);
        exit:
        transport_close();
        return rc;
    }















