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



#ifndef OCTOPUS_BROKER_H_
#define OCTOPUS_BROKER_H_

#include"on_give.h"
#include"on_receive.h"
#include"update.h"
#include"balance.h"
#include"events.h"
#include"nodes.h"
#include"actions.h"
#include"zookeeper.h"
#include"sleep.h"
#include"compute.h"




//TODO I havent yet provided a destructor function
struct broker_t
{
    zhandle_t *zh;
    oconfig_t *config;
};

typedef struct broker_t broker_t;

void broker_init (broker_t ** broker, zhandle_t * zh, oconfig_t * config,);

void *broker_fn (void *arg);


#endif
