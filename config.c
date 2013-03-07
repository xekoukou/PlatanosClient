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





#include<stdio.h>
#include<stdlib.h>
#include"config.h"
#include<string.h>
#include<assert.h>

// the configuration file is always at the location that the program starts

void
oconfig_init (oconfig_t ** config)
{


    *config = malloc (sizeof (oconfig_t));

//Open the configuration file
    FILE *fconfig = fopen ("./config", "r");
    if (fconfig == NULL) {
        printf
            ("\nconfig doesnt exist. Are you in the correnct directory? A config file must be created manually for every computer that is part of the octopus.. exiting");
        exit (1);
    }

    int line_position = 0;
    while ((fgets ((*config)->line[line_position], 1000, fconfig))
           && (line_position < 30)) {
        (*config)->line[line_position][strlen ((*config)->line[line_position])
                                       - 1] = '\0';



        line_position++;
        (*config)->n_lines = line_position;
    }
    fclose (fconfig);

}


void
oconfig_octopus (oconfig_t * config, char *octopus)
{
    strcpy (octopus, config->line[2]);
}


void
oconfig_host (oconfig_t * config, char *host)
{
    strcpy (host, config->line[0]);
}

void
oconfig_recv_timeout (oconfig_t * config, int *timeout)
{
    *timeout = atoi (config->line[1]);
}


void
oconfig_destroy (oconfig_t * config)
{

    free (config);

}
