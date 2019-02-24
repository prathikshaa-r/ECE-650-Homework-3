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

int main(int argv, char *argc[]) {
  // parse input
  ringmaster_inputs_t *rm_ip = malloc(sizeof(ringmaster_inputs_t)); // free
  parse_rm_input(argv, argc, rm_ip);
  printf("Potato Ringmaster\n");
  printf("Players = %lu\n", rm_ip->num_players);
  printf("Hops = %lu\n", rm_ip->num_hops);

  // if parse function returns -1, exit.

  // open socket to listen to players
  // refer to multi-client video here
  // server setup
  int rm_status;
  int rm_fd;
  struct addrinfo rm_hints;
  struct addrinfo *rm_info_list, *rm_it;

  /* server -- binds to ip, port
     client -- connects to ip, port
  */
  // get addr, create socket (internet endpoint), bind, listen,  accept, select

  // get addr
  memset(&rm_hints, 0, sizeof(rm_hints));
  rm_hints.ai_family = AF_UNSPEC;
  rm_hints.ai_socktype = SOCK_STREAM;
  rm_hints.ai_flags = AI_PASSIVE; // AI_CANONNAME;

  rm_status = getaddrinfo(NULL, rm_ip->port_num, &rm_hints, &rm_info_list);
  if (rm_status != 0) {
    fprintf(stderr, "Error: failed to get address of host:\n%s\n",
            gai_strerror(rm_status));
    exit(EXIT_FAILURE);
    //    handle_error("Error: failed to get addr of host\n");
  }

  // loop through all addresses until you can bind successfully
  for (rm_it = rm_info_list; rm_it != NULL; rm_it = rm_it->ai_next) {
    // socket
    rm_fd = socket(rm_it->ai_family, rm_it->ai_socktype, rm_it->ai_protocol);

    if (rm_fd == -1) {
      continue;
    }

    // todo: understand the port reuse mechanism
    int yes = 1;
    if (setsockopt(rm_fd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1) {
      perror("setsockopt:");
      exit(EXIT_FAILURE);
    }

    // bind
    if (bind(rm_fd, rm_it->ai_addr, rm_it->ai_addrlen) == 0) {
      break;
    }
    // if failed to bind
    close(rm_fd);
  }

  if (rm_it == NULL) {
    // no address succeeded
    fprintf(stderr, "Error: failed to bind to any address retrieved");
    exit(EXIT_FAILURE);
  }

  // listen
  if (listen(rm_fd, LISTEN_BACKLOG) == -1) {
    perror("Error: cannot listen on socket\n");
    exit(EXIT_FAILURE);
  }

  printf("Listening on port %s\n", rm_ip->port_num); // remove

  struct sockaddr_storage player_addr;
  socklen_t player_addr_len = sizeof(player_addr);

  // struct array for storing players info
  player_info_t players_info[rm_ip->num_players];

  /* player_info_t *ring_head = &players_info[0]; // player id 0 */
  /* player_info_t *ring_tail = */
  /*     &players_info[rm_ip->num_players - 1]; // player id num_players - 1 */

  // accept incoming connections
  for (size_t i = 0; i < rm_ip->num_players; i++) {

    int player_fd =
        accept(rm_fd, (struct sockaddr *)&player_addr, &player_addr_len);

    if (player_fd == -1) {
      perror("Error: failed to accept connection on socket\n");
    }

    players_info[i].fd = player_fd;
    printf("player id:\t%lu\nsock_id:\t%d\n", i, players_info[i].fd); // remove

    // send ("id:%lu|tot:%lu", i, rm_ip->num_players)

    // listen to <num_players> players

    // send player ("id~%lu|tot~%lu", i, rm_ip->num_players)

    // recv their listening port
  }

  // man select

  // close server
  freeaddrinfo(rm_info_list);
  close(rm_fd);

  return EXIT_SUCCESS;
}
