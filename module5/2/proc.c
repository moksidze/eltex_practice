#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/proc_fs.h>
#include <linux/uaccess.h>

#define PROC_NAME "proc"
#define BUFFER_SIZE 256

static struct proc_dir_entry *proc_entry;
static char message[BUFFER_SIZE] = "Hello from kernel!\n";

static ssize_t proc_read(struct file *file, char __user *user_buffer, size_t count, loff_t *position)
{
    size_t message_len;

    if (*position > 0)
        return 0;

    message_len = strlen(message);

    if (count < message_len)
        message_len = count;

    if (copy_to_user(user_buffer, message, message_len))
        return -EFAULT;

    *position += message_len;

    return message_len;
}

static ssize_t proc_write(struct file *file, const char __user *user_buffer, size_t count, loff_t *position)
{
    size_t bytes_to_copy;

    bytes_to_copy = count;

    if (bytes_to_copy >= BUFFER_SIZE)
        bytes_to_copy = BUFFER_SIZE - 1;

    if (copy_from_user(message, user_buffer, bytes_to_copy))
        return -EFAULT;

    message[bytes_to_copy] = '\0';

    pr_info("proc_module: received from userspace: %s", message);

    return count;
}

static const struct proc_ops proc_file_ops = {
    .proc_read = proc_read,
    .proc_write = proc_write,
};

static int __init proc_module_init(void)
{
    proc_entry = proc_create(PROC_NAME, 0666, NULL, &proc_file_ops);

    if (!proc_entry) {
        pr_err("proc_module: failed to create /proc/%s\n", PROC_NAME);
        return -ENOMEM;
    }

    pr_info("proc_module: loaded\n");
    pr_info("proc_module: /proc/%s created\n", PROC_NAME);

    return 0;
}

static void __exit proc_module_exit(void)
{
    proc_remove(proc_entry);

    pr_info("proc_module: unloaded\n");
}

module_init(proc_module_init);
module_exit(proc_module_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Student");
MODULE_DESCRIPTION("Kernel module for communication through proc");
