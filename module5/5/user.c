#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <linux/netlink.h>

#define NETLINK_USER 31
#define MAX_PAYLOAD 1024

int main(void)
{
    int sock_fd;
    struct sockaddr_nl src_addr;
    struct sockaddr_nl dest_addr;
    struct nlmsghdr *nlh;
    struct iovec iov;
    struct msghdr msg;
    char message[] = "Hello from userspace!";
    char buffer[MAX_PAYLOAD];

    sock_fd = socket(AF_NETLINK, SOCK_RAW, NETLINK_USER);

    if (sock_fd < 0) {
        perror("socket");
        return 1;
    }

    memset(&src_addr, 0, sizeof(src_addr));
    src_addr.nl_family = AF_NETLINK;
    src_addr.nl_pid = getpid();

    if (bind(sock_fd, (struct sockaddr *)&src_addr, sizeof(src_addr)) < 0) {
        perror("bind");
        close(sock_fd);
        return 1;
    }

    memset(&dest_addr, 0, sizeof(dest_addr));
    dest_addr.nl_family = AF_NETLINK;
    dest_addr.nl_pid = 0;

    nlh = malloc(NLMSG_SPACE(MAX_PAYLOAD));

    if (!nlh) {
        close(sock_fd);
        return 1;
    }

    memset(nlh, 0, NLMSG_SPACE(MAX_PAYLOAD));

    nlh->nlmsg_len = NLMSG_SPACE(strlen(message) + 1);
    nlh->nlmsg_pid = getpid();

    strcpy(NLMSG_DATA(nlh), message);

    iov.iov_base = nlh;
    iov.iov_len = nlh->nlmsg_len;

    memset(&msg, 0, sizeof(msg));
    msg.msg_name = &dest_addr;
    msg.msg_namelen = sizeof(dest_addr);
    msg.msg_iov = &iov;
    msg.msg_iovlen = 1;

    if (sendmsg(sock_fd, &msg, 0) < 0) {
        perror("sendmsg");
        free(nlh);
        close(sock_fd);
        return 1;
    }

    memset(buffer, 0, sizeof(buffer));
    iov.iov_base = buffer;
    iov.iov_len = sizeof(buffer);

    if (recvmsg(sock_fd, &msg, 0) < 0) {
        perror("recvmsg");
        free(nlh);
        close(sock_fd);
        return 1;
    }

    printf("Kernel response: %s\n", NLMSG_DATA((struct nlmsghdr *)buffer));

    free(nlh);
    close(sock_fd);

    return 0;
}
