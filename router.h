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





#ifndef OCTOPUS_ROUTER_H_
#define OCTOPUS_ROUTER_H_

#include <stdint.h>
#include <stddef.h>
#include "tree/tree.h"
#include <czmq.h>
#include"hash/khash.h"
#include"MurmurHash/MurmurHash3.h"
#include "hkey.h"
#include "node.h"
#include "nodes.h"


struct node_t;
typedef struct node_t node_t;


struct hash_t
{
    struct _hkey_t hkey;
    struct node_t *node;
      RB_ENTRY (hash_t) field;
};

int cmp_hash_t (struct hash_t *first, struct hash_t *second);


RB_HEAD (hash_rb_t, hash_t);
RB_PROTOTYPE (hash_rb_t, hash_t, field, cmp_hash_t);

struct router_t
{
    int type;                   //1 is db 0 is worker
    struct hash_rb_t hash_rb;
    int repl;                   //replication, used only be the db_routing
    node_t *self;

      khash_t (nodes_t) * nodes;
};


typedef struct router_t router_t;


void router_init (router_t ** router, int type);

void router_destroy (router_t * router);

//I save the key with the null char but I compute the hash without the null
//return 0 if the element already exists and thus the new node is not inserted

int router_add (router_t * router, node_t * node);

//first I need to find the exact same node I want to delete
void router_delete (router_t * router, node_t * node);

//returns the id adrress of the node
//if there are no nodes, returns null
char *router_route (router_t * router, uint64_t key);

void router_set_repl (router_t * router, int repl);

void router_get_repl (struct router_t *router, int *repl);

//rkey contains the addresses of the nodes
//rkey should be big enough and should check repl before
//this will only return alive nodes

//we grab the first repl number of nodes and return only the alive ones
void router_dbroute (struct router_t *router, uint64_t key, char **rkey,
                     int *nreturned);

//finds a node with its  key
//returns null if not found
node_t *router_fnode (struct router_t *router, char *key);



//returns an array of events of a specific size
//if this node already exist it creates events from the difference of their settings
//removal(0,1) is boolean indicating whether this is about removing a node or adding a node
//0 also means that there might be a change in st_piece, n_pieces

zlist_t *router_events (router_t * router, node_t * node, int removal,
                        int *circle);


#endif
