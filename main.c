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





#include<czmq.h>
#include"config.h"
#include"zookeeper.h"
#include"broker.h"




int
main ()
{

    zctx_t *ctx = zctx_new ();


//Open the configuration file
    oconfig_t *config;
    oconfig_init (&config);


    ozookeeper_t *ozookeeper;

    ozookeeper_init (&ozookeeper, config, ctx);

    ozookeeper_getconfig (ozookeeper);

    broker_fn (NULL);


    return 0;
}
