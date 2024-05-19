#include <linux/atomic.h> 
#include <linux/cdev.h> 
#include <linux/delay.h> 
#include <linux/device.h> 
#include <linux/fs.h> 
#include <linux/init.h> 
#include <linux/kernel.h> 
#include <linux/module.h> 
#include <linux/printk.h> 
#include <linux/types.h> 
#include <linux/uaccess.h> 
 
#include <asm/errno.h> 

#include "my_deque.h"

struct my_deque glob;


static int device_open(struct inode *, struct file *); 
static int device_release(struct inode *, struct file *); 
static ssize_t device_read(struct file *, char __user *, size_t, loff_t *); 
static ssize_t device_write(struct file *, const char __user *, size_t, 
                            loff_t *); 
 
#define SUCCESS 0 
#define DEVICE_NAME "custom_fifo" 
 
 
static int major; 
 
enum { 
    CDEV_NOT_USED = 0, 
    CDEV_EXCLUSIVE_OPEN = 1, 
}; 
 
 
 
static struct class *cls; 
 
static struct file_operations chardev_fops = { 
    .read = device_read, 
    .write = device_write, 
    .open = device_open, 
    .release = device_release, 
}; 
 
static int __init custom_fifo_init(void) 
{ 
    deque_init(&glob);
    major = register_chrdev(0, DEVICE_NAME, &chardev_fops); 
 
    if (major < 0) { 
        pr_alert("Registering char device failed with %d\n", major); 
        return major; 
    } 
 
    pr_info("I was assigned major number %d.\n", major); 
 
    cls = class_create(THIS_MODULE, DEVICE_NAME); 
    device_create(cls, NULL, MKDEV(major, 0), NULL, DEVICE_NAME); 
 
    pr_info("Device created on /dev/%s\n", DEVICE_NAME); 
 
    return SUCCESS; 
} 
 
static void __exit custom_fifo_exit(void) 
{ 
    device_destroy(cls, MKDEV(major, 0)); 
    class_destroy(cls); 
 
    unregister_chrdev(major, DEVICE_NAME); 
} 
  

static int device_open(struct inode *inode, struct file *file) 
{ 
    try_module_get(THIS_MODULE); 
 
    return SUCCESS; 
} 
 
static int device_release(struct inode *inode, struct file *file) 
{ 
    module_put(THIS_MODULE); 
 
    return SUCCESS; 
} 
 

static ssize_t device_read(struct file *filp, 
                           char __user *buffer, 
                           size_t length,  
                           loff_t *offset) 
{ 
    s64 ans;
    printk("lenlim = %lu", length);
    ans = pop_front(&glob, buffer, length); 
    printk("ans = %lld", ans);
    if(ans == 0) {
        return -EAGAIN;
    } else {
        return ans;
    }
    
} 
 
static ssize_t device_write(struct file *filp, const char __user *buff, 
                            size_t len, loff_t *off) 
{ 
    printk("lenpr = %lu", len);
    return push_back(&glob, buff, len);
} 
 
module_init(custom_fifo_init); 
module_exit(custom_fifo_exit); 
 
MODULE_LICENSE("GPL");