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
  // parse inputs
  player_inputs_t *p_ip = malloc(sizeof(player_inputs_t)); // free
  parse_p_inputs(argv, argc, p_ip);

  printf("Machine Name:\t%s\n", p_ip->machine_name); // remove
  printf("Port Num:\t%s\n", p_ip->port_num);         // remove

  int rm_fd = open_client_socket(p_ip->machine_name, p_ip->port_num);
  if (rm_fd == -1) {
    fprintf(stderr,
            "Failed to establish connection with ringmaster at %s, %s\n",
            p_ip->machine_name, p_ip->port_num);
    exit(EXIT_FAILURE);
  }

  // get player id and num_players from ringmaster
  // 01 recv "id:###|tot:###"
  char buffer[SHORT_MSG_SIZE]; // random len
  ssize_t read_id;

  read_id = recv(rm_fd, buffer, SHORT_MSG_SIZE, MSG_WAITALL);
  if (read_id == -1) {
    fprintf(stderr, "Failed to recv data");
    exit(EXIT_FAILURE);
  }

  if (read_id == 0) {
    fprintf(stderr, "Connection closed by server\n");
    exit(EXIT_FAILURE);
  }

  printf("Recv returned %ld\n", read_id); // remove
  buffer[read_id] = '\0';
  printf("Server said:\t%s\n", buffer); // remove
  size_t id;
  size_t tot;
  char *arr[2];
  parse_msgs(buffer, arr, 2);
  id = str_to_num(arr[0]);
  tot = str_to_num(arr[1]);
  printf("id:\t%lu\n"
         "tot:\t%lu\n",
         id, tot); // remove

  printf("Connected as player %lu out of %lu total players.\n", id, tot);

  // 02 send "hostname~###|port~###|"
  int l_fd = open_server_socket(NULL, 0);
  if (l_fd == -1) {
    fprintf(stderr, "Player failed to open a server socket.\n");
    exit(EXIT_FAILURE);
  }

  send_player_info(rm_fd, l_fd);

  /* char *ack = "ACK"; */
  /* send(rm_fd, ack, strlen(ack), 0); */

  // bind to and listen on a free port

  // send port number to ringmaster

  // get neighbours info in stream
  // recv "left_ip:###|left_port:###|right_ip:###|right_port:###"

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
