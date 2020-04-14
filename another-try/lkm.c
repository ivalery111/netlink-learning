#include <linux/module.h>
#include <linux/netlink.h>
#include <linux/string.h>

static int __init netlink_init(void)
{
    printk(KERN_INFO "Hello from netlink_init!\n");

    return 0;
}

static void __exit netlink_exit(void)
{
    printk(KERN_INFO "Goodbye from netlink_exit!\n");
}

module_init(netlink_init);
module_exit(netlink_exit);