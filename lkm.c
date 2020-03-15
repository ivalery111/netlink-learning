#include <linux/module.h>
#include <linux/netlink.h>
#include <net/sock.h>
#include <linux/string.h>
#include <stdio.h>

#define NETLINK_TEST_PROTOCOL 31

/**
 * @brief This function prints all field of struct nlmsghdr
 *
 * @return void - nothing
 */
static void nlmsg_dump(struct nlmsghdr *nlh)
{
    if(!nlh){
        printk(KENR_INFO "nlmsg_len  : %u", nlh->nlmsg_len);
        printk(KENR_INFO "nlmsg_type : %u", nlh->nlmsg_type);
        printk(KENR_INFO "nlmsg_flags: %u", nlh->nlmsg_flags);
        printk(KENR_INFO "nlmsg_seq  : %u", nlh->nlmsg_seq);
        printk(KENR_INFO "nlmsg_pid  : %u", nlh->nlmsg_pid);
    }
    else{
        printk(KERN_INFO "nlmsg_dump: can't print nlmsghdr's fields")
    }
}

/**
 * @brief With this socket I will send/receive the data from user-space.
 */
static struct sock *nl_sk = NULL;

/**
 * @brief This function is handle for user-space data.
 */
static void netlink_recv_msg_fn(struct sk_buff *skb)
{
    struct nlmsghdr *nlh_recv, *nlh_reply;
    int user_space_proc_port_id, user_space_data_len, res;
    char *user_space_data;
    char kernel_reply[256];  /* memory for kernel's reply to user-space */
    struct sk_buff *skb_out; /* this socket buffer will be send to user-space */

    printk(KERN_INFO "%s invoked", __FUNCTION__);

    nlh_recv = (struct nlmsghdr *)(skb->data);
    nlmsg_dump(nlh_recv);

    user_space_proc_port_id = nlh_recv->nlmsg_pid;
    printk(LERN_INFO "%s[%d]: port id of the sending user-space process = %u", __FUNCTION__, __LINE__, user_space_proc_port_id);

    user_space_data = (char *)nlmsg_data(nlh_recv);
    user_space_data_len = nlh_recv->nlmsg_len;
    printk(KERN_INFO "%s[%d]: msg recv from user-space = %s, skb->len = %d, nlh->nlmsg_len = %d",
           __FUNCTION__, __LINE__, user_space_data, user_space_data_len, nlh_recv->nlmsg_len);

    /* If user-space set NLM_F_ACK to 'ON' than the kernel send a message back to the user-space */
    if( nlh_recv->nlmsg_flags & NLM_F_ACK)
    {
        memset(kernel_reply, 0, sizeof(kernel_reply));
        snprintf(kernel_reply, sizeof(kernel_reply), "Msg from %d has been precessed by kernel", nlh_recv->nlmsg_pid);

        /* prepare sokcet buffer */
        skb_out = nlmsg_new(sizeof(kernel_reply), 0);

        /* add a netlink message to an skb */
        nlh_reply = nlmsg_put(skb_out,
                              0,                    /* kernel is sender -> port id = 0 */
                              nlh_recv->nlmsg_seq,
                              NLMSG_DONE,
                              sizeof(kernel_reply),
                              0);

        /* sending a message */
        res = nlmsg_unicast(nl_sk, skb_out, user_space_proc_port_id);
        if( res < 0)
        {
            printk(KERN_INFO "Can't send a data back to user-space");
            kfree(skb_out);
        }
    }
}

/**
 * @brief Defines what function will calls when user-space will send the data.
 */
static struct netlink_kernel_cfg cfg = {
    .input = netlink_recv_msg_fn;
};

/**
 * @brief This function invokes when module is loading with 'insmod' command.
 * 
 * Function creates the netlink socket.
 * 
 * @note nl_sk (netlink socket) should be the global variable.
 * 
 * @return 0 - success, -ENOMEM - socket creation failed
 */
static int __init netlink_greetings_init(void)
{
    printk(KERN_INFO "Hello Kernel!");

    nl_sk = netlink_kernel_create(&init_net,             /**< Global variable which provided by the kernel. This one is represent the networking subsystem. */
                                  NETLINK_TEST_PROTOCOL,
                                  &cfg);
    if (!nl_sk)
    {
        printk(KERN_INFO "Netlink Kernel Create failed to create the socket for %u protocol", NETLINK_TEST_PROTOCOL);
        return -ENOMEM;
    }
    printk(KERN_INFO "Netlink Socket Created Successfully!");

    return 0;
}

/**
 * @brief This function invokes when module is unloading with 'rmmod' command.
 * 
 * This function release all needed resources to avoid kernel resources leak.
 * 
 * @return void - nothing
 */
static void __exit netlink_greetings_exit(void)
{
    printk(KERN_INFO "Bye-Bye, Kernel!");

    netlink_kernel_release(nl_sk);
    nl_sk = NULL;
}

module_init(netlink_greetings_init);
module_exit(netlink_greetings_exit);

MODULE_LICENSE("GPL");