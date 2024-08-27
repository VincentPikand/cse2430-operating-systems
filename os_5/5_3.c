#include <linux/module.h>	/* Needed by all modules */
#include <linux/kernel.h>	/* Needed for KERN_INFO */
#include <linux/gpio.h>
#include <linux/hrtimer.h>
#include <linux/ktime.h>

#define LED1 4
#define LED2 17
#define LED3 27
#define LED4 22

static struct hrtimer timer;
static bool isLEDOn = false;

void setLED(int value) {
    gpio_set_value(LED1, value);
    gpio_set_value(LED2, value);
    gpio_set_value(LED3, value);
    gpio_set_value(LED4, value);
}

enum hrtimer_restart hrTimerCallback(struct hrtimer *timer_for_restart) {
        ktime_t interval;
        if(isLEDOn) {
            setLED(0);
            isLEDOn = false;
            interval = ktime_set(2, 0);
        } else {
            setLED(1);
            isLEDOn = true;
            interval = ktime_set(1, 0);
        }
        hrtimer_forward_now(timer_for_restart, interval);
        // restart, or return HRTIMER_NORESTART;
        return HRTIMER_RESTART;
}

int init_module(void) {
    gpio_request(LED1, "LED1");
    gpio_request(LED2, "LED2");
    gpio_request(LED3, "LED3");
    gpio_request(LED4, "LED4");
	printk(KERN_INFO "All pins requested\n");
    gpio_direction_output(LED1, 0);
    gpio_direction_output(LED2, 0);
    gpio_direction_output(LED3, 0);
    gpio_direction_output(LED4, 0);

    hrtimer_init(&timer, CLOCK_MONOTONIC, HRTIMER_MODE_REL);
    timer.function = &hrTimerCallback;

    ktime_t ktime = ktime_set(1, 0);
    hrtimer_start(&timer, ktime, HRTIMER_MODE_REL);
	return 0;
}

void cleanup_module(void)
{   
    hrtimer_cancel(&timer);

    gpio_direction_output(LED1, 0);
    gpio_direction_output(LED2, 0);
    gpio_direction_output(LED3, 0);
    gpio_direction_output(LED4, 0);

    gpio_free(LED1);
    gpio_free(LED2);
    gpio_free(LED3);
    gpio_free(LED4);
	printk(KERN_INFO "Goodbye world <3\n");
}

MODULE_LICENSE("GPL");
