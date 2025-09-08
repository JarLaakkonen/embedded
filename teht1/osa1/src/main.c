// *******************************
// This example declares two tasks
// and runs them in parallel

#include <zephyr/kernel.h>
#include <zephyr/sys/printk.h>
#include <zephyr/device.h>
#include <zephyr/drivers/gpio.h>
static const struct gpio_dt_spec red = GPIO_DT_SPEC_GET(DT_ALIAS(led0), gpios);
 static const struct gpio_dt_spec green = GPIO_DT_SPEC_GET(DT_ALIAS(led1), gpios);
 static const struct gpio_dt_spec blue = GPIO_DT_SPEC_GET(DT_ALIAS(led2), gpios);

// Task declarations
void task1(void *, void *, void*);
void task2(void *, void *, void*);
void task3(void *, void *, void*);
#define	STACKSIZE	500
#define	PRIORITY	5
K_THREAD_DEFINE(tid1,STACKSIZE,task1,NULL,NULL,NULL,PRIORITY,0,0);
K_THREAD_DEFINE(tid2,STACKSIZE,task2,NULL,NULL,NULL,PRIORITY,0,0);
K_THREAD_DEFINE(tid3,STACKSIZE,task3,NULL,NULL,NULL,PRIORITY,0,0);
int state= 0;
int timer = 0;
// Main program
int main(void) {
        
	while (true) {
                if (state == 0){
                init_led();
                 state = 1;
                timer = 1;
                }
                else{
                timer = 1;
                }
		printk("Hello from main\n");
		k_msleep(1000);
		// k_yield();
	}
	
	return 0;
}

// Task1 function
void task1(void *, void *, void*) {

	while (true) {
                if (state == 1 && timer == 1){
                gpio_pin_set_dt(&blue,0);
                gpio_pin_set_dt(&green,0);
		printk("Punainen valo\n");
                gpio_pin_set_dt(&red,1);
               // gpio_pin_set_dt(&red,0);
		state = 2;
                timer = 0;
                }
                k_msleep(100);
		// k_yield();
	}
}

// Task2 function
void task2(void *, void *, void*) {

	while (true) {
                if (state == 2 && timer == 1){
                gpio_pin_set_dt(&red,1);
                gpio_pin_set_dt(&green,1);
                gpio_pin_set_dt(&blue,0);
		printk("Keltainen valo\n");
                state = 3;
                timer = 0;
                }
		k_msleep(100);
               
		// k_yield();
	}
}

void task3(void *, void *, void*) {

        while (true) {
                if (state == 3 && timer == 1){
                 gpio_pin_set_dt(&blue,0);
                 gpio_pin_set_dt(&red,0);
                 gpio_pin_set_dt(&green,1);
                printk("Vihrea valo\n");
                state = 1;
                timer = 0;
                }
                k_msleep(100);
                
                // k_yield();
        }
}

// Initialize leds
int  init_led() {

	// Led pin initialization
	int ret = gpio_pin_configure_dt(&red, GPIO_OUTPUT_ACTIVE);
        int ret2 = gpio_pin_configure_dt(&green, GPIO_OUTPUT_ACTIVE);
        int ret3 = gpio_pin_configure_dt(&blue, GPIO_OUTPUT_ACTIVE);
        if (ret2 < 0) {
                printk("Error: Led configure failed\n");		
                return ret2;
        }
        if (ret3 < 0) {
                printk("Error: Led configure failed\n");                
                return ret3;
        }
	if (ret < 0) {
		printk("Error: Led configure failed\n");		
		return ret;
	}
	// set led off
	gpio_pin_set_dt(&red,0);

	printk("Led initialized ok\n");
	
	return 0;
}