#ifndef __HTTP_H
#define __HTTP_H

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "esp_wifi.h"
#include "esp_wpa2.h"
#include "esp_event_loop.h"
#include "esp_log.h"
#include "esp_system.h"
#include "nvs_flash.h"
#include "tcpip_adapter.h"
#include "esp_smartconfig.h"
#include "lwip/err.h"
#include "lwip/sockets.h"
#include "lwip/sys.h"
#include "lwip/netdb.h"
#include "lwip/dns.h"
#include "cJSON.h"
#include <sys/param.h>
#include "esp_http_client.h"
#include "camera.h"
#include "dht11.h"

#define WEB_SERVER          "img.bemfa.com"    
#define WEB_PORT            "8347"
#define WEB_URL             "http://images.bemfa.com/upload/v1/upimages.php" // 默认上传地址
#define UID                 "94f07762bcdb510c4df4f9bf9fd1b2e5"    //用户私钥，巴法云控制台获取
#define TOPIC               "WechatPic"     //主题名字，可在控制台新建

char* TTAG="HH";

esp_err_t _http_event_handler(esp_http_client_event_t *evt)
{
    switch (evt->event_id)
    {
    case HTTP_EVENT_ERROR:
        ESP_LOGD(TTAG, "HTTP_EVENT_ERROR");
        break;
    case HTTP_EVENT_ON_CONNECTED:
        ESP_LOGD(TTAG, "HTTP_EVENT_ON_CONNECTED");
        break;
    case HTTP_EVENT_HEADER_SENT:
        ESP_LOGD(TTAG, "HTTP_EVENT_HEADER_SENT");
        break;
    case HTTP_EVENT_ON_HEADER:
        ESP_LOGD(TTAG, "HTTP_EVENT_ON_HEADER, key=%s, value=%s", evt->header_key, evt->header_value);
        break;
    case HTTP_EVENT_ON_DATA:
        ESP_LOGD(TTAG, "HTTP_EVENT_ON_DATA, len=%d", evt->data_len);
        if (!esp_http_client_is_chunked_response(evt->client))
        {
            // Write out data
            printf("%.*s", evt->data_len, (char*)evt->data);
        }
        break;
    case HTTP_EVENT_ON_FINISH:
        ESP_LOGD(TTAG, "HTTP_EVENT_ON_FINISH");
        break;
    case HTTP_EVENT_DISCONNECTED:
        ESP_LOGD(TTAG, "HTTP_EVENT_DISCONNECTED");
        break;
    }
    return ESP_OK;
}

static void take_send_photo(){
    printf("Http Start");
    size_t _jpg_buf_len;
    uint8_t *_jpg_buf;
    esp_err_t res = ESP_OK;
    size_t fb_len = 0;

    //配置服务器相关信息
    esp_http_client_config_t config = {
        .url = WEB_URL,
        .method = HTTP_METHOD_POST,
        .event_handler = _http_event_handler,
        .buffer_size = 16384,
        .timeout_ms = 10000,
    };

    esp_http_client_handle_t client = esp_http_client_init(&config);

    //开始拍照
    esp_err_t err = camera_run();
    if (err != ESP_OK)
    {
        printf("Camera capture failed");
    }
    else
    {
        //拍照成功，获取其大小、尺寸等信息
        printf("Camera capture OK , Its size was: %zu bytes\n",camera_get_data_size());
        printf("Camera capture OK , Its width was: %d\n", camera_get_fb_width());
        printf("Camera capture OK , Its height was: %d\n", camera_get_fb_height());

        if (res != ESP_OK)
        {
            printf("esp_http_client_set_header result code : [%s]", esp_err_to_name(res));
        }

        //把图片放在body里面
        res = esp_http_client_set_post_field(client, (char*)camera_get_fb(), camera_get_fb_size());
        printf("pointer=%p, size=%d\n", (char*)camera_get_fb(), camera_get_fb_size());
        //设置HTTP请求头为image/jpg表示图片类型
        res = esp_http_client_set_header(client, "Content-Type", "image/jpeg");
        res = esp_http_client_set_header(client, "Authorization", UID);
        res = esp_http_client_set_header(client, "Authtopic", TOPIC);
        //开始执行请求服务器
        res = esp_http_client_perform(client);
        //判断是否请求成功
        if (res != ESP_OK)
        {
            printf("esp_http_client_set_post_field result  code : [%s]", esp_err_to_name(res));
        }else{
            printf("HTTPS Status = %d, content_length = %d\n", esp_http_client_get_status_code(client), esp_http_client_get_content_length(client));
        }
        printf( "Free heap: %u. data_size=%d.\r\n", xPortGetFreeHeapSize(),camera_get_data_size());

        esp_http_client_cleanup(client);
        printf("Http End\n");
        return;
    }

    
    return;
}

#endif