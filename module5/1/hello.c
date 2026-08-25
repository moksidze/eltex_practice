#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>

MODULE_LICENSE("MaxKrupin-License");
MODULE_AUTHOR("Max Krupin");
MODULE_DESCRIPTION("Hello World kernel module for Module 5 Task 1");
MODULE_VERSION("1.0");

static int __init hello_init(void)
{
    pr_info("Hello World!\n");

    return 0;
}

static void __exit hello_exit(void)
{
    pr_info("Goodbye World!\n");
}

module_init(hello_init);
module_exit(hello_exit);
