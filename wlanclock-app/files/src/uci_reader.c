#include <stdio.h>
#include <uci.h>
#include <limits.h>

#include "uci_reader.h"

#define MAX_VALUE_LEN = LINE_MAX;

char *uci_reader_get(const char *file, const char *section, const char *key)
{
    static char param_value[LINE_MAX];
    static char param_name[LINE_MAX];
    char *ret = NULL;
    struct  uci_ptr ptr;
    struct  uci_context *c = uci_alloc_context();
    int UCI_LOOKUP_COMPLETE = (1 << 1);

    if ((!file) || (!section) || (!key) || (!c))
    {
        uci_free_context(c);
        return NULL;
    }

    snprintf(param_name, sizeof(param_name), "%s.%s.%s", file, section, key);

    if ((uci_lookup_ptr(c, &ptr, param_name, 1) != UCI_OK) ||
         (ptr.o==NULL))
    {
        perror("Parameter not found\n");
        uci_free_context(c);
        return NULL;
    }
    if(ptr.flags & UCI_LOOKUP_COMPLETE)
    {
        if (ptr.o->type == UCI_TYPE_STRING)
        {
            snprintf(param_value, sizeof(param_value), "%s", ptr.o->v.string);
            ret = param_value;
        }
        else
        {
            perror("Wrong parameter type\n");
        }
    }
    uci_free_context(c);
    return ret;
}

int uci_reader_get_list(const char *file, const char *section, const char *key, uci_cb_list_element_t cb)
{
    static char param_name[LINE_MAX];
    int ret = 0;
    struct  uci_ptr ptr;
    struct  uci_context *c = uci_alloc_context();
    int UCI_LOOKUP_COMPLETE = (1 << 1);

    if ((!file) || (!section) || (!key) || (!c) || (!cb) )
    {
        uci_free_context(c);
        return 1;
    }

    snprintf(param_name, sizeof(param_name), "%s.%s.%s", file, section, key);

    if ((uci_lookup_ptr(c, &ptr, param_name, 1) != UCI_OK) ||
         (ptr.o==NULL))
    {
        perror("Parameter not found\n");
        uci_free_context(c);
        return 1;
    }
    if(ptr.flags & UCI_LOOKUP_COMPLETE)
    {
        if (ptr.o->type == UCI_TYPE_LIST) {
            struct uci_element *e;
            uci_foreach_element(&ptr.o->v.list, e) {
                cb(e->name);
            }
        }
        else
        {
            perror("Wrong parameter type\n");
        }
    }
    uci_free_context(c);
    return ret;
}
