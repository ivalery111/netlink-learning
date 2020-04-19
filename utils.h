#ifndef _UTILS_H
#define _UTILS_H

/* Some helps things/defines for kernel/user space */

#ifndef __KERNEL__
#include <stdio.h>
#include <memory.h>
#endif

#define NETLINK_PROTO_TEST 24
#define MAX_PAYLOAD 1024
#define KERN_REPLY_MSG_LEN 256

/* user defined NLMSG type, should be > 16 */
#define NLMSG_HELLO 31

static inline char *nl_get_msg_type(__u16 nlmsg_type)
{
    switch (nlmsg_type)
    {
    case NLMSG_NOOP:
        return "NLMSG_NOOP";
        break;
    case NLMSG_ERROR:
        return "NLMSG_ERROR";
        break;
    case NLMSG_DONE:
        return "NLMSG_DONE";
        break;
    case NLMSG_OVERRUN:
        return "NLMSG_OVERRUN";
        break;
    case NLMSG_HELLO:
        return "NLMSG_HELLO";
        break;

    default:
        return "Unknown Message Type";
        break;
    }
}

static inline void dump_msg(struct nlmsghdr *msg)
{
#ifdef __KERNEL__
    printk(KERN_INFO "\nDump message:\n");
#else
    printf("\nDump message:\n");
#endif

#ifdef __KERNEL__
    printk(KERN_INFO "\tMsg Type : %s\n", nl_get_msg_type(msg->nlmsg_type));
#else
    printf("\tMsg Type : %s\n", nl_get_msg_type(msg->nlmsg_type));
#endif

    if (msg->nlmsg_type == NLMSG_ERROR)
    {
#ifdef __KERNEL__
        printk(KERN_INFO "\tError Code: %d\n", *(int *)(msg + 1));
#else
        printf("\tError Code: %d\n", *(int *)(msg + 1));
#endif
    }

#ifdef __KERNEL__
    printk(KERN_INFO "\tMsg Len  : %d\n", msg->nlmsg_len);
#else
    printf("\tMsg Len  : %d\n", msg->nlmsg_len);
#endif

#ifdef __KERNEL__
    printk(KERN_INFO "\tMsg Flags: %d\n", msg->nlmsg_flags);
#else
    printf("\tMsg Flags: %d\n", msg->nlmsg_flags);
#endif

#ifdef __KERNEL__
    printk(KERN_INFO "\tMsg Seq  : %d\n", msg->nlmsg_seq);
#else
    printf("\tMsg Seq  : %d\n", msg->nlmsg_seq);
#endif

#ifdef __KERNEL__
    printk(KERN_INFO "\tMsg Pid  : %d\n", msg->nlmsg_pid);
#else
    printf("\tMsg Pid  : %d\n", msg->nlmsg_pid);
#endif
}
#endif