#include "driver/gpio.h"
#include "led.h"

//���ƺ��
void led_red(int on)
{
    if(on==LED_ON)
    {
       gpio_set_level(LED_RED_IO, LED_ON);//����
    }
    else
    {
        gpio_set_level(LED_RED_IO, LED_OFF);//�ص�
    }
}

//�����̵�
void led_green(int on)
{
    if(on==LED_ON)
    {
       gpio_set_level(LED_GREEN_IO, LED_ON);//����
    }
    else
    {
        gpio_set_level(LED_GREEN_IO, LED_OFF);//�ص�
    }
}

//��������
void led_blue(int on)
{
    if(on==LED_ON)
    {
       gpio_set_level(LED_BLUE_IO, LED_ON);//����
    }
    else
    {
        gpio_set_level(LED_BLUE_IO, LED_OFF);//�ص�
    }
}


//LED��ʼ��
void initLed()
{
    //��ʼ���ʵƵ�IO�ڣ�
    //��ƣ�GPIO32���̵ƣ�GPIO15�����ƣ�GPIO16
    gpio_pad_select_gpio(LED_RED_IO);
    gpio_pad_select_gpio(LED_GREEN_IO);
    gpio_pad_select_gpio(LED_BLUE_IO);
    gpio_set_direction(LED_RED_IO, GPIO_MODE_OUTPUT);
    gpio_set_direction(LED_GREEN_IO, GPIO_MODE_OUTPUT);
    gpio_set_direction(LED_BLUE_IO, GPIO_MODE_OUTPUT);    
    
    //�غ�������ɫ��
    led_red(LED_OFF);
    led_green(LED_OFF);
    led_blue(LED_OFF);
}

