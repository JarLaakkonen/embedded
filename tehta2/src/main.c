/*
        HUOMIO 
        Tässä on tällä hetkellä 1 pisteen suoritus
        Päivitän tämän tekstin jos tilanne muuttuu

        Koodissa aluksi alustetaan kaikki tarvittavat elementit. sen jälkeen void loopissa alustetaan ledit ja Uart yhteys.
        Tämän jälkeen odotellaan että uartissa tulee dataa, varataan muistia sitten laitetaan se FIfoon. 
        Samalla kun data on tullut annetaan signaali että dataa tullu ja avataaan mutex.
        Tämän jälkeen led taski ottaa datan kerrallaan ja päivittää valoa 

*/



#include <zephyr/kernel.h>
#include <zephyr/sys/printk.h>
#include <zephyr/device.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/drivers/uart.h>

// UART initialization
#define UART_DEVICE_NODE DT_CHOSEN(zephyr_shell_uart)
static const struct device *const uart_dev = DEVICE_DT_GET(UART_DEVICE_NODE);

// Led pin configurations
static const struct gpio_dt_spec red = GPIO_DT_SPEC_GET(DT_ALIAS(led0), gpios);
static const struct gpio_dt_spec green = GPIO_DT_SPEC_GET(DT_ALIAS(led1), gpios);
static const struct gpio_dt_spec blue = GPIO_DT_SPEC_GET(DT_ALIAS(led2), gpios);

#define STACKSIZE 1024
#define PRIORITY 5
void led_task(void *,void *,void*);
void uart_task(void *,void *,void*);

K_THREAD_DEFINE(ledit_thread,STACKSIZE,led_task,NULL,NULL,NULL,PRIORITY,0,0);
K_THREAD_DEFINE(uart_thread,STACKSIZE,uart_task,NULL,NULL,NULL,PRIORITY,0,0);

struct k_mutex my_mutex;
struct k_condvar my_cond;

char Testi_val= 0;
// FIFOA VARTEN SO FIRST IN FIRST OUT
K_FIFO_DEFINE(data_fifo);
struct data_t {
	void *fifo_reserved;
	char data;
};




// tämä hoitaa fifon?
void uart_task(void *, void *, void*) {
    printk("UART task started\n");

    while (1) {
        char rc = 0;

        if (uart_poll_in(uart_dev, &rc) == 0) {
            struct data_t *item = k_malloc(sizeof(struct data_t));
            if (!item) {
                printk("malloc failed!\n");
                continue;
            }
            item->data = rc;
            k_fifo_put(&data_fifo, item);

            // Herätetään Ledi taski
            k_mutex_lock(&my_mutex, K_FOREVER);
            k_condvar_signal(&my_cond);
            k_mutex_unlock(&my_mutex);
        }

        k_msleep(10);  
    }
}
void led_task(void *, void *, void*) {
    while (1) {
        // Odotetaan signaalia UART-taskilta
    
        // Luetaan FIFO:sta
        struct data_t *received = k_fifo_get(&data_fifo, K_FOREVER);
        if (!received) continue;  // turvatarkistus

        switch (received->data) {
            case 'r':
            case 'R':
                gpio_pin_set_dt(&red, 1);
                gpio_pin_set_dt(&green, 0);
                gpio_pin_set_dt(&blue, 0);
                break;
            case 'g':
            case 'G':
                gpio_pin_set_dt(&red, 0);
                gpio_pin_set_dt(&green, 1);
                gpio_pin_set_dt(&blue, 0);
                break;
            case 'y':
            case 'Y':
                gpio_pin_set_dt(&red, 1);
                gpio_pin_set_dt(&green, 1);
                gpio_pin_set_dt(&blue, 0);
                break;
            default:
                gpio_pin_set_dt(&red, 0);
                gpio_pin_set_dt(&green, 0);
                gpio_pin_set_dt(&blue, 0);
                break;
        }

        k_free(received);
        k_msleep(1000); 
    }
}

int init_uart(void) {
	// UART initialization
	if (!device_is_ready(uart_dev)) {
		return 1;
	} 
	return 0;
}
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
int main(void) {
    // mutexia varten alustus
        k_mutex_init(&my_mutex);
        k_condvar_init(&my_cond);
        init_led();
	int ret = init_uart();
	if (ret != 0) {
		printk("UART initialization failed!\n");
		return ret;
	}
	k_msleep(1000);
	// k_yield();
}


