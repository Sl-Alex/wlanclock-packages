#include <pthread.h>
#include <libubus.h>

static struct ubus_context *ctx;
static struct ubus_request_data req_data;
static struct blob_buf  bb;

void ubus_server_deinit();

static int wlanclock_data_handler(
        struct ubus_context *ctx, struct ubus_object *obj,
        struct ubus_request_data *req, const char *method,
        struct blob_attr *msg );

pthread_t   ubus_server_thread;

volatile static int ubus_server_running = 0;
int ubus_server_is_running()
{
    return ubus_server_running;
}

/* Enum for EGI policy order */
enum {
    WLANCLOCK_DATA_GESTURE,
    WLANCLOCK_DATA_BRIGHTNESS,
    WLANCLOCK_DATA_PRESSURE,
    WLANCLOCK_DATA_HUMIDITY,
    WLANCLOCK_DATA_TEMPERATURE,
    __WLANCLOCK_DATA_MAX,
};

/* Ubus Policy */
static const struct blobmsg_policy wlanclock_data_policy[] =
{
    [WLANCLOCK_DATA_GESTURE]     = { .name="gesture",     .type=BLOBMSG_TYPE_INT32  },
    [WLANCLOCK_DATA_BRIGHTNESS]  = { .name="brightness",  .type=BLOBMSG_TYPE_INT32  },
    [WLANCLOCK_DATA_PRESSURE]    = { .name="pressure",    .type=BLOBMSG_TYPE_DOUBLE },
    [WLANCLOCK_DATA_HUMIDITY]    = { .name="humidity",    .type=BLOBMSG_TYPE_DOUBLE },
    [WLANCLOCK_DATA_TEMPERATURE] = { .name="temperature", .type=BLOBMSG_TYPE_DOUBLE },
};

/* Ubus Methods */
static const struct ubus_method wlanclock_data_methods[] =
{
    UBUS_METHOD("data", wlanclock_data_handler, wlanclock_data_policy),
};

/* Ubus object type */
static struct ubus_object_type wlanclock_data_obj_type =
    UBUS_OBJECT_TYPE("wlanclock_uobj", wlanclock_data_methods);

/* Ubus object */
static struct ubus_object wlanclock_data_obj =
{
    .name = "wlanclock.app",
    .type = &wlanclock_data_obj_type,
    .methods = wlanclock_data_methods,
    .n_methods = ARRAY_SIZE(wlanclock_data_methods),
    //.path= /* useless */
};

/*---------------------------------------------
A callback function for ubus methods handling
----------------------------------------------*/
static int wlanclock_data_handler( struct ubus_context *ctx, struct ubus_object *obj,
              struct ubus_request_data *req, const char *method,
              struct blob_attr *msg )
{
    struct blob_attr *tb[__WLANCLOCK_DATA_MAX]; /* for parsed attr */

    /* Parse blob_msg from the caller to request policy */
    blobmsg_parse(wlanclock_data_policy, ARRAY_SIZE(wlanclock_data_policy), tb, blob_data(msg), blob_len(msg));

    /* print request msg */
    printf("Receive msg from caller:\n");
    if(tb[WLANCLOCK_DATA_GESTURE])
         printf("   Gesture     = %u\n",   blobmsg_get_u32   (tb[WLANCLOCK_DATA_GESTURE]));
    if(tb[WLANCLOCK_DATA_BRIGHTNESS])
         printf("   Brightness  = %u\n",   blobmsg_get_u32   (tb[WLANCLOCK_DATA_BRIGHTNESS]));
    if(tb[WLANCLOCK_DATA_PRESSURE])
         printf("   Pressure    = %.2f\n", blobmsg_get_double(tb[WLANCLOCK_DATA_PRESSURE]));
    if(tb[WLANCLOCK_DATA_HUMIDITY])
         printf("   Humidity    = %.2f\n", blobmsg_get_double(tb[WLANCLOCK_DATA_HUMIDITY]));
    if(tb[WLANCLOCK_DATA_TEMPERATURE])
         printf("   Temperature = %.2f\n", blobmsg_get_double(tb[WLANCLOCK_DATA_TEMPERATURE]));

    /* Do some job here according to caller's request */

    /* send a reply msg to the caller for information */
    blob_buf_init(&bb, 0);
    blobmsg_add_u32(&bb,"ack", 1);
    ubus_send_reply(ctx, req, bb.head);

    /*  -----  reply results to the caller -----
     * NOTE: we may put proceeding job in a timeout task, just to speed up service response.
     */
    ubus_defer_request(ctx, req, &req_data);
    ubus_complete_deferred_request(ctx, req, UBUS_STATUS_OK);

    return 0;
}

static void *ubus_server_loop()
{
    /* uloop routine: events monitoring and callback provoking */
    uloop_run();

    ubus_server_running = 0;

    return NULL;
}

int ubus_server_init()
{
    const char *ubus_socket=NULL; /* use default UNIX sock path: /var/run/ubus.sock */

    /* 1. create an epoll instance descriptor poll_fd */
    uloop_init();

    /* 2. connect to ubusd and get ctx */
    ctx=ubus_connect(ubus_socket);
    if(ctx==NULL) {
        printf("Fail to connect to ubusd!\n");
        return 1;
    }

    /* 3. registger epoll events to uloop, start sock listening */
    ubus_add_uloop(ctx);

    /* 4. register a usb_object to ubusd */
    int ret=ubus_add_object(ctx, &wlanclock_data_obj);
    if(ret!=0) {
        printf("Fail to register an object to ubus.\n");
        ubus_free(ctx);
        return 1;
    } else {
        printf("Add '%s' to ubus @%u successfully.\n", wlanclock_data_obj.name, wlanclock_data_obj.id);
    }

    ubus_server_running = 1;
    if (pthread_create(&ubus_server_thread, NULL, &ubus_server_loop, NULL) != 0)
    {
        fprintf(stderr,"pthread_create() error");
        return 1;
    }
    return 0;
}

void ubus_server_deinit()
{
    pthread_join(ubus_server_thread, NULL);
    uloop_end();
    uloop_done();
    ubus_free(ctx);
}
