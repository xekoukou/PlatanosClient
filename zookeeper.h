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




#ifndef _OCTOPUS_ZOOKEEPER_H_
#define _OCTOPUS_ZOOKEEPER_H_

#include<zookeeper/zookeeper.h>
#include"config.h"
#include"zk_common.h"
#include"zk_updater.h"

#define _LL_CAST_ (long long)


struct ozookeeper_t
{
    zhandle_t *zh;
    oconfig_t *config;
    void *pub;
    void *router;
    oz_updater_t updater;
};

typedef struct ozookeeper_t ozookeeper_t;



//initialize the ozookeeper object
void
ozookeeper_init (ozookeeper_t ** ozookeeper, oconfig_t * config, zctx_t * ctx);


int ozookeeper_not_corrupt (ozookeeper_t ** ozookeep);


void ozookeeper_getconfig (ozookeeper_t * ozookeeper);

void ozookeeper_set_zhandle (ozookeeper_t * ozookeeper, zhandle_t * zh);

void ozookeeper_zhandle (ozookeeper_t * ozookeeper, zhandle_t ** zh);

void ozookeeper_destroy (ozookeeper_t * ozookeeper);

void global_watcher (zhandle_t * zzh, int type, int state, const char *path,
                     void *context);

#endif
