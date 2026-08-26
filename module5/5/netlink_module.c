#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/netlink.h>
#include <net/sock.h>

#define NETLINK_USER 31

static struct sock *nl_sk;

static void netlink_recv_msg(struct sk_buff *skb)
{
    struct nlmsghdr *nlh;
    char *msg;

    nlh = nlmsg_hdr(skb);
    msg = (char *)nlmsg_data(nlh);

    printk(KERN_INFO "netlink_module: received: %s\n", msg);

    {
        struct sk_buff *skb_out;
        struct nlmsghdr *nlh_out;
        char response[] = "Hello from kernel! Message received.";
        int pid = nlh->nlmsg_pid;
        int res;

        skb_out = nlmsg_new(strlen(response) + 1, GFP_KERNEL);

        if (!skb_out)
            return;

        nlh_out = nlmsg_put(skb_out, 0, 0, NLMSG_DONE, strlen(response) + 1, 0);

        if (!nlh_out) {
            kfree_skb(skb_out);
            return;
        }

        strcpy(nlmsg_data(nlh_out), response);

        res = nlmsg_unicast(nl_sk, skb_out, pid);

        if (res < 0)
            printk(KERN_ERR "netlink_module: failed to send message\n");
    }
}

static int __init netlink_init(void)
{
    struct netlink_kernel_cfg cfg = {
        .input = netlink_recv_msg,
    };

    nl_sk = netlink_kernel_create(&init_net, NETLINK_USER, &cfg);

    if (!nl_sk) {
        printk(KERN_ERR "netlink_module: failed to create socket\n");
        return -ENOMEM;
    }

    printk(KERN_INFO "netlink_module: loaded\n");

    return 0;
}

static void __exit netlink_exit(void)
{
    netlink_kernel_release(nl_sk);

    printk(KERN_INFO "netlink_module: unloaded\n");
}

module_init(netlink_init);
module_exit(netlink_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Student");
MODULE_DESCRIPTION("Netlink kernel module");
