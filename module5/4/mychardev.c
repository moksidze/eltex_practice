#include <linux/module.h>
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/uaccess.h>
#include <linux/mutex.h>

#define DEVICE_NAME "mychardev"
#define CLASS_NAME "mychar"
#define BUFFER_SIZE 1024

static dev_t device_number;
static struct cdev my_cdev;
static struct class *my_class;
static struct device *my_device;

static char kernel_buffer[BUFFER_SIZE];
static size_t data_size;

static DEFINE_MUTEX(buffer_mutex);

static int my_open(struct inode *inode, struct file *file)
{
    pr_info("mychardev: device opened\n");

    return 0;
}

static int my_release(struct inode *inode, struct file *file)
{
    pr_info("mychardev: device closed\n");

    return 0;
}

static ssize_t my_write(struct file *file,
    const char __user *user_buffer,
    size_t count,
    loff_t *offset)
{
    size_t bytes_to_copy;

    if (count == 0)
        return 0;

    bytes_to_copy = min(count, (size_t)(BUFFER_SIZE - 1));

    mutex_lock(&buffer_mutex);

    if (copy_from_user(kernel_buffer, user_buffer, bytes_to_copy)) {
        mutex_unlock(&buffer_mutex);
        return -EFAULT;
    }

    kernel_buffer[bytes_to_copy] = '\0';
    data_size = bytes_to_copy;

    mutex_unlock(&buffer_mutex);

    pr_info("mychardev: received %zu bytes from userspace\n", bytes_to_copy);

    return bytes_to_copy;
}

static ssize_t my_read(struct file *file,
    char __user *user_buffer,
    size_t count,
    loff_t *offset)
{
    size_t bytes_to_copy;

    mutex_lock(&buffer_mutex);

    if (*offset >= data_size) {
        mutex_unlock(&buffer_mutex);
        return 0;
    }

    bytes_to_copy = min(count, data_size - (size_t)*offset);

    if (copy_to_user(user_buffer, kernel_buffer + *offset, bytes_to_copy)) {
        mutex_unlock(&buffer_mutex);
        return -EFAULT;
    }

    *offset += bytes_to_copy;

    mutex_unlock(&buffer_mutex);

    pr_info("mychardev: sent %zu bytes to userspace\n", bytes_to_copy);

    return bytes_to_copy;
}

static const struct file_operations my_fops = {
    .owner = THIS_MODULE,
    .open = my_open,
    .read = my_read,
    .write = my_write,
    .release = my_release
};

static int __init mychardev_init(void)
{
    int ret;

    ret = alloc_chrdev_region(&device_number, 0, 1, DEVICE_NAME);

    if (ret < 0) {
        pr_err("mychardev: alloc_chrdev_region failed\n");
        return ret;
    }

    pr_info("mychardev: major=%d minor=%d\n",
        MAJOR(device_number),
        MINOR(device_number));

    cdev_init(&my_cdev, &my_fops);
    my_cdev.owner = THIS_MODULE;

    ret = cdev_add(&my_cdev, device_number, 1);

    if (ret < 0) {
        pr_err("mychardev: cdev_add failed\n");
        unregister_chrdev_region(device_number, 1);
        return ret;
    }

    my_class = class_create(CLASS_NAME);

    if (IS_ERR(my_class)) {
        pr_err("mychardev: class_create failed\n");
        cdev_del(&my_cdev);
        unregister_chrdev_region(device_number, 1);
        return PTR_ERR(my_class);
    }

    my_device = device_create(my_class,
        NULL,
        device_number,
        NULL,
        DEVICE_NAME);

    if (IS_ERR(my_device)) {
        pr_err("mychardev: device_create failed\n");
        class_destroy(my_class);
        cdev_del(&my_cdev);
        unregister_chrdev_region(device_number, 1);
        return PTR_ERR(my_device);
    }

    mutex_init(&buffer_mutex);

    pr_info("mychardev: module loaded\n");

    return 0;
}

static void __exit mychardev_exit(void)
{
    device_destroy(my_class, device_number);
    class_destroy(my_class);
    cdev_del(&my_cdev);
    unregister_chrdev_region(device_number, 1);

    pr_info("mychardev: module unloaded\n");
}

module_init(mychardev_init);
module_exit(mychardev_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Student");
MODULE_DESCRIPTION("Simple character device driver");
MODULE_VERSION("1.0");
