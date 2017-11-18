    /*******************************************************************************
     * Copyright (c) 2014 IBM Corp.
     *
     * All rights reserved. This program and the accompanying materials
     * are made available under the terms of the Eclipse Public License v1.0
     * and Eclipse Distribution License v1.0 which accompany this distribution.
     *
     * The Eclipse Public License is available at
     *    http://www.eclipse.org/legal/epl-v10.html
     * and the Eclipse Distribution License is available at
     *   http://www.eclipse.org/org/documents/edl-v10.php.
     *
     * Contributors:
     *    Ian Craggs - initial API and implementation and/or initial documentation
     *    Sergio R. Caprile - "commonalization" from prior samples and/or documentation extension
     *******************************************************************************/
    #ifndef TRANSPORT_MQTTPACK_INC
    #define TRANSPORT_MQTTPACK_INC

    #include "Include.h"

    #include <stdlib.h>
    #include <stdint.h>
    #include <string.h>
    #include "MQTTPacket.h"
    #include "socket.h"

    #include "bsp_usart_1_debug.h"
    #include "NT_Systick.h"
    #include "bsp_spi.h"

	#include "stdint.h"
	
    int transport_sendPacketBuffer(unsigned char* buf, int buflen);
    int transport_getdata(unsigned char* buf, int count);
    int transport_getdatanb(void *sck, unsigned char* buf, int count);
    int transport_open(uint8_t * serverip);
    int transport_close(void);

    #endif
