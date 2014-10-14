#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <errno.h>
#include <sys/types.h>

static struct sockaddr_in saddr;

int main(int argc, char *argv[]) {

  // ==[ Check usage ]==

  // if run as user, inject libsudoport and execute command-line
  if (geteuid()) {
    setenv("DYLD_INSERT_LIBRARIES", "libsudoport.dylib", 1);
    execvp(argv[1], argv + 1);
    _exit(EINVAL);
  }

  if (argc != 3) {
    fprintf(stderr, "usage: %s <ip> <port>\n", argv[0]);
    _exit(EINVAL);
  }

  // ==[ Parse ip/port ]==

  struct in_addr addr;
  unsigned short port;

  // parse ip
  if (!inet_aton(argv[1], &addr)) {
    fprintf(stderr, "%s: invalid IP address\n", argv[0]);
    _exit(EINVAL);
  }

  // parse port
  if ((sscanf(argv[2], "%hu", &port) != 1) || ((port < 1) || (port > 511))) {
    fprintf(stderr, "%s: invalid port\n", argv[0]);
    _exit(EINVAL);
  }

  // ==[ Bind privileged port ]==

  // create desired socket
  memset(&saddr, 0, sizeof(saddr));
  saddr.sin_family = AF_INET;
  saddr.sin_addr.s_addr = (in_addr_t) addr.s_addr;
  saddr.sin_port = htons(port);

  // bind fd 0
  if (bind(0, (const struct sockaddr *) &saddr, sizeof(saddr))) {
    fprintf(stderr, "%s: %s\n", argv[0], strerror(errno));
    _exit(EINVAL);
  }

  // success
  _exit(0);
}
