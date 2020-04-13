#include <stdio.h>
#include <errno.h>
#include <linux/netlink.h>
#include <sys/socket.h> /* for AF_NETLINK */
#include <sys/uio.h>    /* for iovec */
#include <errno.h>
#include <unistd.h>
#include <stdlib.h>
#include <memory.h>
#include <stdint.h>
#include <pthread.h>

/**
 * Returns the number of sent bytes */
int send_nl_msg_to_kernel(int sock_fd,
                          char *msg,
                          uint32_t msg_size,
                          int nlmsg_type,
                          uint16_t flags);

int send_nl_msg_to_kernel(int sock_fd,
                          char *msg,
                          uint32_t msg_size,
                          int nlmsg_type,
                          uint16_t flags)
{
    /* 1 step: prepare nlmsghdr with payload */
    struct sockaddr_nl dest_addr;
    memset(&dest_addr, 0, sizeof(dest_addr));
    dest_addr.nl_family = AF_NETLINK;
    dest_addr.nl_pid = 0; /* 0 because a kernel is destination */

    struct nlmsghdr *nl_hdr = (struct nlmsghdr *)calloc(1, NLMSG_HDRLEN + NLMSG_SPACE(msg_size));
    nl_hdr->nlmsg_len = NLMSG_HDRLEN + NLMSG_SPACE(msg_size);
    nl_hdr->nlmsg_pid = getpid();
    nl_hdr->nlmsg_type = nlmsg_type;
    nl_hdr->nlmsg_seq = 0;
    strncpy(NLMSG_DATA(nl_hdr), msg, msg_size);

    /* 2 step: wrap the message into iovec */
    struct iovec iov;
    iov.iov_base = (void *)nl_hdr;
    iov.iov_len = nl_hdr->nlmsg_len;

    /* 3 step: wrap the iovec into struct msghdr */
    struct msghdr outer_msg_hdr;
    memset(&outer_msg_hdr, 0, sizeof(struct msghdr));
    outer_msg_hdr.msg_name = (void *)&dest_addr;
    outer_msg_hdr.msg_namelen = sizeof(dest_addr);
    outer_msg_hdr.msg_iov = &iov;
    outer_msg_hdr.msg_iovlen = 1;

    /* 4 step: send through sendmsg */
    int send_bytes = sendmsg(sock_fd, &outer_msg_hdr, 0);
    if (send_bytes < 0)
    {
        printf("Message sending error: %d\n", errno);
    }

    return send_bytes;
}