#include <string.h>

#include "client-connect.h"

int
connect_client(const struct per_vhost_data__minimal *vhd, const int port, const char *address, const char *path, void *userdata) {
    struct lws_client_connect_info i;

    memset(&i, 0, sizeof(i));
    i.context = vhd->context;
    i.port = port;
    i.address = address;
    i.path = path;
    i.host = address;
    i.origin = address;
    i.ssl_connection = LCCSCF_USE_SSL;
    i.protocol = "default";
    i.local_protocol_name = "default";
    i.pwsi = (struct lws **) (&vhd->client_wsi);
    i.userdata = userdata;

    return !lws_client_connect_via_info(&i);
}