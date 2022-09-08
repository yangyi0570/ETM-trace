#include <linux/module.h> // included for all kernel modules
#include <linux/kernel.h> // included for KERN_INFO
#include <linux/init.h>
#include <linux/slab.h> // kmalloc()
#include <linux/kdev_t.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <asm/io.h>
#include <linux/uaccess.h> // copy_to_user()

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Yang Yi");
MODULE_DESCRIPTION("mount a character device to store trace data");

int device_buffer_size = 0x10000; //0x10000 = 64KB


//static int init_module(void);
//static void cleanup_module(void);


/* parameters of character device which is trace data buffer */
dev_t dev = 0;
static struct class *dev_class;
static struct cdev cs_cdev;
void __iomem *paddr;
void __iomem* read_point;
void __iomem* write_point;
bool is_end = false;

/* character device operations */
static ssize_t cs_read(struct file *filp, char __user *buf, size_t len, loff_t *off);
static ssize_t cs_write(struct file *filp, const char __user *buf, size_t len, loff_t *off);
static struct file_operations fops =
{
    .owner = THIS_MODULE,
    .read = cs_read,
    .write = cs_write,
};

static ssize_t cs_read(struct file *filp, char __user *buf, size_t len, loff_t *off)
{
    if(read_point == write_point)    // no data to read, return 0
    {
        write_point = paddr;
        read_point = paddr;
        memset(paddr, 0, device_buffer_size);   // clean device buffer
        return 0;
    }

    int data_size = 4;
    uint32_t val = ioread32(read_point);

    printk(KERN_INFO "data:%x",val);
    read_point += data_size;
    if(copy_to_user(buf, &val, sizeof(val)))
        return -EFAULT;
    
    return data_size;
    // return 0 means no more read, the return value means the size of data read to user space.
}

// write 32bit data to cs_device, otherwise, the longer data will be discarded.
static ssize_t cs_write(struct file *filp, const char __user *buf, size_t len, loff_t *off)
{
    int data_size = 4;
    if(copy_from_user(write_point, buf, data_size)) // the size of data is 4
        return -EFAULT;
    write_point += data_size;

    return data_size;
}

/* create /dev/cs_device file, read trace data through dd instruction */
static int create_trace_buffer_device(void)
{
    /*Allocating Major number*/
    if ((alloc_chrdev_region(&dev, 0, 1, "cs_Dev")) < 0)
    {
        pr_err("Cannot allocate major number\n");
        return -1;
    }
    pr_info("Major = %d Minor = %d \n", MAJOR(dev), MINOR(dev));

    /*Creating cdev structure*/
    cdev_init(&cs_cdev, &fops);

    /*Adding character device to the system*/
    if ((cdev_add(&cs_cdev, dev, 1)) < 0)
    {
        pr_err("Cannot add the device to the system\n");
        goto r_class;
    }

    /*Creating struct class*/
    if ((dev_class = class_create(THIS_MODULE, "cs_class")) == NULL)
    {
        pr_err("Cannot create the struct class\n");
        goto r_class;
    }

    /*Creating device*/
    if ((device_create(dev_class, NULL, dev, NULL, "cs_device")) == NULL)
    {
        pr_err("Cannot create the Device 1\n");
        goto r_device;
    }
    pr_info("Device Driver Insert...Done!!!\n");
    return 0;

r_device:
    class_destroy(dev_class);
r_class:
    unregister_chrdev_region(dev, 1);
    return -1;
}

static int __init mount_cs_device_init(void)
{
    /* mount character device */
    int retval = create_trace_buffer_device();
    if (retval)
        return retval;

    paddr = kmalloc(device_buffer_size, GFP_KERNEL); // 64KB,kmalloc()最大可申请内存空间
    memset(paddr, 0, device_buffer_size);
    write_point = paddr;
    read_point = paddr;

    return 0;
}

static void __exit mount_cs_device_exit(void)
{
    kfree(paddr);
    /* unmount character device */
    device_destroy(dev_class, dev);
    class_destroy(dev_class);
    cdev_del(&cs_cdev);
    unregister_chrdev_region(dev, 1);
    pr_info("Device Driver Remove...Done!!!\n");
}

module_init(mount_cs_device_init);
module_exit(mount_cs_device_exit);
