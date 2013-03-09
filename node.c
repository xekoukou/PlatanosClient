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


#include"node.h"
//result should be big enough
void
node_piece (char *key, unsigned long pnumber, char *result)
{
    sprintf (result, "%s%lu", key, pnumber);
}


void
node_init (node_t ** node, char *key, int n_pieces,
           unsigned long st_piece, char *bind_point_nb, char *bind_point_wb)
{

    *node = malloc (sizeof (node_t));
    strcpy ((*node)->key, key);
    (*node)->n_pieces = n_pieces;
    (*node)->st_piece = st_piece;
    strcpy ((*node)->bind_point_nb, bind_point_nb);
    strcpy ((*node)->bind_point_wb, bind_point_wb);

}

void
db_node_init (node_t ** node, char *key, int n_pieces,
              unsigned long st_piece, char *bind_point_db)
{

    *node = malloc (sizeof (node_t));
    strcpy ((*node)->key, key);
    (*node)->n_pieces = n_pieces;
    (*node)->st_piece = st_piece;
    strcpy ((*node)->bind_point_nb, bind_point_db);

}


node_t *
node_dup (node_t * node)
{

    node_t *new_node = malloc (sizeof (node_t));
    memcpy (new_node, node, sizeof (node_t));
    return new_node;
}
