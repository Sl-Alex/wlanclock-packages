#include <stdio.h>
#include <stdlib.h>
#include <uci.h>
#include <limits.h>
#include <errno.h>

#include "uci_reader.h"

#define MAX_VALUE_LEN = LINE_MAX;

char *uci_reader_get(char *file, char *section, char *key)
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
        fprintf(stderr, "Parameter not found: %s.%s.%s\n", file, section, key);
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
            fprintf(stderr, "Wrong parameter type, shall be string\n");
        }
    }
    uci_free_context(c);
    return ret;
}

int *uci_reader_get_int(char *file, char *section, char *key)
{
    static int result;
    char *rc = uci_reader_get(file, section, key);
    if (!rc)
    {
        return NULL;
    }
    long int rcli = strtol(rc, NULL, 0);
    if (errno == ERANGE)
    {
        return NULL;
    }
    if ((rcli > INT_MAX) || (rcli < INT_MIN))
    {
        return NULL;
    }
    result = rcli;
    return &result;
}
