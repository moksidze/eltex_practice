#include <linux/module.h>
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/timer.h>
#include <linux/jiffies.h>
#include <linux/kobject.h>
#include <linux/sysfs.h>
#include <linux/tty.h>
#include <linux/vt.h>
#include <linux/vt_kern.h>
#include <linux/kd.h>
#include <linux/mutex.h>

#define BLINK_DELAY (HZ / 5)

#define LED_MASK 0x07

static struct timer_list blink_timer;
static struct tty_driver *keyboard_driver;

static struct kobject *kbleds_kobject;

static int led_mask;
static bool led_state;

static DEFINE_MUTEX(kbleds_mutex);

static void set_keyboard_leds(unsigned int value)
{
    if (!keyboard_driver)
        return;

    if (!vc_cons[fg_console].d)
        return;

    if (!vc_cons[fg_console].d->port.tty)
        return;

    keyboard_driver->ops->ioctl(vc_cons[fg_console].d->port.tty, KDSETLED, value);
}

static void blink_timer_function(struct timer_list *timer)
{
    unsigned int value;

    mutex_lock(&kbleds_mutex);

    if (led_mask == 0) 
    {
        set_keyboard_leds(0);
        led_state = false;
    } 
    else 
    {
        led_state = !led_state;

        if (led_state)
            value = led_mask;
        else
            value = 0;

        set_keyboard_leds(value);
    }

    mutex_unlock(&kbleds_mutex);

    mod_timer(&blink_timer, jiffies + BLINK_DELAY);
}

static ssize_t mask_show(struct kobject *kobj, struct kobj_attribute *attr, char *buf)
{
    int value;

    mutex_lock(&kbleds_mutex);
    value = led_mask;
    mutex_unlock(&kbleds_mutex);

    return sysfs_emit(buf, "%d\n", value);
}

static ssize_t mask_store(struct kobject *kobj, struct kobj_attribute *attr, const char *buf, size_t count)
{
    int value;

    if (kstrtoint(buf, 10, &value))
        return -EINVAL;

    if (value < 0 || value > LED_MASK)
        return -EINVAL;

    mutex_lock(&kbleds_mutex);

    led_mask = value;
    led_state = false;

    set_keyboard_leds(0);

    mutex_unlock(&kbleds_mutex);

    return count;
}


static struct kobj_attribute mask_attribute = __ATTR(mask, 0660, mask_show, mask_store);

static int __init kbleds_init(void)
{
    int ret;

    printk(KERN_INFO "kbleds: loading\n");

    if (!vc_cons[fg_console].d) 
    {
        printk(KERN_ERR "kbleds: no active console\n");
        return -ENODEV;
    }

    if (!vc_cons[fg_console].d->port.tty) 
    {
        printk(KERN_ERR "kbleds: no tty\n");
        return -ENODEV;
    }

    keyboard_driver = vc_cons[fg_console].d->port.tty->driver;

    if (!keyboard_driver) 
    {
        printk(KERN_ERR "kbleds: no keyboard driver\n");
        return -ENODEV;
    }

    kbleds_kobject = kobject_create_and_add("kbleds", kernel_kobj);

    if (!kbleds_kobject) 
    {
        printk(KERN_ERR "kbleds: cannot create kobject\n");
        return -ENOMEM;
    }

    ret = sysfs_create_file(kbleds_kobject, &mask_attribute.attr);

    if (ret) 
    {
        printk(KERN_ERR "kbleds: cannot create sysfs file\n");

        kobject_put(kbleds_kobject);

        return ret;
    }

    timer_setup(&blink_timer, blink_timer_function, 0);

    mod_timer(&blink_timer, jiffies + BLINK_DELAY);

    printk(KERN_INFO "kbleds: loaded successfully\n");
    printk(KERN_INFO "kbleds: sysfs = /sys/kernel/kbleds/mask\n");

    return 0;
}


static void __exit kbleds_exit(void)
{
    printk(KERN_INFO "kbleds: unloading\n");

    timer_delete_sync(&blink_timer);

    set_keyboard_leds(0);

    if (kbleds_kobject) 
    {
        sysfs_remove_file(kbleds_kobject, &mask_attribute.attr);

        kobject_put(kbleds_kobject);
    }

    printk(KERN_INFO "kbleds: unloaded\n");
}


module_init(kbleds_init);
module_exit(kbleds_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Student");
MODULE_DESCRIPTION("Keyboard LEDs blinking controlled through sysfs");
