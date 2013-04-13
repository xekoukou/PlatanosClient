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

#include"broker.h"

//TODO this is arbitrary
#define ONGOING_TIMEOUT 4000
#define COUNTER_SIZE 1000       /* 1000 vertices per chunk */

#define NEW_INTERVAL "\001"
#define NEW_CHUNK    "\002"
#define INTERVAL_RECEIVED "\003"
#define CONFIRM_CHUNK    "\004"
#define MISSED_CHUNKES    "\005"

void
add_node (update_t * update, zmsg_t * msg, int db)
{
    int start;
    node_t *node;
    char key[100];
    int n_pieces;
    unsigned long st_piece;
    char bind_point[50];

    zframe_t *frame = zmsg_first (msg);
    memcpy (&start, zframe_data (frame), zframe_size (frame));
    frame = zmsg_next (msg);
    memcpy (key, zframe_data (frame), zframe_size (frame));
    frame = zmsg_next (msg);
    memcpy (&n_pieces, zframe_data (frame), zframe_size (frame));
    frame = zmsg_next (msg);
    memcpy (&st_piece, zframe_data (frame), zframe_size (frame));


    if (db) {
        frame = zmsg_next (msg);
        memcpy (bind_point, zframe_data (frame), zframe_size (frame));

        zmsg_destroy (&msg);

//connect to the node

        int rc;
        rc = zsocket_connect (update->router_back, "%s", bind_point);
        rc = zsocket_connect (update->dealer_back, "%s", bind_point);
        assert (rc == 0);



        fprintf (stderr,
                 "\nbroker_db_add_node\nstart:%d\nkey:%s\nn_pieces:%d\nst_piece:%lu",
                 start, key, n_pieces, st_piece);

        db_node_init (&node, key, n_pieces, st_piece, bind_point);
        node_set_alive (node, 1);


//update router object
//this should always happen after the prev step
        assert (1 == router_add (update->db_router, node));
    }
    else {

//connect to the node

        platanos_node_t *platanos_node = platanos_client_connect (update, msg);


        fprintf (stderr,
                 "\nbroker_add_node\nstart:%d\nkey:%s\nn_pieces:%d\nst_piece:%lu",
                 start, key, n_pieces, st_piece);

        node_init (&node, key, n_pieces, st_piece, platanos_node);

//update router object
//this should always happen after the prev step
        assert (1 == router_add (update->router, node));

    }
}


void
add_dead_node (update_t * update, zmsg_t * msg)
{
    node_t *node;
    char key[100];
    int n_pieces;
    unsigned long st_piece;
    char bind_point[50];

    zframe_t *frame = zmsg_first (msg);
    memcpy (key, zframe_data (frame), zframe_size (frame));
    frame = zmsg_next (msg);
    memcpy (&n_pieces, zframe_data (frame), zframe_size (frame));
    frame = zmsg_next (msg);
    memcpy (&st_piece, zframe_data (frame), zframe_size (frame));


    frame = zmsg_next (msg);
    memcpy (bind_point, zframe_data (frame), zframe_size (frame));

    zmsg_destroy (&msg);

//TODO disconnect to the node

    fprintf (stderr,
             "\nbroker_db_add_dead_node\nkey:%s\nn_pieces:%d\nst_piece:%lu",
             key, n_pieces, st_piece);

    if (node != NULL) {
        node_set_alive (node, 0);
    }
    else {

        db_node_init (&node, key, n_pieces, st_piece, bind_point);
        node_set_alive (node, 0);
//update router object
        assert (1 == router_add (update->db_router, node));
    }

}


void
update_st_piece (update_t * update, zmsg_t * msg, int db)
{
    router_t *router;
    if (!db) {
        router = update->router;
    }
    else {
        router = update->db_router;
    }
    node_t *node;
    char key[100];
    unsigned long st_piece;

    zframe_t *frame = zmsg_first (msg);

    memcpy (key, zframe_data (frame), zframe_size (frame));
    frame = zmsg_next (msg);
    memcpy (&st_piece, zframe_data (frame), zframe_size (frame));

    zmsg_destroy (&msg);
    node_t *prev_node;
    node = node_dup (prev_node = nodes_search (router->nodes, key));
    assert (prev_node != NULL);
    node->st_piece = st_piece;

//update router object
//this should always happen after the prev step

    router_delete (router, prev_node);
    router_add (router, node);

}

void
update_n_pieces (update_t * update, zmsg_t * msg, int db)
{
    router_t *router;
    if (!db) {
        router = update->router;
    }
    else {
        router = update->db_router;
    }

    node_t *node;
    char key[100];
    int n_pieces;

    zframe_t *frame = zmsg_first (msg);

    memcpy (key, zframe_data (frame), zframe_size (frame));
    frame = zmsg_next (msg);
    memcpy (&n_pieces, zframe_data (frame), zframe_size (frame));

    zmsg_destroy (&msg);
    node_t *prev_node;
    node = node_dup (prev_node = nodes_search (router->nodes, key));
    assert (prev_node != NULL);
    node->n_pieces = n_pieces;

//update router object
//this should always happen after the prev step

    router_delete (router, prev_node);
    router_add (router, node);
}


void
remove_node (update_t * update, zmsg_t * msg)
{
    router_t *router;
    router = update->router;

    node_t *node;
    char key[100];

    zframe_t *frame = zmsg_first (msg);

    memcpy (key, zframe_data (frame), zframe_size (frame));

    zmsg_destroy (&msg);

    node = nodes_search (router->nodes, key);

    assert (node != NULL);

    node_set_alive (node, 0);


}


void
db_delete_node (update_t * update, zmsg_t * msg)
{
    router_t *router;
    router = update->db_router;

    node_t *node;
    char key[100];

    zframe_t *frame = zmsg_first (msg);

    memcpy (key, zframe_data (frame), zframe_size (frame));

    zmsg_destroy (&msg);

    node = nodes_search (router->nodes, key);

    assert (node != NULL);

//update router object
//this should always happen after the prev step
    router_delete (router, node);


}

int
broker_update (update_t * update, void *sub)
{

//check if it is a new update or an old one
    zmsg_t *msg = zmsg_recv (sub);
    if (!msg) {
        exit (1);
    }

    int db = 0;

    fprintf (stderr, "\nbroker_update:I have received a sub msg");
    zframe_t *db_frame = zmsg_pop (msg);
    if (strcmp ("db", (char *) zframe_data (db_frame)) == 0) {
        db = 1;
    }

    zframe_destroy (&db_frame);

    zframe_t *id = zmsg_pop (msg);
    if (memcmp (zframe_data (id), &(update->id), sizeof (unsigned int)) == 0) {
//lazy pirate reconfirm update
        zframe_send (&id, update->dealer, 0);
        zframe_destroy (&id);
        zmsg_destroy (&msg);
        fprintf (stderr,
                 "\nbroker_update:It was a previous update, resending confirmation");

    }
    else {
        zframe_t *frame = zmsg_pop (msg);
        if (memcmp
            (zframe_data (frame), "remove_node", zframe_size (frame)) == 0) {
            remove_node (update, msg);
        }
        else {
            if (memcmp
                (zframe_data (frame), "add_dead_node",
                 zframe_size (frame)) == 0) {
                add_dead_node (update, msg);
            }

            else {
                if (memcmp
                    (zframe_data (frame), "add_node",
                     zframe_size (frame)) == 0) {
                    add_node (update, msg, db);
                }
                else {
                    if (memcmp
                        (zframe_data (frame), "st_piece",
                         zframe_size (frame)) == 0) {
                        update_st_piece (update, msg, db);
                    }
                    else {
                        if (memcmp
                            (zframe_data (frame), "n_pieces",
                             zframe_size (frame)) == 0) {
                            update_n_pieces (update, msg, db);
                        }
                        else {

                            if (memcmp
                                (zframe_data (frame), "delete_node",
                                 zframe_size (frame)) == 0) {
                                db_delete_node (update, msg);
                            }

                        }
                    }

                }
            }
        }


        zframe_destroy (&frame);


        zframe_send (&id, update->dealer, 0);
        fprintf (stderr, "\nbroker_update:I have sent confirmation to sub msg");

    }

    return 0;
}



void *
broker_fn (void *arg)
{
    zctx_t *ctx = zctx_new ();


    int rc;
//update infrastructure
    void *sub = zsocket_new (ctx, ZMQ_SUB);
    void *dealer = zsocket_new (ctx, ZMQ_DEALER);


    zmq_setsockopt (sub, ZMQ_SUBSCRIBE, "w", strlen ("w") + 1);
    zmq_setsockopt (sub, ZMQ_SUBSCRIBE, "db", strlen ("db") + 1);


    rc = zsocket_connect (sub, "tcp://127.0.0.1:49152");
    assert (rc == 0);

    rc = zsocket_connect (dealer, "tcp://127.0.0.1:49153");
    assert (rc == 0);




//client infrastruct

    void *router_back = zsocket_new (ctx, ZMQ_ROUTER);
    void *router_front = zsocket_new (ctx, ZMQ_ROUTER);
    void *dealer_back = zsocket_new (ctx, ZMQ_DEALER);
    void *dealer_front = zsocket_new (ctx, ZMQ_DEALER);


//router object
//used to find where each msg goes
    router_t *router;

    router_init (&router, 0);

//router object
//used to find where each msg goes
    router_t *db_router;

    router_init (&db_router, 1);

//update object
//used to update things, like the router object
    update_t *update;


    update_init (&update, dealer, router, db_router, router_back, dealer_back);

    zmq_pollitem_t pollitems[3] = { {sub, 0, ZMQ_POLLIN}
    ,
    {dealer_back, 0,
     ZMQ_POLLIN},
    {dealer_front, 0, ZMQ_POLLIN}
    };

//main loop
    while (1) {
        rc = zmq_poll (pollitems, 3, -1);
        assert (rc != -1);


        if (pollitems[0].revents & ZMQ_POLLIN) {
            broker_update (update, sub);
        }
        if (pollitems[1].revents & ZMQ_POLLIN) {
            zmsg_t *msg = zmsg_recv (dealer_back);
            zmsg_send (&msg, router_front);
        }
        if (pollitems[2].revents & ZMQ_POLLIN) {
            zmsg_t *msg = zmsg_recv (dealer_front);
            zmsg_send (&msg, router_back);

        }
    }
}
