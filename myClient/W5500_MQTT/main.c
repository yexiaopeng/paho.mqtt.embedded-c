    /**
      ******************************************************************************
      * @file    main.c
      * @author  yexiaopeng
      * @version V1.0
      * @date    2017/11/6
      * @brief   Default main function.
      ******************************************************************************
    */

    #include "stdio.h"

    #include "bsp_spi.h"
    #include "socket.h"
    #include "dhcp.h"
    #include "bsp_am2302.h"
    #include "bsp_lock.h"

    //MQTT
    #include "w5500Mqtt.h"
    #include "string.h"
   

    
    #define FALSE 0
    #define TRUE  1



    /* Private define ------------------------------------------------------------*/
    //#define SOCK_TCPS        1
    #define SOCK_DHCP           0
    #define MY_MAX_DHCP_RETRY   3
    #define DATA_BUF_SIZE   2048

    /* Private macro -------------------------------------------------------------*/
    uint8_t gDATABUF[DATA_BUF_SIZE];


    #define DHCPMODE 0
    #define STATIPMODE !DHCPMODE



    // Default Network Configuration
    // TODO 默认的设置，在使用dhcp之后会有改变
    // 但需要我们配置mac的地址
    // 2017年11月14日 更改为使用指定的IP
    wiz_NetInfo gWIZNETINFO = { .mac = {0x84, 0x18, 0x69,0x00, 0x00, 0x01},
                                .ip = {192, 168, 9, 115},
                                .sn = {255,255,255,0},
                                .gw = {192, 168, 9, 1},
                                .dns = {0,0,0,0},
                                .dhcp = NETINFO_STATIC };

    //需要的设置
    // 1-loacl IP 2-gateway 3- server ip  4- server port 5-deviceid
    // loacl ip uint8_t[4]  
    // gateway  uint8_t[4]
    // serverip uint8_t[4]
    // server port uint8_t[4]
    // deviceid  uint8_t[8]  0X7B,0XC8,0X84  as mac
    // 一共5Xuint32
    // 其他几项在存储在gWIZNETINFO
    static uint8_t serverip[4];
    static uint8_t serverport[2]; 


    //static void my_ip_assign(void);
    void network_init(void);
    //static void my_ip_conflict(void);

	void App_Init(void);//系统初始化

	void App_Runtime(void);//正常工作模式

	//void App_Test(void);//测试模式





    int main(void)
    {
        App_Init();
        //App_Runtime();
        App_Runtime();
        for(;;);
    }


    static void App_Init(void)
    {
        SysTick_Init(); 
        bsp_led_init();
        bsp_usart_1_debug_init();
        SPI_Configuration();
        timer_InitConfig();
        bsp_am2302_io_init();
        bsp_lock_io_init();


        printf("\r\n@@设备启动");

        //读取设备信息
        bsp_flash_read_device_message();
        //判断是否已设置
        if(0 == bsp_flash_check_device_message_isConfig())
        {
            uint8_t  isConfig = 0;
            while(!isConfig)
           {
                DEBUG_printf("\r\n@@请使用工具配置设备信息 ");
                delay_ms(80);
                DEBUG_printf(".");
                delay_ms(80);
                DEBUG_printf(".");
                delay_ms(80);
                DEBUG_printf(".");
                delay_ms(80);
                DEBUG_printf(".");
                delay_ms(80);
                DEBUG_printf(".");
                delay_ms(80);
                DEBUG_printf(".");
                delay_ms(80);
                DEBUG_printf(".");
                delay_ms(80);
                DEBUG_printf(".");
                delay_ms(80);
                DEBUG_printf(".");
                delay_ms(80);
                DEBUG_printf(".");
                if(bsp_usart_1_debug_get_isFlashConfigOver())
                {
                    //数据判断
                    bsp_usart_1_debug_clear_isFlashConfigOver();
                    isConfig = 1;
                }
           }
           DEBUG_printf("\r\n@@设置信息配置完成");
        }

        memcpy(serverip,bsp_flash_get_device_serverip(),4);
        memcpy(serverport,bsp_flash_get_device_serverport()+2,2);
        memcpy(gWIZNETINFO.ip,bsp_flash_get_device_localip(),4);
        memcpy(gWIZNETINFO.gw,bsp_flash_get_device_gateway(),4);
        memcpy(gWIZNETINFO.mac,bsp_flash_get_device_deviceid()+2,6);
    }





     void App_Runtime(void)
    	{
   
         char meassage[200];//接受的字节长度
         int rc;
         char * name;
         char * action;
         network_init();


         timer_Start();
         while(1)
         {
             memset(meassage,0,sizeof(meassage));
            //订阅消息
             rc = mqtt_subscrib(serverip,"PLLOCK",meassage,15);
             //在设定的时间内，接受到了数据   
             if(rc >= 0)
             {
                 printf("meassage = %s\n\r",meassage);
                 cJSON *root = cJSON_Parse(meassage); //解析cjson
                 if(NULL !=root)
                 {
                    name = cJSON_GetObjectItem(root,"name")->valuestring;
                    printf(" name = %s\n\r",name);

                    if(strcmp(name,"lock") == 0)
                    {
                      
                            action = cJSON_GetObjectItem(root,"action")->valuestring;
                            printf(" action = %s \n\r",action);
                    }
                    
                    
                 }
                 
                  cJSON_Delete(root); 
             }else{
             printf(" mo meassage  \n\r" );

             }

             
             if(timer_get_timeover_flag())
             {
                timer_stop();
                timer_set_timeover_flag(0);
                printf("timer_set_timeover_flag \n\r");
                mqtt_publish(serverip,"PLLOCK","heart");
                timer_Start();
             }
    
             
         }


    
    }


    void network_init(void)
    {
		uint8_t tmp;
		 uint8_t tmpstr[6];
		uint8_t memsize[2][8] = {{2,2,2,2,2,2,2,2},{2,2,2,2,2,2,2,2}};
        reg_wizchip_cris_cbfunc(SPI_CrisEnter, SPI_CrisExit);
        reg_wizchip_cs_cbfunc(SPI_CS_Select, SPI_CS_Deselect);     
        reg_wizchip_spi_cbfunc(SPI_ReadByte, SPI_WriteByte);

        if(ctlwizchip(CW_INIT_WIZCHIP,(void*)memsize) == -1)
         {
            DEBUG_printf("WIZCHIP Initialized fail.\r\n");
            while(1);
         }

        /* PHY link status check */
        //需要网络
        //无网络时阻塞
        //插上网线可运行
        do{
             if(ctlwizchip(CW_GET_PHYLINK, (void*)&tmp) == -1){
                    DEBUG_printf("Unknown PHY Link stauts.\r\n");
             }
        }while(tmp == PHY_LINK_OFF);



        // TODO  配置MAC的地址
        // 需要在setSHAR之前对MAC地址重新写入
        //STM32F030系列不存在唯一ID
        //使用设备编号进行加密好了
        setSHAR(gWIZNETINFO.mac);

        ctlnetwork(CN_SET_NETINFO, (void*)&gWIZNETINFO);

        // Display Network Information
        ctlwizchip(CW_GET_ID,(void*)tmpstr);
        printf("\r\n=== %s NET CONF ===\r\n",(char*)tmpstr);
        printf("MAC: %02X:%02X:%02X:%02X:%02X:%02X\r\n",gWIZNETINFO.mac[0],gWIZNETINFO.mac[1],gWIZNETINFO.mac[2],
        	  gWIZNETINFO.mac[3],gWIZNETINFO.mac[4],gWIZNETINFO.mac[5]);
        printf("SIP: %d.%d.%d.%d\r\n", gWIZNETINFO.ip[0],gWIZNETINFO.ip[1],gWIZNETINFO.ip[2],gWIZNETINFO.ip[3]);
        printf("GAR: %d.%d.%d.%d\r\n", gWIZNETINFO.gw[0],gWIZNETINFO.gw[1],gWIZNETINFO.gw[2],gWIZNETINFO.gw[3]);
        printf("SUB: %d.%d.%d.%d\r\n", gWIZNETINFO.sn[0],gWIZNETINFO.sn[1],gWIZNETINFO.sn[2],gWIZNETINFO.sn[3]);
        printf("DNS: %d.%d.%d.%d\r\n", gWIZNETINFO.dns[0],gWIZNETINFO.dns[1],gWIZNETINFO.dns[2],gWIZNETINFO.dns[3]);
        printf("======================\r\n");

    }




