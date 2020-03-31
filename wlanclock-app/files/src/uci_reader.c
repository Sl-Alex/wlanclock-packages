#include <stdio.h>
#include <uci.h>
#include <limits.h>

#define MAX_VALUE_LEN = LINE_MAX;

char *uci_reader_get(char *file, char *section, char *param)
{
    static char param_value[LINE_MAX];
    static char param_name[LINE_MAX];
    char *ret = NULL;
    struct  uci_ptr ptr;
    struct  uci_context *c = uci_alloc_context();
    int UCI_LOOKUP_COMPLETE = (1 << 1);

    if ((!file) || (!section) || (!param) || (!c))
    {
        uci_free_context(c);
        return NULL;
    }

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
