// *******************************
//  Hox opettajalle 
/*
                Päivitetty 22/09/2025
        
        Tässä on tarvittavat Debugaukset tehty
        1 pisteen suoritus.
        Lisäsin timingit jokaiseen ledi lohkoon ja 
        kun kommentit oli käytössä keski arvo oli noin 20ms koko koodille ja välillä hyppi 
        jopa 28 ms. kun kommentit poistettiin koodilla kesti noin 15.6 ms enään
        merkittävä parannus.
        jos koodi haluttaisiin nopeammaksi niin pitäisi tehdä tästä yksinkertaisempi.


        

*/ 
#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/sys/util.h>
#include <zephyr/sys/printk.h>
#include <inttypes.h>
#include <zephyr/timing/timing.h>

// conf ledeille
static const struct gpio_dt_spec red = GPIO_DT_SPEC_GET(DT_ALIAS(led0), gpios);
static const struct gpio_dt_spec green = GPIO_DT_SPEC_GET(DT_ALIAS(led1), gpios);
static const struct gpio_dt_spec blue = GPIO_DT_SPEC_GET(DT_ALIAS(led2), gpios);
// conf buttoneille
// mielestäni tähän on pakosta parempi tapa saatan muokata mutta mennään tällä vielä ku toimii:DD

#define BUTTON_0 DT_ALIAS(sw0) // b1
#define BUTTON_1 DT_ALIAS(sw1)
#define BUTTON_2 DT_ALIAS(sw2)
#define BUTTON_3 DT_ALIAS(sw3)
#define BUTTON_4 DT_ALIAS(sw4)

static const struct gpio_dt_spec button_0 = GPIO_DT_SPEC_GET_OR(BUTTON_0, gpios, {0});
static const struct gpio_dt_spec button_1 = GPIO_DT_SPEC_GET_OR(BUTTON_1, gpios, {0});
static const struct gpio_dt_spec button_2 = GPIO_DT_SPEC_GET_OR(BUTTON_2, gpios, {0});
static const struct gpio_dt_spec button_3 = GPIO_DT_SPEC_GET_OR(BUTTON_3, gpios, {0});
static const struct gpio_dt_spec button_4 = GPIO_DT_SPEC_GET_OR(BUTTON_4, gpios, {0});

// tämän olisi voinut tehdä vain 1 static komenolla mutta tällä hetkellä en kerennyt vielä
static struct gpio_callback button_0_data;
static struct gpio_callback button_1_data;
static struct gpio_callback button_2_data;
static struct gpio_callback button_3_data;
static struct gpio_callback button_4_data;



// Task declarations ja koko
void red_task(void *, void *, void*);
void yellow_task(void *, void *, void*);
void green_task(void *, void *, void*);
#define	STACKSIZE	500 // koko
#define	PRIORITY	5       // pienempi on == tärkeämpi

// threadin määrittely tänne
K_THREAD_DEFINE(tid1,STACKSIZE,red_task,NULL,NULL,NULL,PRIORITY,0,0);
K_THREAD_DEFINE(tid2,STACKSIZE,yellow_task,NULL,NULL,NULL,PRIORITY,0,0);
K_THREAD_DEFINE(tid3,STACKSIZE,green_task,NULL,NULL,NULL,PRIORITY,0,0);
// globaalit muuttujat
int state= 0;
int timer = 0;
int last_state = 0;
int force_led = 0; // pakotetaan ledi eri väriin
int inter_stat= 0;
// button interrupti

// debuhausta varten
uint32_t red_time;
uint32_t yellow_time;
uint32_t green_time;
uint32_t init_time;



void button_0_handler(const struct device *dev, struct gpio_callback *cb, uint32_t pins) // hoitaa pause toiminnon
{       
         force_led = 0;
         inter_stat= 0;
//	printk("Button pressed\n");
        if (state == 4 && last_state == 4){
                state = 1;
                timer = 1;
         } else if (state == 4) {
                state = last_state;
        } else {
               last_state = state ;
               state = 4;
        }
  //      printk("tilanne %d\n",state);
}       
// hoitaa punaisen ledin 
void button_1_handler(const struct device *dev, struct gpio_callback *cb, uint32_t pins)
{
        state = 4;
//	printk("punainen interr\n");
        if (force_led == 1 && inter_stat == 1) {
  //      printk("punainen force\n");
         gpio_pin_set_dt(&blue,0);
         gpio_pin_set_dt(&red,0);
         gpio_pin_set_dt(&green,0);
        inter_stat = 0; 
        }
        else {
        
        force_led=1;
        inter_stat = 1;
        timer = 1;
        
        }
}

void button_2_handler(const struct device *dev, struct gpio_callback *cb, uint32_t pins)
{        
        state = 4;
//	printk("keltainen\n");
        if (force_led == 2 && inter_stat == 1) {
 //       printk("keltainen force \n");      
         gpio_pin_set_dt(&blue,0);
         gpio_pin_set_dt(&red,0);
         gpio_pin_set_dt(&green,0);
           inter_stat = 0;  
        }
        else {
        force_led=2;
        inter_stat = 1;
        timer = 1;
        }

}
void button_3_handler(const struct device *dev, struct gpio_callback *cb, uint32_t pins)
{
        state = 4;
//	printk("vihreä\n");
        if (force_led == 3 && inter_stat == 1) {
//	printk("vihreä force \n");
         gpio_pin_set_dt(&blue,0);
         gpio_pin_set_dt(&red,0);
         gpio_pin_set_dt(&green,0);
        inter_stat = 0;   
        }
        else {
  //       printk("testi \n");       
        force_led=3;
        inter_stat = 1;
        timer = 1;
        }
}
void button_4_handler(const struct device *dev, struct gpio_callback *cb, uint32_t pins)
{
        state = 4;
//	printk(" on off pressed\n");
        if (force_led == 4 && inter_stat == 1) {
         gpio_pin_set_dt(&blue,0);
         gpio_pin_set_dt(&red,0);
         gpio_pin_set_dt(&green,0);
          inter_stat = 0;                  
        }
        else {
 //       printk("");        
        force_led=4;
        inter_stat = 1;
        }
}

// Main program
int main(void) {
                // debug
                  timing_init();
                 timing_start();
	        timing_t start_time = timing_counter_get();
        init_led();
        int ret = init_button();
	if (ret < 0) {
		return 0;
	}
        // debug
        	timing_t end_time = timing_counter_get();
	        timing_stop();
                 uint64_t timing_ns = timing_cycles_to_ns(timing_cycles_get(&start_time, &end_time));
                  init_time = timing_ns / 1000;
	        printk("Init Times is:  %lld\n", timing_ns / 1000);
	while (true) {
                if (state == 0){
                
                state = 1;
                timer = 1;
                printk("%d\n",state);

                }
                else if (state != 4 ){
                timer = 1;
     //           printk("Ajastin toimii state on %d timer on %d force %d inter %d \n",state,timer,force_led,inter_stat);
                }
                else if (force_led == 4 && inter_stat == 1){
                        timer = 1;
                }
	
	k_msleep(1000);
	// k_yield();
	}
	
	return 0;
}

// Task1 function
void red_task(void *, void *, void*) {

	while (true) {
                timing_start();
		timing_t red_start_time = timing_counter_get();
                if (state == 1 && timer == 1) {
                gpio_pin_set_dt(&blue,0);
                gpio_pin_set_dt(&green,0);
                gpio_pin_set_dt(&red,1);
  //              printk("Punainen valo\n");
               // gpio_pin_set_dt(&red,0);
		state = 2;
                timer = 0;
                }
                else if (force_led ==1 && timer == 1){
                gpio_pin_set_dt(&blue,0);
                gpio_pin_set_dt(&green,0);
                gpio_pin_set_dt(&red,1);

                timer = 0;
                }
                k_msleep(100);
                timing_t red_end_time = timing_counter_get();
		timing_stop();
                 uint64_t timing_ns = timing_cycles_to_ns(timing_cycles_get(&red_start_time, &red_end_time));
		printk("Red task time: %lld\n", timing_ns / 1000);
                 red_time = timing_ns / 1000;
		// k_yield();
	}
}

// Task2 function
void yellow_task(void *, void *, void*) {
              
	while (true) {
                timing_start();
		timing_t yellow_start_time = timing_counter_get();
                if (timer == 1  && state == 2){
                gpio_pin_set_dt(&red,1);
                gpio_pin_set_dt(&green,1);
                gpio_pin_set_dt(&blue,0);
//		printk("Keltainen valo\n");
                state = 3;
                timer = 0;
                }
                else if (timer == 1 && force_led==2){
                gpio_pin_set_dt(&red,1);
                gpio_pin_set_dt(&green,1);
                gpio_pin_set_dt(&blue,0);
//		printk("Keltainen valo\n");
                timer = 0;
                }
                else if (timer == 1 && force_led==4 ){
                 gpio_pin_toggle_dt(&red);
                 gpio_pin_toggle_dt(&green);
                 gpio_pin_set_dt(&blue,0);
                 timer = 0;       
                }
		k_msleep(100);
                timing_t yellow_end_time = timing_counter_get();
		timing_stop();
    	        uint64_t timing_ns = timing_cycles_to_ns(timing_cycles_get(&yellow_start_time, &yellow_end_time));
		printk("Yellow task time: %lld\n", timing_ns / 1000);
                 yellow_time = timing_ns / 1000;
		 k_yield();
	}
}

void green_task(void *, void *, void*) {

        while (true) {
                timing_start();
		timing_t green_start_time = timing_counter_get();
                if (state == 3 && timer == 1){
                 gpio_pin_set_dt(&blue,0);
                 gpio_pin_set_dt(&red,0);
                 gpio_pin_set_dt(&green,1);
     //           printk("Vihrea valo\n");
                state = 1;
                timer = 0;
                }
                else if (force_led ==3 && timer == 1){
                 gpio_pin_set_dt(&blue,0);
                 gpio_pin_set_dt(&red,0);
                 gpio_pin_set_dt(&green,1);
                 timer = 0;
                }
                k_msleep(100);
                timing_t green_end_time = timing_counter_get();
		timing_stop();
                uint64_t timing_ns = timing_cycles_to_ns(timing_cycles_get(&green_start_time, &green_end_time));
		printk("Green task time: %lld\n", timing_ns / 1000);
                 green_time = timing_ns / 1000;
                 uint32_t total_cycle = red_time + yellow_time + green_time + init_time;
                printk("Full time: %u us\n", total_cycle);
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

//	printk("Led initialized ok\n");
	
	return 0;
}

int init_button() {

	int ret;
	if (!gpio_is_ready_dt(&button_0)) {
		printk("Error: button 0 is not ready\n");
		return -1;
	}

	ret = gpio_pin_configure_dt(&button_0, GPIO_INPUT);
	if (ret != 0) {
		printk("Error: failed to configure pin\n");
		return -1;
	}

	ret = gpio_pin_interrupt_configure_dt(&button_0, GPIO_INT_EDGE_TO_ACTIVE);
	if (ret != 0) {
		printk("Error: failed to configure interrupt on pin\n");
		return -1;
	}
        // kommentti tämäkin voisi tehdä paremmin. tässä saattaa alkaa huomaamaan jonku teeman:D 
        // MUTTA SE TOIMII!!
        	if (!gpio_is_ready_dt(&button_1)) {
		printk("Error: button 1 is not ready\n");
		return -1;
	}

	ret = gpio_pin_configure_dt(&button_1, GPIO_INPUT);
	if (ret != 0) {
		printk("Error: failed to configure pin\n");
		return -1;
	}

	ret = gpio_pin_interrupt_configure_dt(&button_1, GPIO_INT_EDGE_TO_ACTIVE);
	if (ret != 0) {
		printk("Error: failed to configure interrupt on pin\n");
		return -1;
	}
        //
        	if (!gpio_is_ready_dt(&button_2)) {
		printk("Error: button 2 is not ready\n");
		return -1;
	}

	ret = gpio_pin_configure_dt(&button_2, GPIO_INPUT);
	if (ret != 0) {
		printk("Error: failed to configure pin\n");
		return -1;
	}

	ret = gpio_pin_interrupt_configure_dt(&button_2, GPIO_INT_EDGE_TO_ACTIVE);
	if (ret != 0) {
		printk("Error: failed to configure interrupt on pin\n");
		return -1;
	}
        //
        	if (!gpio_is_ready_dt(&button_3)) {
		printk("Error: button 3 is not ready\n");
		return -1;
	}

	ret = gpio_pin_configure_dt(&button_3, GPIO_INPUT);
	if (ret != 0) {
		printk("Error: failed to configure pin\n");
		return -1;
	}

	ret = gpio_pin_interrupt_configure_dt(&button_3, GPIO_INT_EDGE_TO_ACTIVE);
	if (ret != 0) {
		printk("Error: failed to configure interrupt on pin\n");
		return -1;
	}
        //
        	if (!gpio_is_ready_dt(&button_4)) {
		printk("Error: button 4 is not ready\n");
		return -1;
	}

	ret = gpio_pin_configure_dt(&button_4, GPIO_INPUT);
	if (ret != 0) {
		printk("Error: failed to configure pin\n");
		return -1;
	}

	ret = gpio_pin_interrupt_configure_dt(&button_4, GPIO_INT_EDGE_TO_ACTIVE);
	if (ret != 0) {
		printk("Error: failed to configure interrupt on pin\n");
		return -1;
	}
	gpio_init_callback(&button_0_data, button_0_handler, BIT(button_0.pin));
	gpio_add_callback(button_0.port, &button_0_data);
        	gpio_init_callback(&button_1_data, button_1_handler, BIT(button_1.pin));
	gpio_add_callback(button_1.port, &button_1_data);
        	gpio_init_callback(&button_2_data, button_2_handler, BIT(button_2.pin));
	gpio_add_callback(button_2.port, &button_2_data);
        	gpio_init_callback(&button_3_data, button_3_handler, BIT(button_3.pin));
	gpio_add_callback(button_3.port, &button_3_data);
        	gpio_init_callback(&button_4_data, button_4_handler, BIT(button_4.pin));
	gpio_add_callback(button_4.port, &button_4_data);
//	printk("Set up on buttons ok\n");
	
	return 0;
}