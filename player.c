/**
 * player.c
 * Usage: ./player <ringmaster_machine_name> <ringmaster_port_num>
 *
 * Prathikshaa
 * February 16, 2019
 */

#include <stdio.h>
#include <stdlib.h>

// socket
#include <sys/socket.h>
#include <sys/types.h>

// socket IP addr
#include <netdb.h>

int main(int argv, char *argc[]) {
  // parse inputs

  // connect to ringmaster

  // get neighbours info in stream

  // connect to neighbours to receive potato | may come from ringmaster or
  // neighbours

  // parse potato stream

  // dec num of hops by 1. if 0, "I'm it." pass potato to ringmaster.

  // if ringmaster stream sends "END_GAME" -- close all sockets and exit
  // successfully.

  return EXIT_SUCCESS;
}
