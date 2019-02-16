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

void parse_input(int margv, char *margc[], ringmaster_inputs_t *inputs);
size_t str_to_num(char *str);

#endif
