#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/device.h>
#include <linux/hrtimer.h>
#include <linux/gpio.h>
#include <linux/fs.h>
#include <linux/mutex.h>

#define LED1 4
#define LED2 17
#define LED3 27
#define LED4 22

#define SUCCESS 0
#define DEVICE_NAME "mypwm"	/* Dev name as it appears in /proc/devices   
*/
static ssize_t device_write(struct file *, const char *, size_t, loff_t 
*);

static int Major;		/* Major number assigned to our device 
driver */
static struct class *pwm_class;	/* Tie with the device model */
static struct hrtimer timer;
static struct mutex lock;

static bool isLEDOn = false;
static int onTime = 0;
static int offTime = 0;


static struct file_operations fops = {
	.write = device_write
};

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
            interval = ktime_set(0, offTime);
        } else {
            if(onTime != 0) setLED(1);
            isLEDOn = true;
            interval = ktime_set(0, onTime);
        }
        hrtimer_forward_now(timer_for_restart, interval);
        // restart, or return HRTIMER_NORESTART;
        return HRTIMER_RESTART;
}

static ssize_t
device_write(struct file *filp, const char *buff, size_t len, loff_t * 
off) {
    mutex_lock(&lock);
    // get int value in range 0-100 from buff
    int value;
    if (sscanf(buff, "%d", &value) != 1) {
        printk(KERN_ALERT "Invalid input\n");
        return -EINVAL;
    }
    printk(KERN_INFO "Received value: %d\n", value);
    onTime = (value * 10 * 1000000) / 100;
    offTime = ((100-value) * 10 * 1000000) / 100;

    ktime_t interval = ktime_set(0, onTime);
    hrtimer_start(&timer, interval, HRTIMER_MODE_REL);
    mutex_unlock(&lock);
    return len;
}

int init_module(void){
    Major = register_chrdev(0, DEVICE_NAME, &fops);

    if (Major < 0) {
        printk(KERN_ALERT "Registering char device failed with %d)", 
Major);
    }

    pwm_class = class_create(THIS_MODULE, DEVICE_NAME);
    device_create(pwm_class, NULL, MKDEV(Major, 0), NULL, DEVICE_NAME);

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
    timer.function = hrTimerCallback;

    mutex_init(&lock);

    return SUCCESS;
}

void cleanup_module(void){
    gpio_set_value(LED1, 0);
    gpio_set_value(LED2, 0);
    gpio_set_value(LED3, 0);
    gpio_set_value(LED4, 0);

    gpio_free(LED1);
    gpio_free(LED2);
    gpio_free(LED3);
    gpio_free(LED4);
    printk(KERN_INFO "All pins freed\n");
    
    hrtimer_cancel(&timer);
    

    device_destroy(pwm_class, MKDEV(Major, 0));
    class_unregister(pwm_class);
    class_destroy(pwm_class);
    unregister_chrdev(Major, DEVICE_NAME);

}

MODULE_LICENSE("GPL");
