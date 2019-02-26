/**
 * player.c
 * Usage: ./player <ringmaster_machine_name> <ringmaster_port_num>
 *
 * Prathikshaa
 * February 16, 2019
 */
#define POTATO_SIZE 5120
#define SHORT_MSG_SIZE 512

#include "potato.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h> // memset

// socket
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

// socket IP addr
#include <netdb.h>

void parse_p_inputs(int margv, char *margc[], player_inputs_t *inputs) {
  if (margv != 3) {
    fprintf(stderr,
            "%d is an invalid number of arguments.\n"
            "Usage: %s <ringmaster_machine_name> <ringmaster_port_num>\n",
            margv, margc[0]);
    exit(EXIT_FAILURE);
  }

  printf("program name:\t\t\t%s\n"
         "ringmaster_machine_name:\t%s\n"
         "ringmaster_port_num:\t\t%s\n",
         margc[0], margc[1], margc[2]); // remove
  inputs->machine_name = margc[1];      // todo: does this work?
  inputs->port_num = margc[2];          // defined in potato.h
  return;
}

int main(int argv, char *argc[]) {
  /* rm_fd = client_socket --> connect to ringmaster
     server_fd = server_socket --> accept connection from left player
     l_fd = accepted fd on server_fd
     r_fd = client_socket --> connect to right player
   */
  int rm_fd, server_fd, l_fd, r_fd;
  size_t id;
  size_t tot;

  char r_hostname[SHORT_MSG_SIZE];
  char r_port[SHORT_MSG_SIZE];

  struct sockaddr_storage r_player_addr;
  socklen_t r_player_addr_len = sizeof(r_player_addr);

  // parse inputs
  player_inputs_t *p_ip = malloc(sizeof(player_inputs_t)); // free
  parse_p_inputs(argv, argc, p_ip);

  printf("Machine Name:\t%s\n", p_ip->machine_name); // remove
  printf("Port Num:\t%s\n", p_ip->port_num);         // remove

  rm_fd = open_client_socket(p_ip->machine_name, p_ip->port_num);
  if (rm_fd == -1) {
    fprintf(stderr,
            "Failed to establish connection with ringmaster at %s, %s\n",
            p_ip->machine_name, p_ip->port_num);
    exit(EXIT_FAILURE);
  }

  // get player id and num_players from ringmaster
  // 01 recv "id:###|tot:###"
  get_id_tot(rm_fd, &id, &tot);

  printf("Connected as player %lu out of %lu total players.\n", id, tot);

  // 02 send "hostname~###|port~###|"
  server_fd =
      open_server_socket(NULL, "0"); // bind to and listen on a free port

  if (server_fd == -1) {
    fprintf(stderr, "Player failed to open a server socket.\n");
    exit(EXIT_FAILURE);
  }

  send_player_port(server_fd, rm_fd); // send port number to ringmaster

  /* char *ack = "ACK"; */
  /* send(rm_fd, ack, strlen(ack), 0); */

  // get neighbours info in stream
  // recv "left_ip:###|left_port:###|right_ip:###|right_port:###"
  for (int i = 0; i < 2; i++) {
    int ret = get_right_neigh(rm_fd, r_hostname, r_port);
    switch (ret) {
    case 0:
      // accept
      l_fd = accept(server_fd, (struct sockaddr *)&r_player_addr,
                    &r_player_addr_len);
      break;
    case 1:
      // connect to right_neigh
      r_fd = open_client_socket(r_hostname, r_port);
      break;
    default:
      fprintf(stderr,
              "Error: player id %lu recv neigh/accept: in valid value %d\n", id,
              ret);
      break;
    }
  }

  // r_fd = 0;
  // l_fd = 0;
  printf("rm_fd = %d\n", rm_fd);
  printf("r_fd = %d\n", r_fd);
  printf("l_fd = %d\n", l_fd);

  // connect to neighbours to receive potato | may come from ringmaster or
  // neighbours

  // parse potato stream

  // dec num of hops by 1. if 0, "I'm it." pass potato to ringmaster.

  // if ringmaster stream sends "END_GAME" -- close all sockets and exit
  // successfully.

  free(p_ip);
  close(rm_fd);

  return EXIT_SUCCESS;
}
