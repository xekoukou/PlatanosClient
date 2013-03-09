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

#ifndef _OCTOPUS_ZK_UPDATER_H_
#define _OCTOPUS_ZK_UPDATER_H_

#include<zookeeper/zookeeper.h>
#include<string.h>
#include<assert.h>
#include<czmq.h>


struct oz_updater_t
{
    unsigned int id;            //used to id updates
    char *key;                  //computer + res_name that is also the id of all communication to the worker 
//contains null

//these are used to find the changes that happened to the children
    struct String_vector computers;
    struct String_vector *w_resources;
    int **w_online;             //this is used tocheck whether a previous watch event has set a resource
//offline (used by w_st_piece ,w_n_pieces)

    struct String_vector *db_resources;
    int **db_online;

};

typedef struct oz_updater_t oz_updater_t;

// doesnt allocate memory
void oz_updater_init (oz_updater_t * updater);

void oz_updater_destroy (oz_updater_t * updater);

void oz_updater_key (oz_updater_t * updater, char *key);

void oz_updater_free_key (oz_updater_t * updater);

//returns the location where the resource is ex.(5, 17)
//if it doesnt exist you get (-1,something) or (something,-1)
void
oz_updater_search (oz_updater_t * updater, int db, char *comp_name,
                   char *res_name, int *m, int *n);


//returns sort which is used to set watches only on the new computers
//sort must be deallocated

int *oz_updater_new_computers (oz_updater_t * updater,
                               struct String_vector computers);

//returns sort which is used to set watches only on the new resources
//sort must be deallocated

int *oz_updater_new_resources (oz_updater_t * updater, char *comp_name,
                               struct String_vector resources, int db,
                               zlist_t * db_old);

#endif
