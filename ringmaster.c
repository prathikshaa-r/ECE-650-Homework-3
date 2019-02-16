/**
 * ringmaster.c
 * Usage: ./ringmaster <port_num> <num_players> <num_hops>
 *
 * Prathikshaa
 * February 14, 2019
 *
 * Attempt-2 after irresponsible file loss.
 * Using git now.
 * Caution -- always double check your make clean
 * https://ubuntuforums.org/archive/index.php/t-1091184.html
 * "Always test a new Makefile before use." -- golden words by dwhitney67
 *
 */

// C/C++ style guides:
// https://www.cs.swarthmore.edu/~newhall/unixhelp/c_codestyle.html,
// https://google.github.io/styleguide/cppguide.html#C++_Version

#define MAX_HOPS 512
#define MIN_HOPS 0
#define MIN_PLAYERS 1

#include "potato.h"
#include <unistd.h>

// sockets
#include <sys/socket.h>
#include <sys/types.h>

// addrinfo
#include <netdb.h>

// todo: is the parse_input function vulnerable to buffer overflow due to
// dynamic memory alloc?

size_t str_to_num(char *str) {
  printf("string: %s\n", str);
  // use strtol

  // negative nums ?? what error
  return 0;
}

void parse_input(int margv, char *margc[], ringmaster_inputs_t *inputs) {
  if (margv != 4) {
    printf("%d is an invalid number of arguments.\n"
           "Usage: %s <port_num> <num_players> <num_hops>\n",
           margv, margc[0]);
    exit(EXIT_FAILURE);
  }

  printf("program name:\t%s\n"
         "port_num:\t%s\n"
         "num_players:\t%s\n"
         "num_hops:\t%s\n",
         margc[0], margc[1], margc[2], margc[3]); // remove

  // convert inputs to numbers -- strtol
  // inputs->port_num = margc[1];
  size_t port_num = str_to_num(margc[1]);
  size_t num_players = str_to_num(margc[2]);
  size_t num_hops = str_to_num(margc[3]);

  printf("PORT NUM:\t%lu\n", port_num); // remove

  // check: num_players > 1
  // check: num_hops >= 0 | <= 512
  if (num_players < MIN_PLAYERS) {
    printf("Minimum no. of players is %d\n", MIN_PLAYERS);
    exit(EXIT_FAILURE);
  }
  if ((num_hops < MIN_HOPS) || (num_hops > MAX_HOPS)) {
    printf("No. of hops must be in range [%d, %d]", MIN_HOPS, MAX_HOPS);
    exit(EXIT_FAILURE);
  }

  // if all ok, return
  // else exit FAILURE
  inputs->port_num = port_num;
  inputs->num_players = num_players;
  inputs->num_hops = num_hops;
  return;
}

// learning comment: looks like void functions set perror and functons that
// actually return a value like size_t return -1 for error

int main(int argv, char *argc[]) {
  // parse input
  ringmaster_inputs_t *rm_ip = malloc(sizeof(ringmaster_inputs_t)); // free
  parse_input(argv, argc, rm_ip);
  printf("Potato Ringmaster\n");
  printf("Players = %lu\n", rm_ip->num_players);
  printf("Hops = %lu\n", rm_ip->num_hops);

  // if parse function returns -1, exit.

  // open socket to listen to players
  // refer to multi-client video here

  // man select

  return EXIT_SUCCESS;
}
