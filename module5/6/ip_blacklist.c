#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/proc_fs.h>
#include <linux/seq_file.h>
#include <linux/uaccess.h>
#include <linux/list.h>
#include <linux/mutex.h>
#include <linux/netfilter.h>
#include <linux/netfilter_ipv4.h>
#include <linux/ip.h>
#include <linux/inet.h>
#include <linux/version.h>

#define PROC_NAME "ip_blacklist"
#define MAX_COMMAND 64

struct blacklist_entry {
    __be32 ip;
    struct list_head list;
};

static LIST_HEAD(blacklist);
static DEFINE_MUTEX(blacklist_mutex);
static struct proc_dir_entry *proc_entry;

static bool ip_is_blacklisted(__be32 ip)
{
    struct blacklist_entry *entry;

    mutex_lock(&blacklist_mutex);

    list_for_each_entry(entry,&blacklist,list) {
        if (entry->ip == ip) {
            mutex_unlock(&blacklist_mutex);
            return true;
        }
    }

    mutex_unlock(&blacklist_mutex);
    return false;
}

static int blacklist_add(__be32 ip)
{
    struct blacklist_entry *entry;

    mutex_lock(&blacklist_mutex);

    list_for_each_entry(entry,&blacklist,list) {
        if (entry->ip == ip) {
            mutex_unlock(&blacklist_mutex);
            return -EEXIST;
        }
    }

    entry = kmalloc(sizeof(*entry),GFP_KERNEL);
    if (!entry) {
        mutex_unlock(&blacklist_mutex);
        return -ENOMEM;
    }

    entry->ip = ip;
    list_add_tail(&entry->list,&blacklist);

    mutex_unlock(&blacklist_mutex);

    return 0;
}

static int blacklist_remove(__be32 ip)
{
    struct blacklist_entry *entry;
    struct blacklist_entry *tmp;

    mutex_lock(&blacklist_mutex);

    list_for_each_entry_safe(entry,tmp,&blacklist,list) {
        if (entry->ip == ip) {
            list_del(&entry->list);
            kfree(entry);
            mutex_unlock(&blacklist_mutex);
            return 0;
        }
    }

    mutex_unlock(&blacklist_mutex);

    return -ENOENT;
}

static int blacklist_show(struct seq_file *seq,void *v)
{
    struct blacklist_entry *entry;

    mutex_lock(&blacklist_mutex);

    list_for_each_entry(entry,&blacklist,list)
        seq_printf(seq,"%pI4\n",&entry->ip);

    mutex_unlock(&blacklist_mutex);

    return 0;
}

static int blacklist_open(struct inode *inode,struct file *file)
{
    return single_open(file,blacklist_show,NULL);
}

static ssize_t blacklist_write(struct file *file,const char __user *buffer,size_t count,loff_t *offset)
{
    char command[MAX_COMMAND];
    char ip_string[32];
    __be32 ip;
    int result;

    if (count >= MAX_COMMAND)
        return -EINVAL;

    if (copy_from_user(command,buffer,count))
        return -EFAULT;

    command[count] = '\0';

    if (sscanf(command,"add %31s",ip_string) == 1) {
        if (!in4_pton(ip_string,-1,(u8 *)&ip,'\0',NULL))
            return -EINVAL;

        result = blacklist_add(ip);

        if (result == -EEXIST)
            return -EEXIST;

        if (result < 0)
            return result;

        return count;
    }

    if (sscanf(command,"del %31s",ip_string) == 1) {
        if (!in4_pton(ip_string,-1,(u8 *)&ip,'\0',NULL))
            return -EINVAL;

        result = blacklist_remove(ip);

        if (result == -ENOENT)
            return -ENOENT;

        if (result < 0)
            return result;

        return count;
    }

    return -EINVAL;
}

static const struct proc_ops proc_fops = {
    .proc_open = blacklist_open,
    .proc_read = seq_read,
    .proc_lseek = seq_lseek,
    .proc_release = single_release,
    .proc_write = blacklist_write
};

static unsigned int netfilter_hook(void *priv,struct sk_buff *skb,const struct nf_hook_state *state)
{
    struct iphdr *ip_header;

    if (!skb)
        return NF_ACCEPT;

    ip_header = ip_hdr(skb);

    if (!ip_header)
        return NF_ACCEPT;

    if (ip_is_blacklisted(ip_header->daddr)) {
        printk(KERN_INFO "ip_blacklist: packet blocked to %pI4\n",&ip_header->daddr);
        return NF_DROP;
    }

    return NF_ACCEPT;
}

static struct nf_hook_ops netfilter_ops = {
    .hook = netfilter_hook,
    .pf = NFPROTO_IPV4,
    .hooknum = NF_INET_LOCAL_OUT,
    .priority = NF_IP_PRI_FIRST
};

static int __init ip_blacklist_init(void)
{
    int result;

    proc_entry = proc_create(PROC_NAME,0666,NULL,&proc_fops);

    if (!proc_entry) {
        printk(KERN_ERR "ip_blacklist: failed to create /proc/%s\n",PROC_NAME);
        return -ENOMEM;
    }

    result = nf_register_net_hook(&init_net,&netfilter_ops);

    if (result < 0) {
        proc_remove(proc_entry);
        printk(KERN_ERR "ip_blacklist: failed to register Netfilter hook\n");
        return result;
    }

    printk(KERN_INFO "ip_blacklist: module loaded\n");

    return 0;
}

static void __exit ip_blacklist_exit(void)
{
    struct blacklist_entry *entry;
    struct blacklist_entry *tmp;

    nf_unregister_net_hook(&init_net,&netfilter_ops);

    proc_remove(proc_entry);

    mutex_lock(&blacklist_mutex);

    list_for_each_entry_safe(entry,tmp,&blacklist,list) {
        list_del(&entry->list);
        kfree(entry);
    }

    mutex_unlock(&blacklist_mutex);

    printk(KERN_INFO "ip_blacklist: module unloaded\n");
}

module_init(ip_blacklist_init);
module_exit(ip_blacklist_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Student");
MODULE_DESCRIPTION("IPv4 outgoing packet blacklist using Netfilter");
