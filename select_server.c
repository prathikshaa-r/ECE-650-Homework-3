#include "get_port.h"
#include "potato.h"

int main(void) {
  printf("Starting select server");

  int listener = open_server_socket(NULL, "0");
  if (listener == -1) {
    fprintf(stderr, "Failed to start select server.\n");
    exit(EXIT_FAILURE);
  }

  print_listener_port(listener);

  return EXIT_SUCCESS;
}
