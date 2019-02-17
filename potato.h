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

typedef struct _potato_t {
  size_t num_hops;
  size_t players[];
  // maybe unnecessary
} potato_t;

typedef struct _ringmaster_inputs_t {
  size_t port_num;
  size_t num_players;
  size_t num_hops;
} ringmaster_inputs_t;

typedef struct _player_inputs_t {
  char *machine_name;
  size_t port_num;
} player_inputs_t;

void parse_rm_input(int margv, char *margc[], ringmaster_inputs_t *inputs);
void parse_p_inputs(int margv, char *margc[], player_inputs_t *inputs);
size_t str_to_num(char *str);

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
