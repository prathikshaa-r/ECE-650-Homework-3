/**
 * potato.h
 *
 * Prathikshaa
 * February 14, 2019
 *
 * Attempt-2
 *
 */

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
