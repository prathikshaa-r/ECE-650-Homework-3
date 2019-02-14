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

void parse_input(int margv, char *margc[], ringmaster_inputs_t *inputs) {
  if (margv != 4) {
    printf("%d is an invalid number of arguments.\n"
           "Usage: %s <port_num> <num_players> <num_hops>",
           margv, margc[0]);
  }
}

// learning comment: looks like void functions set perror and functons that
// actually return a value like size_t return -1 for error

int main(int argv, char *argc[]) {
  // parse input

  // if parse function returns -1, exit.

  return EXIT_SUCCESS;
}
