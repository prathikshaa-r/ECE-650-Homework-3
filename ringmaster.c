/**
 * Prathikshaa
 * February 14, 2019
 * Attempt-2 after irresponsible file loss.
 * Using git now.
 * Caution -- always double check your make clean
 * https://ubuntuforums.org/archive/index.php/t-1091184.html
 * "Always test a new Makefile before use." -- golden words by dwhitney67
 *
 */

#include "potato.h"
#include <unistd.h>

// todo: is the parse_input function vulnerable to buffer overflow due to
// dynamic memory alloc?

void parse_input(int margv, char *margc[], ringmaster_inputs_t *inputs) {
  if (margv != 4) {
    printf("%d is an invalid number of arguments.\n"
           "Usage: %s <port_num> <num_players> <num_hops>\n",
           margv, margc[0]);
    exit(EXIT_FAILURE);
  }

  // convert inputs to numbers
  // inputs->port_num = margc[1];

  // check: num_players > 1
  // check: num_hops >= 0 | <= 512

  // if all ok, return
  // else exit FAILURE
}

// learning comment: looks like void functions set perror and functons that
// actually return a value like size_t return -1 for error

int main(int argv, char *argc[]) {
  // parse input
  ringmaster_inputs_t *rm_ip = malloc(sizeof(ringmaster_inputs_t));
  parse_input(argv, argc, rm_ip);
  printf("Potato Ringmaster\n");
  printf("Players = %lu\n", rm_ip->num_players);
  printf("Hops = %lu\n", rm_ip->num_hops);

  // if parse function returns -1, exit.

  return EXIT_SUCCESS;
}
