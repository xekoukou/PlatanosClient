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

#include<string.h>

//returned pointer to the same memory
const char *
last_path (const char *path)
{

    int size = strlen (path);
    int iter;
    for (iter = size - 1; iter >= 0; iter--) {
        if (path[iter] == '/') {
            return &(path[iter + 1]);
        }
    }
    return NULL;

}

// the result , ie start doesnt have a null at the end
//location 1 means first from the last
void
part_path (char *path, int location, char **start, int *siz)
{

    int st = strlen (path);
    int end;
    int en = 0;

    int size = strlen (path);
    int iter;
    for (iter = size - 1; iter >= 0; iter--) {
        if (path[iter] == '/') {
            end = st;
            st = iter;
            en++;
        }
        if (en == location) {

            break;
        }
    }

    *start = &(path[st + 1]);
    *siz = -st + end - 1;
}
