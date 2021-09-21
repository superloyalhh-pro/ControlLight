#include <stdio.h>
#include "esp_system.h"
#include "esp_spi_flash.h"
#include "esp_wifi.h"
#include "esp_event_loop.h"
#include "esp_log.h"
#include "esp_err.h"
#include "nvs_flash.h"
#include "esp_event.h"
#include "Lcd.h"
#include <lwip/sockets.h>
#include "driver/gpio.h"

#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "freertos/semphr.h"
#include "mqtt_client.h"
#include "xMd5.h"
#include "cJSON.h"
#include "led.h"
#include "steer.h"
#include "http.h"
#include "camera.h"
#include "sdkconfig.h"

#define  DEFAULT_SSID "North217"        //��Ҫ���ӵ�WIFI����
#define  DEFAULT_PWD "meimimaa"   //wifi��Ӧ������

char productKey[] = {"gdn4HYkButt"};                        //���������ӵ���Ԫ�� �����Լ����Ϊ�Լ��Ĳ�Ʒ��Ϣ!!
char deviceName[] = {"ESP8266"};                        //���������ӵ���Ԫ�� �����Լ����Ϊ�Լ��Ĳ�Ʒ��Ϣ!!
char deviceSecret[] = {""}; //���������ӵ���Ԫ�� �����Լ����Ϊ�Լ��Ĳ�Ʒ��Ϣ!!
char regionId[] = {"cn-shanghai"};                          // ���������ӵ���Ԫ�� �����Լ����Ϊ�Լ��Ĳ�Ʒ��Ϣ !!

#define MQTT_DATA_PUBLISH "/gdn4HYkButt/ESP8266/user/update"
#define MQTT_DATA_SUBSCRIBE "/gdn4HYkButt/ESP8266/user/get"
static const char *TAG = "DOUBLE_H";

xTaskHandle xHandlerMqtt = NULL;
//�Ƿ����ӳɹ�������
static bool isConnectClouds = false;
//�豸mac
uint8_t mac[6];

//mqtt
static esp_mqtt_client_handle_t client;
static ip4_addr_t s_ip_addr,s_gw_addr,s_netmask_addr;

#define CAMERA_PIXEL_FORMAT CAMERA_PF_JPEG
#define CAMERA_FRAME_SIZE  CAMERA_FS_SVGA
#define QUALITY 5

//����ͷ
static camera_pixelformat_t s_pixel_format=CAMERA_PF_JPEG;

//����ͷ״̬
uint8_t camera_state=0; //0:û�д��� 1:��������ͷ���� 2����֧�ֵ�����ͷ 3����ʼ��ʧ��


static void post_data_to_clouds()
{
    cJSON *pRoot = cJSON_CreateObject();

    cJSON_AddBoolToObject(pRoot, "power", true);

    char *s = cJSON_Print(pRoot);
    //������Ϣ
    esp_mqtt_client_publish(client, MQTT_DATA_PUBLISH, s, strlen(s), 1, 0);
    cJSON_free((void *)s);
    cJSON_Delete(pRoot);
}


/**
 * @description: �ص�����
 * @param {type} 
 * @return: 
 */
esp_err_t MqttCloudsCallBack(esp_mqtt_event_handle_t event)
{
    int msg_id;
    client = event->client;
    switch (event->event_id)
    {
        //���ӳɹ�
    case MQTT_EVENT_CONNECTED:
        printf("MQTT_EVENT_CONNECTED\n");
        msg_id = esp_mqtt_client_subscribe(client, MQTT_DATA_SUBSCRIBE, 1);
        printf("sent subscribe successful, msg_id=%d\n", msg_id);
        printf("[APP] Free memory: %d bytes\n", esp_get_free_heap_size());
        post_data_to_clouds();
        break;
        //�Ͽ����ӻص�
    case MQTT_EVENT_DISCONNECTED:
        printf("MQTT_EVENT_DISCONNECTED\n");
        break;
        //���ĳɹ�
    case MQTT_EVENT_SUBSCRIBED:
        printf("MQTT_EVENT_SUBSCRIBED, msg_id=%d\n", event->msg_id);
        break;
        //����ʧ��
    case MQTT_EVENT_UNSUBSCRIBED:
        printf("MQTT_EVENT_UNSUBSCRIBED, msg_id=%d\n", event->msg_id);
        break;
        //���ͷ�����Ϣ�ɹ�
    case MQTT_EVENT_PUBLISHED:
        printf("MQTT_EVENT_PUBLISHED, msg_id=%d\n", event->msg_id);
        break;
        //�������·���Ϣ�����سɹ����ջص�
    case MQTT_EVENT_DATA:
    {
        ////�� ������json�ַ����еõ�cjson����
        cJSON *pJsonRoot = cJSON_Parse(event->data);

        //����Ƿ�json��ʽ����
        if (pJsonRoot == NULL)
        {
            break;
        }
        //���ݼ���ȡ��Ӧ��ֵ��cjson����
        cJSON *pCMD = cJSON_GetObjectItem(pJsonRoot, "cmd");
        cJSON *pValue = cJSON_GetObjectItem(pJsonRoot, "data");

        //�ж��ֶ��Ƿ�pChange��ʽ
        if (pCMD && pValue)
        {
            printf("xQueueReceive topic: %s \r\n", event->topic);
            printf("xQueueReceive payload: %s \r\n", event->data);
            printf("esp_get_free_heap_size : %d  \r\n", esp_get_free_heap_size());

            //�ж��ֶ��Ƿ�string����
            if (cJSON_IsArray(pValue))
                printf("get pChange:%s  \r\n", pCMD->valuestring);
            else
                break;
          //�����±��ȡcjosn���������еĶ���ֵ
           printf("pValue0:%d \r\n", cJSON_GetArrayItem(pValue, 0)->valueint);
           if((cJSON_GetArrayItem(pValue, 0)->valueint)==0){
                led_green(LED_OFF);
                Steer_Off();
           }
           else if((cJSON_GetArrayItem(pValue, 0)->valueint)==1){
                led_green(LED_ON);
                Steer_On();
           }
           else if((cJSON_GetArrayItem(pValue, 0)->valueint)==2){
               take_send_photo();
           }
        //    post_data_to_clouds();
        }
        else
            printf("get pCMD failed \n");
        cJSON_Delete(pJsonRoot);
        break;
    }
    default:
        break;
    }
    return ESP_OK;
}

void TaskXMqttRecieve(void *p)
{

    char BorkerHost[50];
    sprintf(BorkerHost, "%s.iot-as-mqtt.%s.aliyuncs.com", productKey, regionId);
    char UserName[50];
    sprintf(UserName, "%s&%s", deviceName, productKey);
    char ClientId[90];
    char messageSerect[200];
    sprintf(messageSerect, "clientId%02x:%02x:%02x:%02x:%02x:%02xdeviceName%sproductKey%stimestamp1995", mac[0], mac[1], mac[2], mac[3], mac[4], mac[5], deviceName, productKey);
    sprintf(ClientId, "%02x:%02x:%02x:%02x:%02x:%02x|securemode=3,signmethod=hmacmd5,timestamp=1995|", mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);

    char PassWord[80];
    HMAC_XH_MD5((uint8_t *)messageSerect, strlen(messageSerect), (uint8_t *)(deviceSecret), PassWord);

    printf("BorkerHost: %s\r\n", BorkerHost);
    printf("UserName: %s\r\n", UserName);
    printf("PassWord: %s\r\n", PassWord);
    printf("ClientId: %s\r\n", ClientId);

    //���ӵ����ò���
    esp_mqtt_client_config_t mqtt_cfg = {
        .host = BorkerHost,
        .username = UserName,
        .password = PassWord,
        .client_id = ClientId,
        .port = 1883,                       //�˿�
        .event_handle = MqttCloudsCallBack, //���ûص�����
        .keepalive = 200,
        .disable_auto_reconnect = false, //�����Զ�����
        .disable_clean_session = false,  //���� ����Ự
    };

    printf("TaskXMqttRecieve mqtt_cfg.host %s\r\n", mqtt_cfg.host);
    printf( "TaskXMqttRecieve mqtt_cfg.username %s\r\n", mqtt_cfg.username);
    printf("TaskXMqttRecieve mqtt_cfg.client_id %s\r\n", mqtt_cfg.client_id);
    printf("TaskXMqttRecieve mqtt_cfg.messageSerect %s\r\n", messageSerect);

    esp_mqtt_client_handle_t client = esp_mqtt_client_init(&mqtt_cfg);
    esp_err_t error = esp_mqtt_client_start(client);

    vTaskDelete(NULL);
}

//����s��0����ʼ������1�������С���2��������
void lcd_display(int s)
{
  char lcd_buff[100]={0};

  Gui_DrawFont_GBK24(15,0,RED,WHITE,(u8 *)"�Ĵ���ѧ");
  LCD_P6x8Str(10,24,WHITE,BLACK,(u8 *)"SiChuan University");
  Gui_DrawFont_GBK16(16,34,VIOLET,WHITE,(u8 *)"�Ĵ���ѧ");
  Gui_DrawFont_GBK16(32,50,BLUE,WHITE,(u8 *)"��ӭ��");

  //��ʾ���ӵ�wifi��Ϣ
  LCD_P6x8Str(0,70,BLACK,WHITE,(u8 *)"mode:STA");
  sprintf(lcd_buff, "ssid:%s",DEFAULT_SSID);
  LCD_P6x8Str(0,80,BLACK,WHITE,(u8 *)lcd_buff);
  sprintf(lcd_buff, "psw:%s",DEFAULT_PWD);
  LCD_P6x8Str(0,90,BLACK,WHITE,(u8 *)lcd_buff);

  if(2==s)
  {
    //2��������
    sprintf(lcd_buff, "ip:%s      ",ip4addr_ntoa(&s_ip_addr));
    LCD_P6x8Str(0,100,BLUE,WHITE,(u8 *)lcd_buff);        
    sprintf(lcd_buff, "gw:%s",ip4addr_ntoa(&s_gw_addr));
    LCD_P6x8Str(0,110,BLUE,WHITE,(u8 *)lcd_buff);        
    sprintf(lcd_buff, "mask:%s",ip4addr_ntoa(&s_netmask_addr));
    LCD_P6x8Str(0,120,BLUE,WHITE,(u8 *)lcd_buff);        
  }
  else
  {
    LCD_P6x8Str(0,100,RED,WHITE,(u8 *)"wifi connecting......");
  }
  
}

// wifi�¼�������
// ctx     :��ʾ������¼����Ͷ�Ӧ��Я���Ĳ���
// event   :��ʾ������¼�����
// ESP_OK  : succeed
// ����    :ʧ�� 
static esp_err_t event_handler(void *ctx, system_event_t *event)
{
    switch (event->event_id)
    {
        case SYSTEM_EVENT_STA_START://STA����
            printf("sta start.\r\n");
            //�޸��豸������
            ESP_ERROR_CHECK(tcpip_adapter_set_hostname(TCPIP_ADAPTER_IF_STA, "DOUBLE_H"));
            esp_wifi_connect();//��ʼ����wifi
            lcd_display(1);
            break;
        case SYSTEM_EVENT_STA_CONNECTED://STA��������
          printf("SYSTEM_EVENT_STA_CONNECTED.\r\n");
          break;
        case SYSTEM_EVENT_STA_GOT_IP://STAȡ��IP
            //ͨ��LOG���IP��Ϣ
            printf("\r\nip:%s.\r\n",ip4addr_ntoa(&event->event_info.got_ip.ip_info.ip));
            printf("gw:%s.\r\n",ip4addr_ntoa(&event->event_info.got_ip.ip_info.gw));
            printf("netmask:%s.\r\n",ip4addr_ntoa(&event->event_info.got_ip.ip_info.netmask));
            //����IP��Ϣ����LCD��ʾ
            s_ip_addr = event->event_info.got_ip.ip_info.ip;//����IP
            s_gw_addr = event->event_info.got_ip.ip_info.gw;//��������
            s_netmask_addr = event->event_info.got_ip.ip_info.netmask;//��������
            //LCD��ʾ
            lcd_display(2);
            esp_err_t ret = 0;

        printf("xHandlerMqtt\r\n");
        if (xHandlerMqtt == NULL)
        {
           printf("xTaskCreate xMqttStartConnect1 ok\r\n");
            ret = xTaskCreate(TaskXMqttRecieve, "TaskXMqttRecieve", 1024 * 4, NULL, 7, NULL); // ��������
        }

        if (!ret)
        {
            printf("xTaskCreate xMqttStartConnect1 fail\r\n");
        }
            printf("over\r\n");
            break;
        case SYSTEM_EVENT_STA_DISCONNECTED://STA�Ͽ�����
            esp_wifi_connect();//��ʼ����wifi
            lcd_display(1);
            break;
        default:
            break;
    }
    return ESP_OK;
}

//����WIFI��STA
void wifi_init_sta()
{
    tcpip_adapter_init();//tcp/IP����

    ESP_ERROR_CHECK(esp_event_loop_init(event_handler, NULL));//����wifi�¼��ص�����

    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));//wifiĬ�ϳ�ʼ��    
    ESP_ERROR_CHECK(esp_wifi_set_storage(WIFI_STORAGE_RAM));//NVS ���ڴ洢 WiFi ����.
    wifi_config_t wifi_config = {
        .sta = {
            .ssid = DEFAULT_SSID,//���ӵ��ȵ�����
            .password = DEFAULT_PWD,//���ӵ��ȵ�����
            .scan_method = WIFI_ALL_CHANNEL_SCAN,
            .sort_method = WIFI_CONNECT_AP_BY_SIGNAL,
            .threshold.rssi = -127,
        },
    };    
    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));//����wifi����ģʽΪSTA
    ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_STA, &wifi_config));//����AP����    
    ESP_ERROR_CHECK(esp_wifi_start());    //ʹ��wifi
}

void Camera_init(){
    //�Ƿ�ʹ������ͷ
    camera_config_t camera_config = {
        .ledc_channel = LEDC_CHANNEL_0,
        .ledc_timer = LEDC_TIMER_0,
        .pin_d0 = CONFIG_D0,
        .pin_d1 = CONFIG_D1,
        .pin_d2 = CONFIG_D2,
        .pin_d3 = CONFIG_D3,
        .pin_d4 = CONFIG_D4,
        .pin_d5 = CONFIG_D5,
        .pin_d6 = CONFIG_D6,
        .pin_d7 = CONFIG_D7,
        .pin_pclk = CONFIG_PCLK,
        .pin_vsync = CONFIG_VSYNC,
        .pin_href = CONFIG_HREF,
        .pin_sscb_sda = CONFIG_SDA,
        .pin_sscb_scl = CONFIG_SCL,
        .xclk_freq_hz = CONFIG_XCLK_FREQ,
        .pixel_format = CAMERA_PF_JPEG, //YUV422,GRAYSCALE,RGB565,JPEG
        .frame_size = CAMERA_FRAME_SIZE,    //QQVGA-UXGA Do not use sizes above QVGA when not JPEG

        .jpeg_quality = QUALITY, //0-63 lower number means higher quality
    };

    camera_model_t camera_model;
    esp_err_t err = camera_probe(&camera_config, &camera_model);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Camera probe failed with error 0x%x", err);
        camera_state=1;
    }

    //��⵽����ͷ
    if(camera_state==0)
    {
      if (camera_model == CAMERA_OV2640) {
          ESP_LOGI(TAG, "Detected OV2640 camera, using JPEG format");
          s_pixel_format = CAMERA_PIXEL_FORMAT;
          camera_config.frame_size = CAMERA_FRAME_SIZE;
          if (s_pixel_format == CAMERA_PF_JPEG)
          {
            camera_config.jpeg_quality = QUALITY;
          }
          else if(s_pixel_format == CAMERA_PF_GRAYSCALE){

          }
      } else {
          ESP_LOGE(TAG, "Camera not supported");
          camera_state=2;
      }

      
      //��⵽֧�ֵ�����ͷ
      if(camera_state==0)
      {
        camera_config.pixel_format = s_pixel_format;
        err = camera_init(&camera_config);
        if (err != ESP_OK) {
            ESP_LOGE(TAG, "Camera init failed with error 0x%x", err);
            camera_state=3;
        }
      }
    }

    ESP_LOGI(TAG, "Free heap: %u", xPortGetFreeHeapSize());
    ESP_LOGI(TAG, "Camera demo ready");  
}

//�û�������ڣ��൱��main����
void app_main()
{
    //��ʼ��NVS
    if (nvs_flash_init() != ESP_OK) {
        ESP_ERROR_CHECK( nvs_flash_erase() );
        ESP_ERROR_CHECK( nvs_flash_init() );
        printf("nvs error.\n");
    }

    initLed();//LED IO�ڳ�ʼ��
    Camera_init();//����ͷ��ʼ��
    Steer_init();//Steer��ʼ��
    //��ʾ����ʼ�����Լ�������ʾ
    Lcd_Init();
    lcd_display(0);

    //����WIFI��STA
    wifi_init_sta();
}





