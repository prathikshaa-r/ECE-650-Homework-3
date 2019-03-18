#include "potato.h"

void print_listener_port(int listener_fd) {
  struct sockaddr_in listen_addr;
  socklen_t listen_addr_len = sizeof(listen_addr);

  int sockname_status = getsockname(
      listener_fd, (struct sockaddr *)&listen_addr, &listen_addr_len);

  if (sockname_status == -1) {
    perror("getsockname:");
    exit(EXIT_FAILURE);
  }

  size_t len = SHORT_MSG_SIZE;
  char my_hostname[len]; // = NULL;
  if (gethostname(my_hostname, len) != 0) {
    perror("Failed to get host name.\n");
    exit(EXIT_FAILURE);
  }

  if (my_hostname[len - 1] != '\0') {
    printf("hostname longer than %lu characters:%s\n", len, my_hostname);
  }
  printf("hostname:\t%s\n", my_hostname);
  int my_port = ntohs(listen_addr.sin_port);

  printf("Listening on port %d\n", my_port);

  char buf_my_serv_info[SHORT_MSG_SIZE];
  len = SHORT_MSG_SIZE;
  memset(&buf_my_serv_info, 0, len);
  if (snprintf(buf_my_serv_info, len, "hostname~%s|port~%d", my_hostname,
               my_port) < 0) {
    fprintf(stderr, "building string using snprintf failed.\n");
    exit(EXIT_FAILURE);
  }

  printf("%s\n", buf_my_serv_info);
}
