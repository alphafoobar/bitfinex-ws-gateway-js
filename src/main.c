#include <stdio.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <signal.h>

#include "cjson/cJSON.h"
#include "ws/client-server-protocol.h"

static int interrupted = 0;
static int opts = 0;

static const struct lws_http_mount mount = {
        /* .mount_next */           NULL,        /* linked-list "next" */
        /* .mountpoint */           "/",        /* mountpoint URL */
        /* .origin */               "./mount-origin", /* serve from dir */
        /* .def */                  "index.html",    /* default filename */
        /* .protocol */             NULL,
        /* .cgienv */               NULL,
        /* .extra_mimetypes */      NULL,
        /* .interpret */            NULL,
        /* .cgi_timeout */          0,
        /* .cache_max_age */        0,
        /* .auth_mask */            0,
        /* .cache_reusable */       0,
        /* .cache_revalidate */     0,
        /* .cache_intermediaries */ 0,
        /* .origin_protocol */      LWSMPRO_FILE,    /* files in a dir */
        /* .mountpoint_len */       1,        /* char count */
        /* .basic_auth_login_file */NULL,
        /* ._unused */              {}
};

static const struct lws_protocols client_protocols[] = {
        { "http", lws_callback_http_dummy, 0, 0, 0, NULL, 0 },
    LWS_PLUGIN_PROTOCOL_MINIMAL,
    {NULL, NULL, 0, 0, 0, NULL, 0}
};

void sigint_handler(int sig){
    lwsl_user("interrupt received signal=%d\n", sig);
    interrupted = 1;
}

int main() {
    struct lws_context_creation_info info;
    struct lws_context *context;
    int n = 0, logs = LLL_USER | LLL_ERR | LLL_WARN | LLL_NOTICE
    /* for LLL_ verbosity above NOTICE to be built into lws,
     * lws must have been configured and built with
     * -DCMAKE_BUILD_TYPE=DEBUG instead of =RELEASE */
    /* | LLL_INFO */ /* | LLL_PARSER */ /* | LLL_HEADER */
    /* | LLL_EXT */ /* | LLL_CLIENT */ /* | LLL_LATENCY */
    /* | LLL_DEBUG */;

    signal(SIGINT, sigint_handler);

    lws_set_log_level(logs, NULL);

    memset(&info, 0, sizeof info); /* otherwise uninitialized garbage */
    info.port = 7681; /* we do not run any server */
    info.protocols = client_protocols;
    info.vhost_name = "localhost";
    info.mounts = &mount;
    info.error_document_404 = "/404.html";
    info.ws_ping_pong_interval = 60;
    info.options = opts | LWS_SERVER_OPTION_DO_SSL_GLOBAL_INIT;

    context = lws_create_context(&info);
    if (!context) {
        lwsl_err("lws init failed\n");
        return 1;
    }

    lwsl_user("bitfinex-c-gateway websocket started on port %d！\n", info.port);
    while (n >= 0 && !interrupted) {
        n = lws_service(context, 0);
    }

    lws_context_destroy(context);

    lwsl_user("Fin.\n");
    return 0;
}
