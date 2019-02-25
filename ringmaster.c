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

#define POTATO_SIZE 5120
#define SHORT_MSG_SIZE 512

#define LISTEN_BACKLOG 100 // 1000? more?

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

void get_player_host(int player_fd, char *hostname, char *port) {
  ssize_t recv_status;
  char buffer[SHORT_MSG_SIZE];
  char *arr[2];
  /* char *hostname = NULL; */
  /* char *port = NULL; */

  recv_status = recv(player_fd, buffer, SHORT_MSG_SIZE, MSG_WAITALL);
  if (recv_status == -1) {
    fprintf(stderr, "Failed to recv data");
    exit(EXIT_FAILURE);
  }

  if (recv_status == 0) {
    fprintf(stderr, "Connection closed by server\n");
    exit(EXIT_FAILURE);
  }

  printf("Recv returned %ld\n", recv_status); // remove
  buffer[recv_status] = '\0';
  printf("Server said:\t%s\n", buffer); // remove

  parse_msgs(buffer, arr, 2);

  strncpy(hostname, arr[0], SHORT_MSG_SIZE);
  strncpy(port, arr[1], SHORT_MSG_SIZE);

  printf("hostname:\t%s\n"
         "port:\t%s\n",
         hostname, port); // remove

  /* *id_ptr = id; */
  /* *tot_ptr = tot; */

  return;
}

int main(int argv, char *argc[]) {

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

    players_info[id].fd = player_fd;
    printf("player id:\t%lu\n"
           "sock_id:\t%d\n",
           id,
           players_info[id].fd); // remove

    // 01 send "id:###|tot:###|"
    send_player_id_tot(player_fd, id, rm_ip->num_players);

    // recv their listening port
    // 02 recv "hostname~###|port~###|"
    /* get_player_host(player_fd, players_info[id].hostname, */
    /*                 players_info[id].port); */
  }

  // man select

  // close server
  /* freeaddrinfo(rm_info_list); */
  free(rm_ip);
  close(rm_fd);

  return EXIT_SUCCESS;
}
