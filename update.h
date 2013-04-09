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




#ifndef _OCTOPUS_UPDATE_H_
#define _OCTOPUS_UPDATE_H_

#include"router.h"

struct update_t
{
    unsigned int id;            //the id of the previous update
    void *dealer;               //used to confirm the updates to the ozookeeper object
    router_t *router;
    router_t *db_router;
    void *router_back;
    void *dealer_back;
};

typedef struct update_t update_t;

void update_init (update_t ** update, void *dealer, router_t * router,
                  router_t * db_router, void *socket);




#endif
