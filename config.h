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




#ifndef OCTOPUS_CONFIG_H_
#define OCTOPUS_CONFIG_H_


//first line is the host
//comma separated host:port pairs, each corresponding to a zk
//server. e.g. "127.0.0.1:3000,127.0.0.1:3001,127.0.0.1:3002"


typedef struct
{

    char line[30][1000];        //all lines have 1000 max char including null
    int n_lines;                //maximum lines are 30

} oconfig_t;

void oconfig_init (oconfig_t ** config);

//name of the octopus
void oconfig_octopus (oconfig_t * config, char *octopus);

void oconfig_host (oconfig_t * config, char *host);
void oconfig_destroy (oconfig_t * config);
void oconfig_recv_timeout (oconfig_t * config, int *timeout);

#endif
