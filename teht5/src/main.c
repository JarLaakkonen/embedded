/*
                     HUOMIO Opelle! 16/10/2025
             Tässä on käsittääkseni 3 pisteen suoritus.
            Koodi on muokattu mainin osalta siten, että kaikki ylimääräiset print komennot pois
            ja Koodi ottaa vastaan viestit jotka loppuu x ja lähettää print("%dX",data).
            Robot framework koodi löytyy robot kansiosta ja sen ajetun koodin kuvakaappaus löytyy
            nimeltä RoboTesti_ulostulo.
            Tässä on testattu 1 pisteen minimit , lisätestit hhmmss ja lopuksi testattu 
            Liikennevalo sisään ottoa.
       
*/



#include <zephyr/kernel.h>
#include <zephyr/sys/printk.h>
#include <zephyr/device.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/drivers/uart.h>
#include <ctype.h>
#include <stdlib.h>
#include <string.h>
// UART initialization
#define UART_DEVICE_NODE DT_CHOSEN(zephyr_shell_uart)
static const struct device *const uart_dev = DEVICE_DT_GET(UART_DEVICE_NODE);

// Led pin configurations
static const struct gpio_dt_spec red = GPIO_DT_SPEC_GET(DT_ALIAS(led0), gpios);
static const struct gpio_dt_spec green = GPIO_DT_SPEC_GET(DT_ALIAS(led1), gpios);
static const struct gpio_dt_spec blue = GPIO_DT_SPEC_GET(DT_ALIAS(led2), gpios);

#define STACKSIZE 1024
#define PRIORITY 5
// init
int init_led(void);
int init_uart(void);
// Parse funktiot
int time_parse(char *time);
int light_parse(const char *light);
// taski
void led_task(void *,void *,void*);
void uart_task(void *,void *,void*);

K_THREAD_DEFINE(ledit_thread,STACKSIZE,led_task,NULL,NULL,NULL,PRIORITY,0,0);
K_THREAD_DEFINE(uart_thread,STACKSIZE,uart_task,NULL,NULL,NULL,PRIORITY,0,0);
// jos tulee error niin mennään taskiin
void error_task(int er_num);
void light_error_task(int er_val);
struct k_mutex my_mutex;
struct k_condvar my_cond;
// error koodit lisätty numeroita jotta tiedetään mikä error robotframe
#define TIME_LEN_ERROR      -11
#define TIME_VALUE_OVERFLOW_ERROR    -22
#define TIME_VALUE_NEGATIVE_ERROR    -33
#define TIME_VALUE_SIZE_ERROR       -44
#define TIME_VALUE_CHAR_ERROR       -55
#define TIME_VALUE_NULL_ERROR      -66
#define TIME_SECONDS_ZERO_ERROR      -77
//Lisää error koodeja GRY Varten
#define LIGHT_LEN_ERROR -1
#define LIGHT_LOWER_CASE_ERROR -2
#define LIGHT_NUMERIC_ERROR -3
#define LIGHT_NOT_CHAR_ERROR -4
#define LIGHT_INVALID_VALUE_ERROR -5
#define LIGHT_NULL_ERROR -6
#define LIGHT_VALUES_OK 0
// Timer initializations
struct k_timer timer;
void timer_handler(struct k_timer *timer_id);

char Testi_val= 0;
// FIFOA VARTEN SO FIRST IN FIRST OUT
K_FIFO_DEFINE(data_fifo);
struct data_t {
	void *fifo_reserved;
	char text[128];
};
void timer_handler(struct k_timer *timer_id) {
//	printk("HHMMSS was in a correct form\n");
    gpio_pin_set_dt(&red, 0);
    gpio_pin_set_dt(&green, 1);
    gpio_pin_set_dt(&blue, 0);
}
int main(void) {
    // mutexia varten alustus ja timer
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



void uart_task(void *, void *, void*) {
    printk("UART task started\n");

    char buffer[128];
    int index = 0;

    while (1) {
        char rc = 0;

        if (uart_poll_in(uart_dev, &rc) == 0) {
            if (rc == 'X') {

                if (index > 0) {
                    buffer[index] = '\0';
                    index = 0;

                    // Jos ei ala  R/Y/G 
                    // aluksi käytin tätä testaamiseen mutta 3p pisteen suorituksessa laitoin vain
                    // jos ensimmäinen arvo on 0, jotta voin testata sitä paremmin.
                    // if (buffer[0] != 'R' && buffer[0] != 'Y' && buffer[0] != 'G') { 
                     if (buffer[0] == '0'){
                 //       printk("Test value was %s \n",buffer);
                        int ret = time_parse(buffer);

                        if (ret >= 0) {
                            k_timer_init(&timer, timer_handler, NULL );
                            // Käynnistä ajastin palautetun ajan perusteella
                          //  printk("lets go to the interrupt");
                            printk("%dX",ret);
                            k_timer_init(&timer, timer_handler, NULL );
                            k_timer_start(&timer, K_SECONDS(1), K_NO_WAIT);
                            continue;
                        } else {
                            // Parserivirhe
                            printk("%dX",ret); 
                            error_task(ret);
                            continue;
                        }
                    }
                    else {
                        int val = light_parse(buffer);
                     //   printk("Test value was %s \n",buffer);
                        if (val >= 0){
                         //   printk("value ok : %d\n", val);
                            printk("%dX", val);
                        }
                        else {
                            printk("%dX",val);
                            
                            light_error_task(val);
                            continue;
                            
                        }
                    }
                    // Muuten ledi komennot jotta vanha koodi toimii
                    struct data_t *item = k_malloc(sizeof(struct data_t));
                    if (!item) {
                        printk("malloc failed!\n");
                        continue;
                    }

                    strcpy(item->text, buffer);
                    k_fifo_put(&data_fifo, item);

                    // Herätä LED-taski
                    k_mutex_lock(&my_mutex, K_FOREVER);
                    k_condvar_signal(&my_cond);
                    k_mutex_unlock(&my_mutex);

                //    printk("Task is : %s\n", buffer);
                }
            } else {
                // Lisätään merkki puskuriin
                if (index < sizeof(buffer) - 1) {
                    buffer[index++] = rc;
                }
            }
        }

        k_msleep(10);
    }
}
int light_parse(const char *light) {
    int value = LIGHT_LEN_ERROR;

    // Tarkista että merkkijono ei ole NULL tai tyhjä
    if (light == NULL || strlen(light) == 0) {
        return LIGHT_NULL_ERROR;
    }
    int length = strlen(light); // Käydään läpi jokainen merkki
    for (int i = 0; i < length; i++) {
        char c = light[i];
        if (isdigit((unsigned char)c)) {
            return LIGHT_NUMERIC_ERROR;  // Numero ei sallittu
        }
        else if (islower((unsigned char)c)) {
            return LIGHT_LOWER_CASE_ERROR;  // Pienet kirjaimet ei sallittu
        }
        else if (!isalpha((unsigned char)c)) {
            return LIGHT_NOT_CHAR_ERROR; 
        }
		 else if (c != 'R' && c != 'Y' && c != 'G') {
            return LIGHT_INVALID_VALUE_ERROR; // Sallitaan vain arvot R Y G
        }
    }
    value = LIGHT_VALUES_OK;    // Kaikki ok
    return value;
}
void light_error_task(int er_val){
        gpio_pin_set_dt(&red, 0);
        gpio_pin_set_dt(&green, 0);
        gpio_pin_set_dt(&blue, 1);
        /*
    switch (er_val) { // vähä vaihtelua elämään nii ei tuu nii hirveetä if pläjäystä.
        case  LIGHT_LEN_ERROR:
            printk("LIGHT_LEN_ERROR\n");
            break;
        case LIGHT_NULL_ERROR:
            printk("LIGHT_NULL_ERROR\n");
            break;
        case LIGHT_NUMERIC_ERROR:
            printk("LIGHT_NUMERIC_ERROR\n");
            break;
        case LIGHT_LOWER_CASE_ERROR:
            printk("LIGHT_LOWER_CASE_ERROR\n");
            break;
        case  LIGHT_NOT_CHAR_ERROR:
            printk(" LIGHT_NOT_CHAR_ERROR\n");
            break;
        case LIGHT_INVALID_VALUE_ERROR:
            printk("LIGHT_INVALID_VALUE_ERROR\n");
            break;
        default:
            printk("UNKNOWN_ERROR (%d)\n", er_val);
            break;
    }
            */

}
void error_task(int er_num){
        gpio_pin_set_dt(&red, 1);
        gpio_pin_set_dt(&green, 0);
        gpio_pin_set_dt(&blue, 0);
        /*
    if (er_num == TIME_LEN_ERROR){
        printk("TIME_LEN_ERROR\n");
    }  
         else if (er_num == TIME_VALUE_NULL_ERROR){
        printk("TIME_VALUE_NULL_ERROR \n");
    }
    else if (er_num == TIME_VALUE_OVERFLOW_ERROR ){
        printk("TIME_VALUE_OVERFLOW_ERROR \n");
    }
        else if (er_num == TIME_VALUE_NEGATIVE_ERROR){
        printk("TIME_VALUE_NEGATIVE_ERROR \n");
    }
        else if (er_num == TIME_VALUE_SIZE_ERROR ){
        printk("TIME_VALUE_SIZE_ERROR \n");
        }
        else if (er_num == TIME_VALUE_CHAR_ERROR){
        printk("TIME_VALUE_CHAR_ERROR \n");
    }
      else if (er_num == TIME_SECONDS_ZERO_ERROR){
        printk("IME_SECONDS_ZERO_ERROR \n");
    }
    else { 
        printk("This should not be possible\n");
    }
*/        
}

int time_parse(char *time) {
	// jos ei muuta niin antaa time len error eli sitten koodi ei toimi
	int seconds = TIME_LEN_ERROR;
	// onko arvo 0
	 if (time == NULL){
		return 	TIME_VALUE_NULL_ERROR;
	}
    if (time[0] == '\0') {
    return TIME_VALUE_NULL_ERROR;
}
	// liian iso tai pieni
	if (strlen(time) != 6) {  // tarkistetaan pituus täsmälleen 6
    return TIME_VALUE_SIZE_ERROR;
	}
	// kirjaimia seassa
	for (int i = 0; i < 6;i++){
		if (time[i] == '-') {
			return TIME_VALUE_NEGATIVE_ERROR;
		}
		if (!isdigit((unsigned char)time[i])){
			return TIME_VALUE_CHAR_ERROR;
		}
	}
    int values[3];
	values[2] = atoi(time+4); // seconds
	time[4] = 0;
	values[1] = atoi(time+2); // minutes
	time[2] = 0;
	values[0] = atoi(time); // hours
	// liian isoja arvoja
	if (values[0] > 23 || values[1] > 59 || values[2] > 59) {
		seconds = TIME_VALUE_OVERFLOW_ERROR;
	}
	else {
		seconds = values[0]*3600 + values[1]*60 + values[2] ;
	}
    if (seconds == 0){
        return TIME_SECONDS_ZERO_ERROR;
    }
	return seconds;
}

void led_task(void *, void *, void*) {
    while (1) {
        struct data_t *received = k_fifo_get(&data_fifo, K_FOREVER);
        if (!received) continue;

        printk("LED task got: %s\n", received->text);

        // Käydään läpi jokainen kirjain rivissä
        for (int i = 0; received->text[i] != '\0'; i++) {
            char c = received->text[i];
            switch (c) {
                case 'R':
                    gpio_pin_set_dt(&red, 1);
                    gpio_pin_set_dt(&green, 0);
                    gpio_pin_set_dt(&blue, 0);
                    break; 
                case 'G':
                    gpio_pin_set_dt(&red, 0);
                    gpio_pin_set_dt(&green, 1);
                    gpio_pin_set_dt(&blue, 0);
                    break;
                case 'Y':
                    gpio_pin_set_dt(&red, 1);
                    gpio_pin_set_dt(&green, 1);
                    gpio_pin_set_dt(&blue, 0);
                    break;
                default:
                    gpio_pin_set_dt(&red, 0);
                    gpio_pin_set_dt(&green, 0);
                    gpio_pin_set_dt(&blue, 0);
                    printk("Value is not  G R Y\n");
                    break;
            }

            k_msleep(500); // pieni viive jokaisen vaihdon välillä
        }

        // Vapautetaan 
        k_free(received);
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
    gpio_pin_set_dt(&green, 0);
    gpio_pin_set_dt(&blue, 0);
	printk("Led initialized ok\n");
	
	return 0;
}


