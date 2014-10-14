#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <signal.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <errno.h>
#include <sys/wait.h>

#define DYLD_INTERPOSE(_replacer,_replacee) \
  __attribute__ ((used)) static struct { const void* replacer; const void* replacee; } _interpose_##_replacee \
  __attribute__ ((section ("__DATA,__interpose"))) = { (const void*)(unsigned long)&_replacer, (const void*)(unsigned long)&_replacee }

int my_bind(int socket, const struct sockaddr *addr, socklen_t addr_len) {

  // ==[ Verify access ]==

  // ensure non-root user binding TCP/IP port 1-511
  if (!getuid()
    ||addr_len        != sizeof(struct sockaddr_in)
    ||addr->sa_family != AF_INET
    ||      ((struct sockaddr_in *) addr)->sin_port  == 0
    ||ntohs(((struct sockaddr_in *) addr)->sin_port) >= 512
  ) return bind(socket, addr, addr_len);

  // ==[ Stringify ip/port ]==

  char *ip_str, port_str[6];

  ip_str = inet_ntoa(((struct sockaddr_in *) addr)->sin_addr);
  sprintf(port_str, "%hu", ntohs(((struct sockaddr_in *) addr)->sin_port));

  // ==[ Invoke sudoport ]==

  pid_t child;
  int status, result = -1;

  // fork a child
  if ((child = fork()) == -1) {
    fprintf(stderr, "unable to fork child\n");
    errno = EPERM; // fix this
    return -1;
  }

  // invoke sudoport (from child)
  if (child == 0) {

    // duplicate socket as fd 0
    if (dup2(socket, 0))
      exit(-1);

    // exec sudoport as sudo
    execl("/usr/bin/sudo", "sudoport", "sudoport", ip_str, port_str, NULL);

    // exec failed
    fprintf(stderr, "libsudoport: sudo failed to launch sudoport\n");
    exit(EPERM);
  }

  // wait for child (from parent)
  if (waitpid(child, &status, 0) != child) { // lost child
    fprintf(stderr, "libsudoport: sudoport unable to be tracked\n");
    errno = EPERM; // fix this
  } else if (!WIFEXITED(status)) { // illegal child
    fprintf(stderr, "libsudoport: sudoport terminated abnormally\n");
    errno = EPERM; // fix this
  } else if ((result = WEXITSTATUS(status))) { // complaining child
    fprintf(stderr, "libsudoport: sudoport exited with %d\n", result);
    errno = result; // fix this
  } else { // mission accomplished!
    return 0;
  }

  // failed
  return errno;
}

DYLD_INTERPOSE(my_bind, bind);
