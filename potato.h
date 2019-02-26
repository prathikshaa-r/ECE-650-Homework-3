/**
 * potato.h
 *
 * Prathikshaa
 * February 14, 2019
 *
 * Attempt-2
 *
 */

#ifndef __POTATO_H__
#define __POTATO_H__

#define SHORT_MSG_SIZE 512
#define POTATO_SIZE 5120
#define LISTEN_BACKLOG 100

#include <assert.h>
#include <errno.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>

typedef struct _potato_t {
  size_t num_hops;
  size_t players[];
  // maybe unnecessary
} potato_t;

typedef struct _ringmaster_inputs_t {
  const char *port_num;
  size_t num_players;
  size_t num_hops;
} ringmaster_inputs_t;

typedef struct _player_inputs_t {
  const char *machine_name;
  const char *port_num;
} player_inputs_t;

typedef struct _player_info_t {
  int id;
  int fd;
  char hostname[SHORT_MSG_SIZE];
  char port[SHORT_MSG_SIZE];
  struct _player_info_t *left;
  struct _player_info_t *right;
} player_info_t;

/*----------------server and client socket------------------*/
int open_server_socket(const char *hostname, const char *port);
int open_client_socket(const char *server_hostname, const char *server_port);

void send_all(int fd, char *buf, size_t size); // todo: test

void send_signal(int sig, int send_to_fd); // 0 - accept, 1 - ready, 2 - end

/*-----------------------------------------------------------*/

/*-----------------------parsing functions-------------------*/
// ringmaster.c - ringmaster inputs
void parse_rm_input(int margv, char *margc[], ringmaster_inputs_t *inputs);

// player.c - player inputs
void parse_p_inputs(int margv, char *margc[], player_inputs_t *inputs);

// potato.h - string to number conversion function
size_t str_to_num(const char *str);

// potato.h - parse messsages from ringmaster - return num of fields found
size_t parse_msgs(char *msg, char *results[], size_t num_fields);
/*-------------------------------------------------------------*/

/*----------------------------Player---------------------------*/
// "id~##|tot~##|"
void get_id_tot(int rm_fd, size_t *id_ptr, size_t *tot_ptr);

// "hostname~###|port~###|"
void send_player_port(int listener_fd, int send_to_fd);

// "r_hostname~###|r_port~###|"
int get_right_neigh(int player_fd, char *hostname, char *port); // todo: test

// "r~ready|"
void send_ready_signal(int rm_fd); // todo: write
/*-------------------------------------------------------------*/

/*------------------------Ringmaster---------------------------*/
// "id~##|tot~##|"
void send_player_id_tot(int fd, size_t player_id, size_t num_players);

// "hostname~###|port~###|"
int get_player_host(int player_fd, char *hostname, char *port);

// "r_hostname~###|r_port~###|"
void send_right_neigh(int player_fd, player_info_t *r_neigh); // todo: test

// "a~accept|"
void send_accept_signal(int player_fd); // todo: write
/*-------------------------------------------------------------*/

/*----------------Function Implementations---------------------*/
size_t parse_msgs(char *msg, char *results[], size_t num_fields) {
  // find the fields using pipe -- |
  char *str1, *str2, *field, *value, *n_value;
  char *t1 = "|";
  char *t2 = "~";
  char *saveptr1, *saveptr2;
  size_t j = 0;

  for (j = 0, str1 = msg;; j++, str1 = NULL) {
    field = strtok_r(str1, t1, &saveptr1);
    if (field == NULL) {
      break;
    }

    printf("Field %lu:\t%s\n", j, field);
    // parse field to get value using '~'
    for (str2 = field;; str2 = NULL) {
      value = n_value;
      n_value = strtok_r(str2, t2, &saveptr2);
      if (n_value == NULL) {
        break;
      }
    }

    printf("n_value:\t%s\n", n_value);
    printf("Value:\t%s\n", value);
    results[j] = value;
  }
  if (j != num_fields) {
    fprintf(stderr, "expected %lu fields, found only %lu\n", num_fields, j);
  }

  return j; // return num of fields found
}

// todo: is the parse_input function vulnerable to buffer overflow due to
// dynamic memory alloc?
size_t str_to_num(const char *str) {
  printf("string: %s\n", str);
  // use strtoul
  char *endptr;
  // check for -ve nos.
  if (str[0] == '-') {
    printf("Invalid Input:\t%s\n", str);
    exit(EXIT_FAILURE);
  }
  errno = 0;
  size_t val = strtoul(str, &endptr, 10);

  if (errno) {
    perror("Invalid Input: ");
    exit(EXIT_FAILURE);
  }

  if (endptr == str) {
    fprintf(stderr, "Invalid Input:\t%s\nNo digits were found.\n", str);
    exit(EXIT_FAILURE);
  }

  return val;
}

int open_server_socket(const char *hostname, const char *port) {
  assert(hostname == NULL); // for server socket
  str_to_num(port);

  int fd;
  int status;
  struct addrinfo hints;
  struct addrinfo *addr_list, *addr_iterator;

  memset(&hints, 0, sizeof hints);
  hints.ai_family = AF_UNSPEC;
  hints.ai_socktype = SOCK_STREAM;
  hints.ai_flags = AI_PASSIVE;

  status = getaddrinfo(hostname, port, &hints, &addr_list);

  if (status != 0) {
    fprintf(stderr, "Error: getaddrinfo\n%s", gai_strerror(status));
    freeaddrinfo(addr_list);
    return -1;
  }

  for (addr_iterator = addr_list; addr_iterator != NULL;
       addr_iterator = addr_iterator->ai_next) {
    fd = socket(addr_iterator->ai_family, addr_iterator->ai_socktype,
                addr_iterator->ai_protocol);
    if (fd == -1) {
      // socket failed at addr. try next addr
      continue;
    }

    /*----------socket succeeded-------*/
    int yes = 1;
    if (setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1) {
      perror("Error:setsockopt\n");
      freeaddrinfo(addr_list);
      return -1;
    }

    /*---------setsockopt succeeded-------*/
    // bind
    if (bind(fd, addr_iterator->ai_addr, addr_iterator->ai_addrlen) == 0) {
      // bind succeeded -- get out of loop
      break;
    }

    /*----------bind failed--------------*/
    close(fd);
  }

  if (addr_iterator == NULL) {
    fprintf(stderr, "Error: failed to bind to any of the addresses retrieved");
    freeaddrinfo(addr_list);
    return -1;
  }

  // listen
  if (listen(fd, LISTEN_BACKLOG) == -1) {
    perror("Error: cannot listen on socket\n");
    freeaddrinfo(addr_list);
    return -1;
  }

  printf("Listening on port %s\n", port); // remove
  freeaddrinfo(addr_list);
  return fd;
}

int open_client_socket(const char *server_hostname, const char *server_port) {
  int fd;
  int status;
  struct addrinfo hints;
  struct addrinfo *addr_list, *addr_it;

  memset(&hints, 0, sizeof(hints));
  hints.ai_family = AF_UNSPEC;
  hints.ai_socktype = SOCK_STREAM;
  hints.ai_flags = AI_PASSIVE;

  status = getaddrinfo(server_hostname, server_port, &hints, &addr_list);
  if (status != 0) {
    perror("ERROR: getaddrinfo\n");
    freeaddrinfo(addr_list);
    return -1;
  }

  for (addr_it = addr_list; addr_it != NULL; addr_it = addr_it->ai_next) {
    fd = socket(addr_it->ai_family, addr_it->ai_socktype, addr_it->ai_protocol);
    if (fd == -1) {
      continue;
    }

    int yes = 1;
    if (setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1) {
      perror("ERROR: sersockopt\n");
      freeaddrinfo(addr_list);
      return -1;
    }

    if (connect(fd, addr_it->ai_addr, addr_it->ai_addrlen) != -1) {
      // connect successful
      break;
    }
    // connect failed
    close(fd);
  }

  freeaddrinfo(addr_list);
  return fd;
}

void send_all(int fd, char *buf, size_t size) {
  ssize_t send_size = size;
  while (send_size > 0) {
    ssize_t sent = send(fd, buf, send_size, 0);
    if (sent == -1) {
      perror("Error: sendall\n");
      exit(EXIT_FAILURE);
    }

    buf += sent;
    send_size -= sent;
  }
}

void send_player_port(int listener_fd, int send_to_fd) {
  // hostname~###|port~###|

  struct sockaddr_in listen_addr;
  socklen_t listen_addr_len = sizeof(listen_addr);

  int sockname_status = getsockname(
      listener_fd, (struct sockaddr *)&listen_addr, &listen_addr_len);

  if (sockname_status == -1) {
    perror("getsockname:");
    exit(EXIT_FAILURE);
  }

  size_t len = SHORT_MSG_SIZE;
  char my_hostname[len]; // = NULL;
  if (gethostname(my_hostname, len) != 0) {
    perror("Failed to get host name.\n");
    exit(EXIT_FAILURE);
  }

  if (my_hostname[len - 1] != '\0') {
    printf("hostname longer than %lu characters:%s\n", len, my_hostname);
  }
  printf("hostname:\t%s\n", my_hostname);
  int my_port = ntohs(listen_addr.sin_port);

  printf("Listening on port %d\n", my_port);

  char buf_my_serv_info[SHORT_MSG_SIZE];
  len = SHORT_MSG_SIZE;
  memset(&buf_my_serv_info, 0, len);
  if (snprintf(buf_my_serv_info, len, "hostname~%s|port~%d|", my_hostname,
               my_port) < 0) {
    fprintf(stderr, "building string using snprintf failed.\n");
    exit(EXIT_FAILURE);
  }

  printf("send_player_port:\n%s\n\n", buf_my_serv_info);

  if (send(send_to_fd, buf_my_serv_info, len, 0) == -1) {
    perror("Error: sending player server info:\n");
    exit(EXIT_FAILURE);
  }

  return;
}

void send_player_id_tot(int fd, size_t player_id, size_t num_players) {
  // id~###|tot~###

  size_t len = SHORT_MSG_SIZE;
  char msg_buf[len];
  memset(&msg_buf, 0, len);

  if (snprintf(msg_buf, len, "id~%lu|tot~%lu|", player_id, num_players) < 0) {
    fprintf(stderr, "Error: building id string using snprintf failed.\n");
    exit(EXIT_FAILURE);
  }

  if (send(fd, msg_buf, len, 0) == -1) {
    perror("Error: sending init msg to players\n");
    exit(EXIT_FAILURE);
  }

  return;
}

void get_id_tot(int rm_fd, size_t *id_ptr, size_t *tot_ptr) {
  // id~###|tot~###

  ssize_t recv_status;
  char buffer[SHORT_MSG_SIZE];
  char *arr[2];
  size_t id, tot;

  recv_status = recv(rm_fd, buffer, SHORT_MSG_SIZE, MSG_WAITALL);
  if (recv_status == -1) {
    fprintf(stderr, "Failed to recv data\n");
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
  id = str_to_num(arr[0]);
  tot = str_to_num(arr[1]);
  printf("id:\t%lu\n"
         "tot:\t%lu\n",
         id, tot); // remove

  *id_ptr = id;
  *tot_ptr = tot;
  return;
}

int get_player_host(int player_fd, char *hostname, char *port) {
  // hostname~###|port~###|

  ssize_t recv_status;
  char buffer[SHORT_MSG_SIZE];
  char *arr[2];

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

  size_t num_fields = parse_msgs(buffer, arr, 2);

  if (num_fields == 2) {

    strncpy(hostname, arr[0], SHORT_MSG_SIZE);
    strncpy(port, arr[1], SHORT_MSG_SIZE);

    printf("hostname:\t%s\n"
           "port:\t%s\n",
           hostname, port); // remove
  } else if (num_fields == 1) {
    // check accept
    return 0;
  }

  return 1;
}

void send_right_neigh(int player_fd, player_info_t *r_neigh) {
  // r_hostname~###|r_port~###|

  size_t len = SHORT_MSG_SIZE;
  char msg_buf[len];
  memset(&msg_buf, 0, len);

  if (snprintf(msg_buf, len, "r_hostname~%s|r_port~%s|", r_neigh->hostname,
               r_neigh->port) < 0) {
    fprintf(stderr,
            "Error: right neigh info building using snprintf failed.\n");
    exit(EXIT_FAILURE);
  }

  if (send(player_fd, msg_buf, len, 0) == -1) {
    perror("Error: sending init msg to players\n");
    exit(EXIT_FAILURE);
  }

  return;
}

int get_right_neigh(int rm_fd, char *r_hostname, char *r_port) {
  // r_hostname~###|r_port~###|

  return get_player_host(rm_fd, r_hostname, r_port);
}

void send_signal(int sig, int send_to_fd) {
  // 0 - accept, 1 - ready, 2 - end
  size_t len = (sig == 2) ? POTATO_SIZE : SHORT_MSG_SIZE;
  char msg_buf[len];
  memset(&msg_buf, 0, len);

  switch (sig) {
  case 0:
    if (snprintf(msg_buf, len, "a~accept|") < 0) {
      fprintf(stderr, "Error: snprintf - sig accept\n");
      exit(EXIT_FAILURE);
    }
    break;
  case 1:
    if (snprintf(msg_buf, len, "r~ready|") < 0) {
      fprintf(stderr, "Error: snprintf - sig accept\n");
      exit(EXIT_FAILURE);
    }
    break;
  case 2:
    if (snprintf(msg_buf, len, "e~end|") < 0) {
      fprintf(stderr, "Error: snprintf - sig accept\n");
      exit(EXIT_FAILURE);
    }
    break;
  default:
    fprintf(stderr, "Error: invalid signal to send_signal\n");
    exit(EXIT_FAILURE);
    break;
  }

  send_all(send_to_fd, msg_buf, len);
}

void send_accept_signal(int player_fd) { send_signal(0, player_fd); }

void send_ready_signal(int rm_fd) { send_signal(1, rm_fd); }

// think about END signal once again later
void send_end_signal(int player_fd) { send_signal(2, player_fd); }
/*---------------------end implementations----------------------*/

#endif
