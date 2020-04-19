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
    struct nlmsghdr *recv_msg = NULL, *reply_msg = NULL;
    int us_port_id = 0, us_data_len = 0, err = -1;
    char *us_data = NULL;
    char kernel_reply[KERN_REPLY_MSG_LEN];
    struct sk_buff *skb_out = NULL;

    printk(KERN_INFO "%s invoked.\n", __FUNCTION__);

    recv_msg = (struct nlmsghdr *)(skb->data);
    if (recv_msg == NULL)
    {
        printk(KERN_INFO "[%d]: Invalid recv_msg pointer!\n", __LINE__);
        return;
    }
    dump_msg(recv_msg);

    /* prepare reply to the userspace */
    us_port_id = recv_msg->nlmsg_pid;
    printk(KERN_INFO "us_port_id: %d\n", us_port_id);

    us_data = (char *)nlmsg_data(recv_msg);
    printk(KERN_INFO "us_data: %s\n", us_data);

    us_data_len = skb->len;
    printk(KERN_INFO "us_data_len: %d\n", us_data_len);

    /* checking if userspace request a reply? */
    if (recv_msg->nlmsg_flags && NLM_F_REQUEST)
    {
        printk(KERN_INFO "userspace request a reply!\n");
        memset(kernel_reply, 0, KERN_REPLY_MSG_LEN);

        snprintf(kernel_reply, KERN_REPLY_MSG_LEN, "Kernel got the msg from pid: %d\n", recv_msg->nlmsg_pid);

        skb_out = nlmsg_new(KERN_REPLY_MSG_LEN, 0);
        if (skb_out == NULL)
        {
            printk("[%d]: Failed to create a new netlink message!\n", __LINE__);
            return;
        }

        /* trying to allocate memory for a new netlink message */
        reply_msg = nlmsg_put(skb_out,
                              0,                   /* 0 - because kernel is a sender */
                              recv_msg->nlmsg_seq, /* be carefull here; put sequence number of a received msg! */
                              NLMSG_DONE,
                              KERN_REPLY_MSG_LEN,
                              0);
        if (reply_msg == NULL)
        {
            printk(KERN_INFO "[%d]: Failed to add a netlink message to an skb!\n", __LINE__);
            return;
        }

        /* copy a payload */
        strncpy(nlmsg_data(reply_msg), kernel_reply, KERN_REPLY_MSG_LEN);

        /* send a message to the userspace*/
        err = nlmsg_unicast(nl_socket, skb_out, us_port_id);
        if (err < 0)
        {
            printk(KERN_INFO "[%d]: Failed to send unicast!\n", __LINE__);
            return;
        }

        printk(KERN_INFO "Kernel sent a message to the userspace!\n");
    }
    else
    {
        printk(KERN_INFO "[%d]: userspace doesn't request a reply!\n", __LINE__);
    }
}