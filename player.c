/**
 * player.c
 * Usage: ./player <ringmaster_machine_name> <ringmaster_port_num>
 *
 * Prathikshaa
 * February 16, 2019
 */
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

  int rm_status;
  int rm_fd;
  struct addrinfo rm_hints;
  struct addrinfo *rm_info_list, *rm_it;

  // get the addr of the ringmaster
  memset(&rm_hints, 0, sizeof(rm_hints));
  rm_hints.ai_family = AF_UNSPEC; // v4 v6 agnostic
  rm_hints.ai_socktype = SOCK_STREAM;
  rm_hints.ai_flags = AI_CANONNAME; // cannon name

  rm_status =
      getaddrinfo(p_ip->machine_name, p_ip->port_num, &rm_hints, &rm_info_list);

  if (rm_status != 0) {
    fprintf(stderr, "Error: cannot get addr info for ringmaster:\n%s\n",
            gai_strerror(rm_status));
    exit(EXIT_FAILURE);
    // ref: beej.us/guide/bgnet/examples/selectserver.c
  }

  // connect to ringmaster
  // from man getaddrinfo -- loop till we connect
  for (rm_it = rm_info_list; rm_it != NULL; rm_it = rm_it->ai_next) {
    rm_fd = socket(rm_it->ai_family, rm_it->ai_socktype, rm_it->ai_protocol);
    if (rm_fd == -1) {
      fprintf(
          stderr,
          "Error: cannot create socket to connect to ringmaster\n"); // remove
      continue;
    }
    if (connect(rm_fd, rm_it->ai_addr, rm_it->ai_addrlen) != -1) {
      // successful connecion to ringmaster
      break;
    }
    close(rm_fd); // failed to connect
  }

  if (rm_it == NULL) {
    fprintf(stderr, "Error: Failed to connect to ringmaster.\n");
    exit(EXIT_FAILURE);
  }

  // get player id and num_players from ringmaster
  // recv "id:###|tot:###"
  char buffer[512]; // random len
  ssize_t read_id;
  //  while (1) {
  read_id = recv(rm_fd, buffer, 512, 0);
  if (read_id == -1) {
    fprintf(stderr, "Failed to recv data");
  }
  /*  else */
  /*     break; */
  /* } */
  if (read_id == 0) {
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

  // get neighbours info in stream
  // recv "left_ip:###|left_port:###|right_ip:###|right_port:###"

  // connect to neighbours to receive potato | may come from ringmaster or
  // neighbours

  // parse potato stream

  // dec num of hops by 1. if 0, "I'm it." pass potato to ringmaster.

  // if ringmaster stream sends "END_GAME" -- close all sockets and exit
  // successfully.

  freeaddrinfo(rm_info_list);
  close(rm_fd);

  return EXIT_SUCCESS;
}
