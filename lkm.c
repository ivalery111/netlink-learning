#include <linux/module.h>
#include <linux/netlink.h>
#include <linux/string.h>
#include <net/sock.h> /* for init_dev */
#include "utils.h"

static void nl_recv_msg_fn(struct sk_buff *skb);

static struct sock *nl_socket = NULL;
static struct netlink_kernel_cfg cfg = {.input = nl_recv_msg_fn};

static int __init netlink_init(void)
{
    printk(KERN_INFO "Hello from netlink_init!\n");

    nl_socket = netlink_kernel_create(&init_net,
                                      NETLINK_PROTO_TEST,
                                      &cfg);
    if (nl_socket == NULL)
    {
        printk(KERN_INFO "Failed to create kernel socket\n");
        return -ENOMEM;
    }
    printk(KERN_INFO "Kernel socket created!\n");

    return 0;
}

static void __exit netlink_exit(void)
{
    netlink_kernel_release(nl_socket);
    nl_socket = NULL;

    printk(KERN_INFO "Goodbye from netlink_exit!\n");
}

module_init(netlink_init);
module_exit(netlink_exit);

static void nl_recv_msg_fn(struct sk_buff *skb)
{
    struct nlmsghdr *recv_msg = NULL;

    printk(KERN_INFO "%s invoked.\n", __FUNCTION__);

    recv_msg = (struct nlmsghdr *)(skb->data);
    if (recv_msg == NULL)
    {
        printk(KERN_INFO "[%d]: Invalid recv_msg pointer!\n", __LINE__);
        return;
    }
    dump_msg(recv_msg);
}