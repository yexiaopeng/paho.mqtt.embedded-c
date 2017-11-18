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

    #include "transport.h"
    
    #include "stdint.h"
  
    

    //�򿪶˿�
    int transport_open(uint8_t * serverip)
    {

   
		//��ֹ���Ӳ�����·�����
        int8_t ret;
	label:
        //�½�һ��Socket���󶨱��ض˿�5000
        while(socket(SOCK_TCPS,Sn_MR_TCP,5000,0x00) != SOCK_TCPS)
        {
            DEBUG_printf("\r\n@@�½��׽���ʧ��\r\n");
            delay_ms(1000);
        }
       // DEBUG_printf("\r\n@@�½��׽��ֳɹ�\r\n");

        //����TCP������
        //DEBUG_printf("\r\n@@��ʼ���ӷ�����\r\n");
        
        ret = connect(SOCK_TCPS,serverip,1883);
        if(SOCKERR_PORTZERO == ret)
        {
            DEBUG_printf("\r\n@@���ӷ���������      �˿ڲ���Ϊ0  ");
            DEBUG_printf("\r\n@@���������ԣ������޸���֮������");
            delay_ms(5000);
    		goto label;
        }else if(SOCKERR_TIMEOUT == ret)
        {
            DEBUG_printf("\r\n@@���ӷ�������ʱ  ");
            //�Զ������õģ���Ҫ���³�ʼ��socket
            goto label;
        }else if(SOCK_OK == ret)
         {
            //DEBUG_printf("\r\n@@����SERVER: %d.%d.%d.%d:%d �ɹ�",serverip[0],serverip[1],serverip[2],serverip[3],1883);
        }else{
    		DEBUG_printf("\r\n@@����SERVER���󣬴����� %d   \r\n",ret);
            DEBUG_printf("\r\n@@�����豸������������\r\n");
            delay_ms(2000);
    		goto label;
    	}
		
		return 0;
    }


    //�رն˿�
    int transport_close(void)
    {
        close(SOCK_TCPS);
        return 0;
    }


    //��������
    int transport_sendPacketBuffer(unsigned char* buf, int buflen)
    {
        return send(SOCK_TCPS,buf,buflen);
    }

    //��ȡ����
    int transport_getdata(unsigned char* buf, int count)
    {
        return recv(SOCK_TCPS,buf,count);
    }

    int transport_getdatanb(void *sck, unsigned char* buf, int count)
    {
        
        return 0;
    }

