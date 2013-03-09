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





#include"tree/tree.h"
#include"MurmurHash/MurmurHash3.h"
#include <stddef.h>
#include<stdlib.h>
#include<string.h>
#include"router.h"
#include<stdio.h>
#include<assert.h>
#include<czmq.h>
#include"hash/khash.h"


RB_GENERATE (hash_rb_t, hash_t, field, cmp_hash_t);


int
cmp_hash_t (struct hash_t *first, struct hash_t *second)
{

    if (first->hkey.prefix > second->hkey.prefix) {
        return 1;
    }
    else {
        if (first->hkey.prefix < second->hkey.prefix) {
            return -1;
        }
        else {
            if (first->hkey.suffix > second->hkey.suffix) {
                return 1;
            }
            else {
                if (first->hkey.suffix < second->hkey.suffix) {
                    return -1;
                }
                else {
                    return 0;
                }
            }
        }
    }

}


void
router_init (struct router_t **router, int type)
{

    *router = malloc (sizeof (struct router_t));

    (*router)->type = type;
    RB_INIT (&((*router)->hash_rb));
    (*router)->self = NULL;
    (*router)->repl = -1;

    nodes_init (&(*router)->nodes);


}

void
router_destroy (struct router_t *router)
{

    struct hash_t *hash;
//deletion per node
    while ((hash = RB_MIN (hash_rb_t, &(router->hash_rb)))) {
        router_delete (router, hash->node);
    }

    free (router);

}

//I save the key with the null char but I compute the hash without the null
//return 0 if the element already exists and thus the new node is not inserted
int
router_add (struct router_t *router, node_t * node)
{

    char key[1000];

    struct hash_t *hash[node->n_pieces];

    int iter;
    for (iter = 0; iter < node->n_pieces; iter++) {
        node_piece (node->key, iter + node->st_piece, key);


        hash[iter] = malloc (sizeof (struct hash_t));
        hash[iter]->node = node;

        MurmurHash3_x64_128 ((void *) key, strlen (key), 0,
                             (void *) &(hash[iter]->hkey));

        if (RB_INSERT (hash_rb_t, &(router->hash_rb), hash[iter]) != NULL) {
            //delete the previous hashes
            int siter;
            for (siter = 0; siter < iter; siter++) {
                RB_REMOVE (hash_rb_t, &(router->hash_rb), hash[siter]);
                free (hash[siter]);
            }
            return 0;
        }
    }
    nodes_put (router->nodes, node);
    return 1;

}

void
router_delete (struct router_t *router, node_t * node)
{

    if (node != NULL) {
        char key[1000];
        int iter;
        for (iter = 0; iter < node->n_pieces; iter++) {
            node_piece (node->key, iter + node->st_piece, key);

            struct hash_t hash;
            struct hash_t *result;

            MurmurHash3_x64_128 ((void *) key, strlen (key), 0,
                                 (void *) &(hash.hkey));

            result = RB_FIND (hash_rb_t, &(router->hash_rb), &hash);

            if (result != NULL) {
                RB_REMOVE (hash_rb_t, &(router->hash_rb), result);
                free (result);
            }
        }
        nodes_delete (router->nodes, node->key);
        free (node);
    }
}

//rkey should be big enough
char *
router_route (struct router_t *router, uint64_t key)
{

    struct hash_t hash;
    struct hash_t *result;

    MurmurHash3_x64_128 ((void *) &key, sizeof (uint64_t), 0,
                         (void *) &(hash.hkey));

    result = RB_NFIND (hash_rb_t, &(router->hash_rb), &hash);

    if (result == NULL) {
        result = RB_MIN (hash_rb_t, &(router->hash_rb));
    }

    if (result == NULL) {
        return NULL;
    }

    return result->node->key;

}

void
router_set_repl (struct router_t *router, int repl)
{
    router->repl = repl;
}

void
router_get_repl (struct router_t *router, int *repl)
{
    *repl = router->repl;
}


//only used in db_routing
void
node_set_alive (node_t * node, int alive)
{
    node->alive = alive;
}

//rkey the key of the node that is responsible for the key
//rkey should be big enough and should check repl before
//this will only return alive nodes

//we grab the first repl number of nodes and return only the alive ones
void
router_dbroute (struct router_t *router, uint64_t key, char **rkey,
                int *nreturned)
{

    struct hash_t hash;
    struct hash_t *result;
    struct hash_t *first_result;        //used to identify a full circle
    *nreturned = 0;

    MurmurHash3_x64_128 ((void *) &key, sizeof (uint64_t), 0,
                         (void *) &(hash.hkey));

    result = RB_NFIND (hash_rb_t, &(router->hash_rb), &hash);

    if (result == NULL) {
        result = RB_MIN (hash_rb_t, &(router->hash_rb));
    }

    if (result == NULL) {
        *rkey = NULL;
        return;
    }

    int all = 1;
    first_result = result;
//printf("\ndbroute debug key: %s",result->key);
    if (result->node->alive) {
        strcpy (rkey[0], result->node->key);
        (*nreturned)++;
    }
    while (1) {
        struct hash_t *prev_result = result;
        result = RB_NEXT (hash_rb_t, &(router->hash_rb), prev_result);

//go back to the beggining
        if (result == NULL) {
            result = RB_MIN (hash_rb_t, &(router->hash_rb));
        }
//stop at a full circle or we found a repl number of nodes dead or alive
        if ((strcmp (result->node->key, first_result->node->key) == 0)
            || (all == router->repl)) {
            break;
        }

//chech whether we already picked this node
        int iter;
        int neww = 1;
        for (iter = 0; iter < *nreturned; iter++) {
            if (0 == strcmp (rkey[iter], result->node->key)) {
                neww = 0;
                break;
            }
        }

        if (neww) {

            all++;              //all unique dead or alive
            if (result->node->alive) {

                strcpy (rkey[*nreturned], result->node->key);
                (*nreturned)++;
            }
        }
    }


}



//the key contains only the address
node_t *
router_fnode (struct router_t *router, char *key)
{
    return nodes_search (router->nodes, key);
}
