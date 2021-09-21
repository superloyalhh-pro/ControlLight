#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "soc/gpio_reg.h"
#include "soc/io_mux_reg.h"
#include "driver/gpio.h"
#include "DHT11.h"
//
#define DHT11_PIN   4//����DHT11������

//��ʪ�ȶ���
uchar ucharFLAG,uchartemp;
uchar shidu,wendu;
uchar ucharT_data_H,ucharT_data_L,ucharRH_data_H,ucharRH_data_L,ucharcheckdata;
uchar ucharT_data_H_temp,ucharT_data_L_temp,ucharRH_data_H_temp,ucharRH_data_L_temp,ucharcheckdata_temp;
uchar ucharcomdata;

static void InputInitial(void)//���ö˿�Ϊ����
{
  gpio_pad_select_gpio(DHT11_PIN);
  gpio_set_direction(DHT11_PIN, GPIO_MODE_INPUT);
}

static void OutputHigh(void)//���1
{
  gpio_pad_select_gpio(DHT11_PIN);
  gpio_set_direction(DHT11_PIN, GPIO_MODE_OUTPUT);
  gpio_set_level(DHT11_PIN, 1);
}

static void OutputLow(void)//���0
{
  gpio_pad_select_gpio(DHT11_PIN);
  gpio_set_direction(DHT11_PIN, GPIO_MODE_OUTPUT);
  gpio_set_level(DHT11_PIN, 0);
}

static uint8 getData()//��ȡ״̬
{
	return gpio_get_level(DHT11_PIN);
}



//��ȡһ���ֽ�����
static void COM(void)    // ��ʪд��
{
    uchar i;
    for(i=0;i<8;i++)
    {
        ucharFLAG=2;

        //�ȴ�IO�ڱ�ͣ���ͺ�ͨ����ʱȥ�ж���0����1
        while((getData()==0)&&ucharFLAG++) ets_delay_us(10);
        ets_delay_us(35);//��ʱ35us
        uchartemp=0;

        //������λ��1��35us�󣬻���1������Ϊ0
        if(getData()==1) uchartemp=1;
        ucharFLAG=2;

        //�ȴ�IO�ڱ�ߣ���ߺ󣬱�ʾ���Զ�ȡ��һλ
        while((getData()==1)&&ucharFLAG++) ets_delay_us(10);
        if(ucharFLAG==1)break;
        ucharcomdata<<=1;
        ucharcomdata|=uchartemp;
    }
}

void Delay_ms(uint16 ms)
{
	int i=0;
	for(i=0; i<ms; i++){
		ets_delay_us(1000);
	}
}

void DHT11(void)   //��ʪ��������
{
    OutputLow();
    Delay_ms(19);  //>18MS
    OutputHigh();
    InputInitial(); //����
    ets_delay_us(30);
    if(!getData())//��ʾ��������������
    {
        ucharFLAG=2;
        //�ȴ����߱�����������
        while((!getData())&&ucharFLAG++) ets_delay_us(10);
        ucharFLAG=2;
        //�ȴ����߱�����������
        while((getData())&&ucharFLAG++) ets_delay_us(10);
        COM();//��ȡ��1�ֽڣ�
        ucharRH_data_H_temp=ucharcomdata;
        COM();//��ȡ��2�ֽڣ�
        ucharRH_data_L_temp=ucharcomdata;
        COM();//��ȡ��3�ֽڣ�
        ucharT_data_H_temp=ucharcomdata;
        COM();//��ȡ��4�ֽڣ�
        ucharT_data_L_temp=ucharcomdata;
        COM();//��ȡ��5�ֽڣ�
        ucharcheckdata_temp=ucharcomdata;
        OutputHigh();
        //�ж�У����Ƿ�һ��
        uchartemp=(ucharT_data_H_temp+ucharT_data_L_temp+ucharRH_data_H_temp+ucharRH_data_L_temp);
        if(uchartemp==ucharcheckdata_temp)
        {
            //У���һ�£�
            ucharRH_data_H=ucharRH_data_H_temp;
            ucharRH_data_L=ucharRH_data_L_temp;
            ucharT_data_H=ucharT_data_H_temp;
            ucharT_data_L=ucharT_data_L_temp;
            ucharcheckdata=ucharcheckdata_temp;
            //�����¶Ⱥ�ʪ��
            shidu=ucharRH_data_H;
            wendu=ucharT_data_H;
        }
        else
        {
          shidu=100;
          wendu=100;
        }
    }
    else //û�óɹ���ȡ������0
    {
    	shidu=0,
    	wendu=0;
    }

    OutputHigh(); //���
}
