#ifndef _LED_H_
#define _LED_H_

//����LED�Ƶ�IO��
#define LED_RED_IO 		32  //��Ӧ��Ƶ�LED���̵�Ϊ15������Ϊ16
#define LED_GREEN_IO 	15  //��Ӧ��Ƶ�LED���̵�Ϊ15������Ϊ16
#define LED_BLUE_IO 	16  //��Ӧ��Ƶ�LED���̵�Ϊ15������Ϊ16

//����LED״̬
#define LED_ON          0   //LED������ƽΪ�͵�ƽ
#define LED_OFF         1   //LED�����ƽΪ�ߵ�ƽ

//���ƺ��
void led_red(int on);
//�����̵�
void led_green(int on);
//��������
void led_blue(int on);
//LED��ʼ��
void initLed();

#endif