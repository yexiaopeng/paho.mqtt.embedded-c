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
  
    

    //打开端口
    int transport_open(uint8_t * serverip)
    {

   
		//防止连接不上网路的情况
        int8_t ret;
	label:
        //新建一个Socket并绑定本地端口5000
        while(socket(SOCK_TCPS,Sn_MR_TCP,5000,0x00) != SOCK_TCPS)
        {
            DEBUG_printf("\r\n@@新建套接字失败\r\n");
            delay_ms(1000);
        }
       // DEBUG_printf("\r\n@@新建套接字成功\r\n");

        //连接TCP服务器
        //DEBUG_printf("\r\n@@开始连接服务器\r\n");
        
        ret = connect(SOCK_TCPS,serverip,1883);
        if(SOCKERR_PORTZERO == ret)
        {
            DEBUG_printf("\r\n@@连接服务器错误      端口不能为0  ");
            DEBUG_printf("\r\n@@请重启再试，或是修改配之后重启");
            delay_ms(5000);
    		goto label;
        }else if(SOCKERR_TIMEOUT == ret)
        {
            DEBUG_printf("\r\n@@连接服务器超时  ");
            //自定义设置的，需要重新初始化socket
            goto label;
        }else if(SOCK_OK == ret)
         {
            //DEBUG_printf("\r\n@@连接SERVER: %d.%d.%d.%d:%d 成功",serverip[0],serverip[1],serverip[2],serverip[3],1883);
        }else{
    		DEBUG_printf("\r\n@@连接SERVER错误，错误码 %d   \r\n",ret);
            DEBUG_printf("\r\n@@请检测设备或是重启再试\r\n");
            delay_ms(2000);
    		goto label;
    	}
		
		return 0;
    }


    //关闭端口
    int transport_close(void)
    {
        close(SOCK_TCPS);
        return 0;
    }


    //发送数据
    int transport_sendPacketBuffer(unsigned char* buf, int buflen)
    {
        return send(SOCK_TCPS,buf,buflen);
    }

    //获取数据
    int transport_getdata(unsigned char* buf, int count)
    {
        return recv(SOCK_TCPS,buf,count);
    }

    int transport_getdatanb(void *sck, unsigned char* buf, int count)
    {
        
        return 0;
    }

