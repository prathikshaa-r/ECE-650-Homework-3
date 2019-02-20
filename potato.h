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

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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

void parse_rm_input(int margv, char *margc[], ringmaster_inputs_t *inputs);
void parse_p_inputs(int margv, char *margc[], player_inputs_t *inputs);
size_t str_to_num(char *str);

void parse_msgs(char *msg, char *results[], size_t num_fields);

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

// open a socket connection that listens -- server side
int server_socket(char *server_IP_addr,
                  size_t server_port_num); // return socket fd
int client_socket(char *server_IP_addr, size_t server_port_num);

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

#endif
