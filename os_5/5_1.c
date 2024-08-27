#include <linux/module.h>	/* Needed by all modules */
#include <linux/kernel.h>	/* Needed for KERN_INFO */
#include <linux/gpio.h>

#define LED1 4
#define LED2 17
#define LED3 27
#define LED4 22

int init_module(void)
{
    gpio_request(LED1, "LED1");
    gpio_request(LED2, "LED2");
    gpio_request(LED3, "LED3");
    gpio_request(LED4, "LED4");


	printk(KERN_INFO "All pins requested\n");

    gpio_direction_output(LED1, 1);
    gpio_direction_output(LED2, 1);
    gpio_direction_output(LED3, 1);
    gpio_direction_output(LED4, 1);

    gpio_set_value(LED1, 1);
    gpio_set_value(LED2, 1);
    gpio_set_value(LED3, 1);
    gpio_set_value(LED4, 1);


	return 0;
}

void cleanup_module(void)
{
    gpio_set_value(LED1, 0);
    gpio_set_value(LED2, 0);
    gpio_set_value(LED3, 0);
    gpio_set_value(LED4, 0);
    
    gpio_free(LED1);
    gpio_free(LED2);
    gpio_free(LED3);
    gpio_free(LED4);
	printk(KERN_INFO "Goodbye world <3\n");
}

MODULE_LICENSE("GPL");
