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


#include "zk_updater.h"

// doesnt allocate memory
void
oz_updater_init (oz_updater_t * updater)
{
    updater->id = 1;
    //allocate_String_vector (&(updater->computers));
    updater->computers.count = 0;
    updater->computers.data = 0;
    updater->w_resources = NULL;
    updater->w_online = NULL;
    updater->db_resources = NULL;
    updater->db_online = NULL;
    updater->key = NULL;
}

// just creates a path
void
oz_updater_key (oz_updater_t * updater, char *key)
{
    updater->key = malloc (strlen (key) + 1);
    strcpy (updater->key, key);
}

void
oz_updater_free_key (oz_updater_t * updater)
{
    if (updater->key != NULL) {
        free (updater->key);
    }
}


void
oz_updater_destroy (oz_updater_t * updater)
{
    free (updater->key);
    int iter;

    for (iter = 0; iter < updater->computers.count; iter++) {
        deallocate_String_vector (&(updater->w_resources[iter]));
        deallocate_String_vector (&(updater->db_resources[iter]));
        free (updater->w_online[iter]);
        free (updater->db_online[iter]);
    }
    free (updater->w_resources);
    free (updater->db_resources);
    free (updater->db_online);
    free (updater->w_online);
}


//returns the location where the resource is ex.(5, 17)
//if it doesnt exist you get (-1,something) or (something,-1)
void
oz_updater_search (oz_updater_t * updater, int db, char *comp_name,
                   char *res_name, int *m, int *n)
{
    *m = -1;
    *n = -1;

    int iter;
    for (iter = 0; iter < updater->computers.count; iter++) {
        if (strcmp (updater->computers.data[iter], comp_name) == 0) {
            *m = iter;
            break;
        }
    }
    if (db) {
        for (iter = 0; iter < updater->db_resources[*m].count; iter++) {
            if (strcmp (updater->db_resources[*m].data[iter], res_name) == 0) {
                *n = iter;
                break;
            }
        }
    }
    else {
        for (iter = 0; iter < updater->w_resources[*m].count; iter++) {
            if (strcmp (updater->w_resources[*m].data[iter], res_name) == 0) {
                *n = iter;
                break;
            }
        }
    }

}

//returns sort which is used to set watches only on the new computers
//sort must be deallocated

int *
oz_updater_new_computers (oz_updater_t * updater,
                          struct String_vector computers)
{

    int iter;
    int siter;

//i use this in case there is a reordering of the existing children
    int *sort = malloc (sizeof (int) * computers.count);
//array to free the previous resources
//TODO calloc puts things to zero?
    int size = updater->computers.count;
    int *array = (int *) calloc (updater->computers.count,
                                 sizeof (int)
        );

//set watches to new computers
    for (iter = 0; iter < computers.count; iter++) {
        int exists = 0;
        for (siter = 0; siter < updater->computers.count; siter++) {
            if (strcmp
                (computers.data[iter], updater->computers.data[siter]) == 0) {
                exists = 1;
                break;
            }
        }
        if (exists) {
            sort[iter] = siter;
            array[siter] = 1;
        }
        else {
            sort[iter] = -1;
        }
    }
    deallocate_String_vector (&(updater->computers));

//set the new computer list
    memcpy (&(updater->computers), &computers, sizeof (struct String_vector));


//I allocate the new resources and set watches to new computer's resources
    struct String_vector *new_w_resources =
        malloc (sizeof (struct String_vector)
                * computers.count);

    int **new_w_online_matrix = malloc (sizeof (int *) * computers.count);

    struct String_vector *new_db_resources =
        malloc (sizeof (struct String_vector)
                * computers.count);

    int **new_db_online_matrix = malloc (sizeof (int *) * computers.count);


//obtain the previous data
    for (iter = 0; iter < computers.count; iter++) {
        if (sort[iter] != -1) {

            memcpy (&(new_w_resources[iter]),
                    &(updater->w_resources[sort[iter]]),
                    sizeof (struct String_vector));
            new_w_online_matrix[iter] = updater->w_online[sort[iter]];

            memcpy (&(new_db_resources[iter]),
                    &(updater->db_resources[sort[iter]]),
                    sizeof (struct String_vector));
            new_db_online_matrix[iter] = updater->db_online[sort[iter]];


        }
        else {
//initialize the vector
            new_w_resources[iter].count = 0;
            new_w_resources[iter].data = 0;
            new_w_online_matrix[iter] = NULL;

            new_db_resources[iter].count = 0;
            new_db_resources[iter].data = 0;
            new_db_online_matrix[iter] = NULL;


        }
    }

//free the previous memory and update the pointer to the new memory

    for (siter = 0; siter < size; siter++) {
        if (array[siter] == 0) {
            deallocate_String_vector (&(updater->w_resources[siter]));
            free (updater->w_online[siter]);

            deallocate_String_vector (&(updater->db_resources[siter]));
            free (updater->db_online[siter]);

        }

    }
    if (updater->w_resources != NULL) {
        free (updater->w_resources);
        assert (updater->w_online != NULL);
        assert (updater->db_resources != NULL);
        assert (updater->db_online != NULL);
        free (updater->w_online);

        free (updater->db_resources);
        free (updater->db_online);

    }
    updater->w_resources = new_w_resources;
    updater->w_online = new_w_online_matrix;

    updater->db_resources = new_db_resources;
    updater->db_online = new_db_online_matrix;


    free (array);

    return sort;


}

//returns sort which is used to set watches only on the new resources
//and inform whether a db has unregistered
//sort must be deallocated

int *
oz_updater_new_resources (oz_updater_t * updater, char *comp_name,
                          struct String_vector resources, int db,
                          zlist_t * db_old)
{

    int iter;
    int siter;

//update resources
//find its location
//at least one computer should exist that has the same name
    int position = -1;
    for (iter = 0; iter < updater->computers.count; iter++) {
        if (strcmp (updater->computers.data[iter], comp_name) == 0) {
            position = iter;
            break;
        }
    }
    assert (position != -1);



    struct String_vector *old_resources;
    int **old_online;

    if (db) {
        old_resources = &(updater->db_resources[position]);
        old_online = updater->db_online;
    }
    else {
        old_resources = &(updater->w_resources[position]);
        old_online = updater->w_online;

    }

//i use this in case there is a reordering of the existing children
    int *sort = malloc (sizeof (int) * resources.count);
//update the online vector
    int *online_vector = (int *) calloc (resources.count, sizeof (int));

//set watches to new resources
    zlist_autofree (db_old);


    for (iter = 0; iter < resources.count; iter++) {
        int exists = 0;
        for (siter = 0; siter < old_resources->count; siter++) {
            if (strcmp (resources.data[iter], old_resources->data[siter]) == 0) {
                exists = 1;
            }
        }
        if (exists) {
            sort[iter] = siter;
            online_vector[iter] = old_online[position][siter];
        }
        else {
            sort[iter] = -1;
            if (db) {
                zlist_append (db_old, old_resources->data[siter]);
            }
        }
    }

    deallocate_String_vector (old_resources);


    memcpy (old_resources, &resources, sizeof (struct String_vector));
    if (old_online[position] != NULL) {
        free (old_online[position]);
    }
    old_online[position] = online_vector;


    return sort;

}
