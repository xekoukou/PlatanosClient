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


#ifndef OCTOPUS_NODES_H_
#define OCTOPUS_NODES_H_

#include"hash/khash.h"
#include"node.h"

void nodes_init (khash_t (nodes_t) ** nodes);

void nodes_put (khash_t (nodes_t) * nodes, node_t * node);

//deletes the entry if present
//the node is deleted bu router_delete
void nodes_delete (khash_t (nodes_t) * nodes, char *key);

//returns NULL if not found
node_t *nodes_search (khash_t (nodes_t) * nodes, char *key);



#endif
