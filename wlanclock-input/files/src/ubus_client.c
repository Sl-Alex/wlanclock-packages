#include <pthread.h>
#include <libubus.h>

static struct ubus_context *ctx;
static struct blob_buf  bb;

#include <libubus.h>
#include <libubox/blobmsg_json.h>

static struct ubus_context *ctx;
static struct blob_buf  bb;

/* ubus object assignment */
static struct ubus_object wlanclock_obj=
{
    .name = "wlanclock_caller",
};

/* callback function for ubus_invoke to process result from the host
 * Here we just print out the message.
 */
static void result_handler(struct ubus_request *req, int type, struct blob_attr *msg)
{
        char *strmsg;

        if(!msg)
                return;

        strmsg=blobmsg_format_json_indent(msg,true, 0); /* 0 type of format */
        printf("Response from the host: %s\n", strmsg);
        free(strmsg); /* need to free strmsg */
}

void ubus_client_send_gesture(uint32_t gesture)
{
    int ret;
    uint32_t host_id;
    const char *ubus_socket=NULL; /* use default UNIX sock path: /var/run/ubus.sock */

    /* 1. create an epoll instatnce descriptor poll_fd */
    uloop_init();

    /* 2. connect to ubusd and get ctx */
    ctx=ubus_connect(ubus_socket);
    if(ctx==NULL) {
        fprintf(stderr, "Fail to connect to ubusd!\n");
        return;
    }

    /* 3. registger epoll events to uloop, start sock listening */
    ubus_add_uloop(ctx);

    /* 4. register a usb_object to ubusd */
    ret=ubus_add_object(ctx, &wlanclock_obj);
    if(ret!=0) {
        fprintf(stderr, "Fail to register an object to ubus.\n");
        goto UBUS_FAIL;

    } else {
        printf("Add '%s' to ubus @%u successfully.\n",wlanclock_obj.name, wlanclock_obj.id);
    }

    /* 5. search a registered object with a given name */
    if( ubus_lookup_id(ctx, "wlanclock.app", &host_id) ) {
        fprintf(stderr, "wlanclock.app is NOT found in ubus!\n");
        goto UBUS_FAIL;
    }
    printf("wlanclock.app is found in ubus @%u.\n",host_id);

    /* 6. prepare request method policy and data */
    blob_buf_init(&bb,0);
    blobmsg_add_u32   (&bb, "gesture",  gesture );

    /* 7. call the ubus host object */
    ret=ubus_invoke(ctx, host_id, "data", bb.head, result_handler, 0, 500);
    printf("Call result: %s\n", ubus_strerror(ret));

    uloop_done();

UBUS_FAIL:
    ubus_free(ctx);
}

void ubus_client_send_rel(int32_t x, int32_t y)
{
    int ret;
    uint32_t host_id;
    const char *ubus_socket=NULL; /* use default UNIX sock path: /var/run/ubus.sock */

    /* 1. create an epoll instatnce descriptor poll_fd */
    uloop_init();

    /* 2. connect to ubusd and get ctx */
    ctx=ubus_connect(ubus_socket);
    if(ctx==NULL) {
        fprintf(stderr, "Fail to connect to ubusd!\n");
        return;
    }

    /* 3. registger epoll events to uloop, start sock listening */
    ubus_add_uloop(ctx);

    /* 4. register a usb_object to ubusd */
    ret=ubus_add_object(ctx, &wlanclock_obj);
    if(ret!=0) {
        fprintf(stderr, "Fail to register an object to ubus.\n");
        goto UBUS_FAIL;

    } else {
        printf("Add '%s' to ubus @%u successfully.\n",wlanclock_obj.name, wlanclock_obj.id);
    }

    /* 5. search a registered object with a given name */
    if( ubus_lookup_id(ctx, "wlanclock.app", &host_id) ) {
        fprintf(stderr, "wlanclock.app is NOT found in ubus!\n");
        goto UBUS_FAIL;
    }
    printf("wlanclock.app is found in ubus @%u.\n",host_id);

    /* 6. prepare request method policy and data */
    blob_buf_init(&bb,0);
    blobmsg_add_u32   (&bb, "rel_x", x);
    blobmsg_add_u32   (&bb, "rel_y", y);

    /* 7. call the ubus host object */
    ret=ubus_invoke(ctx, host_id, "data", bb.head, result_handler, 0, 500);
    printf("Call result: %s\n", ubus_strerror(ret));

    uloop_done();

    UBUS_FAIL:
    ubus_free(ctx);
}
