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
#include <errno.h>
#include <unistd.h>

// sockets
#include <sys/socket.h>
#include <sys/types.h>

// addrinfo
#include <netdb.h>

void parse_rm_input(int margv, char *margc[], ringmaster_inputs_t *inputs) {
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

  // parsing the string to numbers
  const char *port_num = margc[1];
  size_t num_players = str_to_num(margc[2]);
  size_t num_hops = str_to_num(margc[3]);

  printf("PORT NUM:\t%s\n", port_num); // remove

  // check: num_players > 1
  // check: num_hops >= 0 | <= 512
  if (num_players < MIN_PLAYERS) {
    printf("Minimum no. of players is %d\n", MIN_PLAYERS);
    exit(EXIT_FAILURE);
  }
  if ((num_hops < MIN_HOPS) || (num_hops > MAX_HOPS)) {
    printf("No. of hops must be in range [%d, %d]\n", MIN_HOPS, MAX_HOPS);
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

  int fd_max;
  fd_set master;
  fd_set read_fds;
  FD_ZERO(&master);
  FD_ZERO(&read_fds);

  size_t end_hops;
  size_t trace_len = POTATO_SIZE - 4;
  char trace[trace_len];
  memset(&trace, 0, trace_len);

  // parse input
  ringmaster_inputs_t *rm_ip = malloc(sizeof(ringmaster_inputs_t)); // free
  parse_rm_input(argv, argc, rm_ip);
  printf("Potato Ringmaster\n"
         "Players = %lu\n"
         "Hops = %lu\n",
         rm_ip->num_players, rm_ip->num_hops);

  // server socket at port defined in input
  int rm_fd = open_server_socket(NULL, rm_ip->port_num);
  if (rm_fd == -1) {
    fprintf(stderr, "Failed to listen on port %s\n", rm_ip->port_num);
    exit(EXIT_FAILURE);
  }

  struct sockaddr_storage player_addr;
  socklen_t player_addr_len = sizeof(player_addr);

  // struct array for storing players info
  player_info_t players_info[rm_ip->num_players];

  /* player_info_t *ring_head = &players_info[0]; // player id 0 */
  /* player_info_t *ring_tail = */
  /*     &players_info[rm_ip->num_players - 1]; // player id num_players - 1 */

  // accept incoming connections
  for (size_t id = 0; id < rm_ip->num_players; id++) {

    int player_fd =
        accept(rm_fd, (struct sockaddr *)&player_addr, &player_addr_len);

    if (player_fd == -1) {
      perror("Error: failed to accept connection on socket\n");
    }

    // include player_fd in set
    FD_SET(player_fd, &master);
    // reset fd_max with latest player_fd
    if (player_fd > fd_max) {
      fd_max = player_fd;
    }

    players_info[id].id = id;
    players_info[id].fd = player_fd;
    printf("player id:\t%lu\n"
           "sock_id:\t%d\n",
           id,
           players_info[id].fd); // remove

    // 01 send "id:###|tot:###|"
    send_player_id_tot(player_fd, id, rm_ip->num_players);

    // recv their listening port
    // 02 recv "hostname~###|port~###|"
    get_player_host(player_fd, players_info[id].hostname,
                    players_info[id].port);

    if (id == (rm_ip->num_players - 1)) {
      players_info[id].right = &players_info[0];
    } else {
      players_info[id].right = &players_info[id + 1];
    }

    if (id == 0) {
      players_info[id].left = &players_info[rm_ip->num_players - 1];
    } else {
      players_info[id].left = &players_info[id - 1];
    }
  } // end for

  for (size_t id = 0; id < rm_ip->num_players; id++) {
    printf("player id:\t%d | hostname:\t%s | port:\t%s\n"
           "r_neigh id:\t%d | hostname:\t%s | port:\t%s\n\n",
           players_info[id].id, players_info[id].hostname,
           players_info[id].port, players_info[id].right->id,
           players_info[id].right->hostname, players_info[id].right->port);
  }

  // send neighbour info to all players
  for (size_t id = 0; id < rm_ip->num_players; id++) {
    size_t right = (id == (rm_ip->num_players - 1)) ? 0 : (id + 1);

    // send second one in pair "a~accept|" signal to let them accept connection
    send_accept_signal(players_info[right].fd);

    // send first one in pair r_neigh info to connect to
    send_right_neigh(players_info[id].fd, &players_info[right]);
  }

  // master fds

  // get ready signal from all players
  for (size_t id = 0; id < rm_ip->num_players; id++) {

    /* int fd; */
    /* read_fds = master; */

    /* if (select(fd_max + 1, &read_fds, NULL, NULL, NULL) == -1) { */
    /*   perror("Error: select to get ready signal\n"); */
    /*   exit(EXIT_FAILURE); */
    /* } */

    /* if(FD_ISSET(players_info[id].fd, &read_fds)){ */
    /*   fd = ; */
    /* } */

    int ret = get_player_host(players_info[id].fd, NULL, NULL);
    if (ret == 1) {
      printf("recvd ready signal from player %d\n", players_info[id].id);
    }
  }

  // seed random in ringmaster
  srand((unsigned int)time(NULL));
  int random = rand() % (rm_ip->num_players);

  printf("Ready to start the game, sending potato to player %d.\n", random);

  /*-------------------------Send Potato to Player-------------------------*/
  size_t len = POTATO_SIZE;
  char hops[len];
  memset(&hops, 0, len);

  if (snprintf(hops, len, "%lu|", rm_ip->num_hops) < 0) {
    fprintf(stderr, "Error: potato snprintf failed.\n");
    exit(EXIT_FAILURE);
  }

  printf("Potato:\n%s\n", hops); // remove

  send_all(players_info[random].fd, hops, len);
  /*------------------------------------------------------------------------*/

  printf("Sent potato to socket %d.\n", players_info[random].fd);
  // man select
  while (1) {
    read_fds = master;
    int ready = select(fd_max + 1, &read_fds, NULL, NULL, NULL);
    if (ready == -1) {
      perror("Error: select\n");
      exit(EXIT_FAILURE);
    } else if (ready == 0) {
      printf("No player ready.\n"); // remove
      continue;
    } else if (ready != 1) {
      fprintf(stderr,
              "Error: Should receive final potato from only one player.\n"
              "Ready:\t%d\n",
              ready);
      exit(EXIT_FAILURE);
    }
    // ready == 1
    // end game
    // receive final potato
    int ready_fd = -1;
    for (size_t id = 0; id < rm_ip->num_players; id++) {
      if (FD_ISSET(players_info[id].fd, &read_fds)) {
        ready_fd = players_info[id].fd;
        break;
      }
    } // end for

    if (ready_fd == -1) {
      fprintf(stderr, "Error: Failed to identify the ready player.\n");
      exit(EXIT_FAILURE);
    }

    get_potato(ready_fd, &end_hops, trace);
    printf("Trace of potato:\n"
           "%s\n",
           trace);
    break;

  } // end while

  for (size_t id = 0; id < rm_ip->num_players; id++) {
    send_end_signal(players_info[id].fd);
  }

  // close server
  /* freeaddrinfo(rm_info_list); */
  free(rm_ip);
  close(rm_fd);

  return EXIT_SUCCESS;
}
