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

#define LISTEN_BACKLOG 100

#include <assert.h>
#include <errno.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
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
  int fd;
  char *hostname;
  char *port;
  struct _player_info_t *left;
  struct _player_info_t *right;
} player_info_t;

/*----------------server and client socket------------------*/
// open a socket connection that listens -- server side
// implemented in potato.h
// return socket fd

int open_server_socket(char *hostname, char *port);

// open a socket connection to connnect to specified server
// implemented in potato.h

int open_client_socket(char *server_hostname, char *server_port);
/*-----------------------------------------------------------*/

/*-----------------------parsing functions-------------------*/
// ringmaster inputs
void parse_rm_input(int margv, char *margc[], ringmaster_inputs_t *inputs);

// player inputs
void parse_p_inputs(int margv, char *margc[], player_inputs_t *inputs);

// string to number conversion function -- implemented in potato.h
size_t str_to_num(char *str);

// parse messsages from ringmaster -- implemented in potato.h
void parse_msgs(char *msg, char *results[], size_t num_fields);
/*-------------------------------------------------------------*/

/*----------------Function Implementations---------------------*/
void parse_msgs(char *msg, char *results[], size_t num_fields) {
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
    // parse field to get value using ':'
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
  return;
}

// todo: is the parse_input function vulnerable to buffer overflow due to
// dynamic memory alloc?
size_t str_to_num(char *str) {
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

int open_server_socket(char *hostname, char *port) {
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
    return -1;
  }

  // listen
  if (listen(fd, LISTEN_BACKLOG) == -1) {
    perror("Error: cannot listen on socket\n");
    return -1;
  }

  printf("Listening on port %s\n", port); // remove
  return fd;
}

int open_client_socket(char *server_hostname, char *server_port) {
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
      return -1;
    }

    if (connect(fd, addr_it->ai_addr, addr_it->ai_addrlen) != -1) {
      // connect successful
      break;
    }
    // connect failed
    close(fd);
  }

  return fd;
}
/*---------------------end implementations----------------------*/

#endif
