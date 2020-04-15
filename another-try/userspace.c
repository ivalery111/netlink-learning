#include <stdio.h>
#include <linux/netlink.h>
#include <sys/socket.h>
#include <sys/types.h> /* for getpid() */
#include <unistd.h>    /* for getpid() */
#include <stdlib.h>
#include <string.h>
#include "utils.h"

int main(void)
{
    /* create netlink socket */
    int socket_fd = socket(PF_NETLINK, SOCK_RAW, NETLINK_PROTO_TEST);
    if (socket_fd == -1)
    {
        perror("socket creation failed: ");
        exit(EXIT_FAILURE);
    }

    /* prepare data for sending */
    struct sockaddr_nl src_addr = {0};
    src_addr.nl_family = AF_NETLINK;
    src_addr.nl_pid = getpid();

    if (bind(socket_fd, (struct sockaddr *)&src_addr, sizeof(src_addr)) == -1)
    {
        perror("bind failed: ");
        exit(EXIT_FAILURE);
    }

    struct sockaddr_nl dst_addr = {0};
    dst_addr.nl_family = AF_NETLINK;
    dst_addr.nl_pid = 0; /* Always zero for kernel */

    /* 1 step: prepare nlmsghdr with payload */
    struct nlmsghdr *msg_hdr = calloc(1, NLMSG_HDRLEN + NLMSG_SPACE(MAX_PAYLOAD));
    msg_hdr->nlmsg_len = NLMSG_HDRLEN + NLMSG_SPACE(MAX_PAYLOAD);
    msg_hdr->nlmsg_pid = getpid();
    msg_hdr->nlmsg_type = NLMSG_HELLO;
    msg_hdr->nlmsg_seq = 0;
    msg_hdr->nlmsg_flags |= NLM_F_REQUEST; /* ? */
    strncpy(NLMSG_DATA(msg_hdr), "MSG: Hello!", 12);

    /* 2 step: wrap the message into iovec */
    struct iovec iov = {0};
    iov.iov_base = (void *)msg_hdr;
    iov.iov_len = msg_hdr->nlmsg_len;

    /* 3 step: wrap the iovec into struct msghdr */
    static struct msghdr outer_msg_hdr = {0};
    outer_msg_hdr.msg_name = (void *)&dst_addr;
    outer_msg_hdr.msg_namelen = sizeof(dst_addr); /* To whom sending a message? */
    outer_msg_hdr.msg_iov = &iov;
    outer_msg_hdr.msg_iovlen = 1;

    /* 4 step: sending */
    int sent_bytes = sendmsg(socket_fd, &outer_msg_hdr, 0);
    if (sent_bytes < 0)
    {
        perror("msg sending failed: ");
        exit(EXIT_FAILURE);
    }
    printf("sent %d bytes!\n", sent_bytes);
    return 0;
}