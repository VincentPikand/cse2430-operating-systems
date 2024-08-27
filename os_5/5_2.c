#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/device.h>
#include <linux/fs.h>
#include <asm/uaccess.h>

int init_module(void);
void cleanup_module(void);
static int device_open(struct inode *, struct file *);
static int device_release(struct inode *, struct file *);
static ssize_t device_write(struct file *, const char *, size_t, loff_t 
*);

#define SUCCESS 0
#define DEVICE_NAME "klog"	/* Dev name as it appears in /proc/devices   
*/
#define BUF_LEN 80		/* Max length of the message from the 
device */

/* 
 * Global variables are declared as static, so are global within the file. 
 */

static int Major;		/* Major number assigned to our device 
driver */
static int Device_Open = 0;	/* Is device open?  
				 * Used to prevent multiple access to 
device */
static struct class *myClass;

static struct file_operations fops = {
	.write = device_write,
	.open = device_open,
	.release = device_release
};

/*
 * This function is called when the module is loaded
 */
int init_module(void)
{
    Major = register_chrdev(0, DEVICE_NAME, &fops);

	if (Major < 0) {
	  printk(KERN_ALERT "Registering char device failed with %d\n", 
Major);
	  return Major;
	}

	myClass = class_create(THIS_MODULE, DEVICE_NAME);
	device_create(myClass, NULL, MKDEV(Major, 0), NULL, DEVICE_NAME);

	return SUCCESS;
}

/*
 * This function is called when the module is unloaded
 */
void cleanup_module(void)
{
	/* 
	 * Unregister the device 
	 */
	device_destroy(myClass, MKDEV(Major, 0));
	class_destroy(myClass);

	unregister_chrdev(Major, DEVICE_NAME);
}

/*
 * Methods
 */

/* 
 * Called when a process tries to open the device file, like
 * "cat /dev/mycharfile"
 */
static int device_open(struct inode *inode, struct file *file)
{

	if (Device_Open)
		return -EBUSY;

	Device_Open++;
	try_module_get(THIS_MODULE);

	return SUCCESS;
}

/* 
 * Called when a process closes the device file.
 */
static int device_release(struct inode *inode, struct file *file)
{
	Device_Open--;		/* We're now ready for our next caller */

	/* 
	 * Decrement the usage count, or else once you opened the file, 
you'll
	 * never get get rid of the module. 
	 */
	module_put(THIS_MODULE);

	return 0;
}

/*  
 * Called when a process writes to dev file: echo "hi" > /dev/hello 
 */
static ssize_t
device_write(struct file *filp, const char *buff, size_t len, loff_t * 
off) {
	char temp_buff[BUF_LEN + 1];

    if (len > BUF_LEN || len <= 0) {
        printk(KERN_ALERT "Input too long");
        return -EINVAL;
    }

    if (copy_from_user(temp_buff, buff, len)) {
        return -EFAULT;
    }
    temp_buff[len] = '\0';

    // print to kernel log the input
    printk(KERN_INFO "%s\n", temp_buff);
    return len;
}

MODULE_LICENSE("GPL");
