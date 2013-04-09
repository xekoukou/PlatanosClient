/*
    Copyright contributors as noted in the AUTHORS file.
                
    This file is part of PLATANOS.

    PLATANOS is free software; you can redistribute it and/or modify it under
    the terms of the GNU Affero General Public License as published by
    the Free Software Foundation; either version 3 of the License, or
    (at your option) any later version.
            
    PLATANOS is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU Affero General Public License for more details.
        
    You should have received a copy of the GNU Affero General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/


#ifndef OCTOPUS_NODE_H_
#define OCTOPUS_NODE_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include"hash/khash.h"
#include"api/platanos_client.h"



struct node_t
{
    unsigned long st_piece;
    int n_pieces;
    char key[18];              //key is the routing_address or the subscription subject that we will accept from this node.
    int alive;                  //this is only used in a db_routing, all worker nodes received 
    //are assumed alive. The reason for this is that db nodes need 
    //to remain the same for each vertex despite the failures
    // so that the vertex can know which db to fix in case of failures

    platanos_node_t *platanos_node;
    char bind_point_db[50];
    char bind_point_bl[50];

};



typedef struct node_t node_t;


//result should be big enough
void node_piece (char *key, unsigned long pnumber, char *result);

void node_init (node_t ** node, char *key, int n_pieces,
                unsigned long st_piece,
                platanos_node_t *platanos_node);

void
db_node_init (node_t ** node, char *key, int n_pieces,
              unsigned long st_piece, char *bind_point_db);

void node_destroy (node_t ** node);

node_t *node_dup (node_t * node);

// only used in a db_routing 
void node_set_alive (node_t * node, int alive);


#endif
