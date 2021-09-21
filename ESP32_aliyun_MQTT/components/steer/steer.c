#include "steer.h"

void Steer_init(){
    mcpwm_gpio_init(MCPWM_UNIT_0, MCPWM0A, STEER_IO);    //Set GPIO 18 as PWM0A, to which servo is connected

    printf("Configuring Initial Parameters of mcpwm......\n");
    mcpwm_config_t pwm_config;
    pwm_config.frequency = 50;    //frequency = 50Hz, i.e. for every servo motor time period should be 20ms
    pwm_config.cmpr_a = DUTY0;    //duty cycle of PWMxA = 0
    pwm_config.cmpr_b = DUTY0;    //duty cycle of PWMxb = 0
    pwm_config.counter_mode = MCPWM_UP_COUNTER;
    pwm_config.duty_mode = MCPWM_DUTY_MODE_0;
    mcpwm_init(MCPWM_UNIT_0, MCPWM_TIMER_0, &pwm_config);    //Configure PWM0A & PWM0B with above settings
    mcpwm_set_duty_in_us(MCPWM_UNIT_0, MCPWM_TIMER_0, MCPWM_OPR_A, DUTY0);
}

void Steer_On(){
    uint32_t angle = servo_per_degree_init(DUTY_ON);
    mcpwm_set_duty_in_us(MCPWM_UNIT_0, MCPWM_TIMER_0, MCPWM_OPR_A, angle);
     vTaskDelay(TIME_DELAY);
     //Add delay, since it takes time for servo to rotate, generally 100ms/60degree rotation at 5V
    angle = servo_per_degree_init(DUTY0);
    mcpwm_set_duty_in_us(MCPWM_UNIT_0, MCPWM_TIMER_0, MCPWM_OPR_A, angle);
}

void Steer_Off(){
    uint32_t angle = servo_per_degree_init(DUTY_OFF);
    mcpwm_set_duty_in_us(MCPWM_UNIT_0, MCPWM_TIMER_0, MCPWM_OPR_A, angle);
     vTaskDelay(TIME_DELAY);
     //Add delay, since it takes time for servo to rotate, generally 100ms/60degree rotation at 5V
    angle = servo_per_degree_init(DUTY0);
    mcpwm_set_duty_in_us(MCPWM_UNIT_0, MCPWM_TIMER_0, MCPWM_OPR_A, angle);
}



