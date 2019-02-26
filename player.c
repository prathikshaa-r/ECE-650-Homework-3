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

  /**
   * rm_fd = client_socket --> connect to ringmaster
   * server_fd = server_socket --> accept connection from left player
   * l_fd = accepted fd on server_fd
   * r_fd = client_socket --> connect to right player
   */

  int rm_fd, server_fd, l_fd, r_fd;
  size_t id;
  size_t tot;

  char r_hostname[SHORT_MSG_SIZE];
  char r_port[SHORT_MSG_SIZE];

  memset(&r_hostname, 0, SHORT_MSG_SIZE);
  memset(&r_port, 0, SHORT_MSG_SIZE);

  int fd_max;
  fd_set master;
  fd_set read_fds;
  FD_ZERO(&master);
  FD_ZERO(&read_fds);

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

  // connect to neighbours to receive potato
  // recv "r_hostname~###|r_port~###|"
  for (int i = 0; i < 2; i++) {
    int ret = get_right_neigh(rm_fd, r_hostname, r_port);
    switch (ret) {
    case 0:
      // accept
      l_fd = accept(server_fd, (struct sockaddr *)&r_player_addr,
                    &r_player_addr_len);
      if (l_fd == -1) {
        fprintf(stderr, "Error: Failed to accept connection.\n");
        exit(EXIT_FAILURE);
      }
      break;
    case 3:
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

  int l_id = (id == 0) ? (tot - 1) : (id - 1);
  int r_id = (id == (tot - 1)) ? 0 : (id + 1);

  // r_fd = 0;
  // l_fd = 0;
  printf("rm_fd = %d\n", rm_fd);
  printf("r_fd = %d\n", r_fd);
  printf("l_fd = %d\n", l_fd);

  fd_max = (r_fd > l_fd) ? r_fd : l_fd;
  FD_SET(rm_fd, &master);
  FD_SET(l_fd, &master);
  FD_SET(r_fd, &master);

  // send "r~ready|" to ringmaster
  send_ready_signal(rm_fd);

  // recv potato or END signal - may come from ringmaster or neighbours
  while (1) {
    read_fds = master;
    int ready = select(fd_max + 1, &read_fds, NULL, NULL, NULL);
    if (ready == -1) {
      perror("Error: select\n");
      exit(EXIT_FAILURE);
    } /* else if (ready == 0) { */
    /*   printf("No player ready.\n"); // remove */
    /*   continue; */
    /* } else if (ready != 1) { */
    /*   fprintf(stderr, */
    /*           "Error: Should receive potato from only one
     * player/ringmaster.\n" */
    /*           "Ready:\t%d\n", */
    /*           ready); */
    /*   exit(EXIT_FAILURE); */
    /* } */

    // ready == 1
    // end game
    // receive final potato
    int ready_fd = -1;

    if (FD_ISSET(rm_fd, &read_fds)) {
      ready_fd = rm_fd;
      printf("Ringmaster is ready\n");
    } else if (FD_ISSET(l_fd, &read_fds)) {
      ready_fd = l_fd;
      printf("Left is ready.\n");
    } else if (FD_ISSET(r_fd, &read_fds)) {
      ready_fd = r_fd;
      printf("Right is ready.\n");
    }

    if (ready_fd == -1) {
      fprintf(stderr, "Error: Failed to identify the ready player.\n");
      exit(EXIT_FAILURE);
    }

    // parse potato stream
    size_t num_hops;
    size_t trace_len = POTATO_SIZE - 4;
    char trace[trace_len];
    memset(&trace, 0, trace_len);

    if (get_potato(ready_fd, &num_hops, trace)) {
      // end signal received
      break;
    }

    /* char id_str[8]; */
    /* snprintf(id_str, 8, ",%lu", id); */

    num_hops--;
    /* strcat(trace, id_str); */
    /* trace[trace_len - 1] = '\0'; */

    char potato_buf[POTATO_SIZE];
    memset(&potato_buf, 0, POTATO_SIZE);

    if (strcmp(trace, "")) {
      snprintf(potato_buf, POTATO_SIZE, "%lu|%s,%lu", num_hops, trace, id);
    } else {
      snprintf(potato_buf, POTATO_SIZE, "%lu|%lu", num_hops, id);
    }

    if (num_hops == 0) {
      // send to rm_fd
      printf("I'm it.\n");
      send_all(rm_fd, potato_buf, POTATO_SIZE);
    } else {
      // seed random function
      srand((unsigned int)time(NULL) + id);
      int random = rand() % 2;
      switch (random) {
      case 0:
        printf("Sending potato to %d\n", l_id);
        send_all(l_fd, potato_buf, POTATO_SIZE);
        break;
      case 1:
        printf("Sending potato to %d\n", r_id);
        send_all(r_fd, potato_buf, POTATO_SIZE);
        break;
      default:
        fprintf(stderr, "Invalid value:%d\n", random);
        break;
      }
    }

  } // end while

  // dec num of hops by 1. if 0, "I'm it." pass potato to ringmaster.

  // if ringmaster stream sends "END_GAME" -- close all sockets and exit
  // successfully.

  free(p_ip);
  close(rm_fd);
  close(server_fd);
  close(l_fd);
  close(r_fd);

  return EXIT_SUCCESS;
}
