#include "Config.h"
#include "UciReader.h"
#include <uci.h>
#include <cstring>
#include <limits.h>
#include <iostream>

std::string UciReader::getKey(const char *path)
{
    static char param_value[LINE_MAX];
    static char param_name[LINE_MAX];
    char *ret = nullptr;
    struct  uci_ptr ptr;
    struct  uci_context *c = uci_alloc_context();

    if (!path)
    {
        std::cerr << "Path is NULL" << std::endl;
        uci_free_context(c);
        return "";
    }

    strncpy(param_name, path, LINE_MAX-1);

    if ((uci_lookup_ptr(c, &ptr, param_name, 1) != UCI_OK) ||
         (ptr.o==NULL))
    {
        std::cerr << "Parameter not found" << std::endl;
        uci_free_context(c);
        return "";
    }
    if(ptr.flags & uci_ptr::UCI_LOOKUP_COMPLETE)
    {
        if (ptr.o->type == UCI_TYPE_STRING)
        {
            snprintf(param_value, sizeof(param_value), "%s", ptr.o->v.string);
            ret = param_value;
        }
        else
        {
            std::cerr << "Wrong parameter type" << std::endl;
        }
    }
    uci_free_context(c);
    return (ret == nullptr) ? "" : std::string(ret);
}

int UciReader::setKey(const char *path, const char *value)
{
    static char param_name[LINE_MAX];
    struct  uci_ptr ptr;
    struct  uci_context *c = uci_alloc_context();

    if (!path)
    {
        std::cerr << "Path is NULL" << std::endl;
        uci_free_context(c);
        return 1;
    }

    strncpy(param_name, path, LINE_MAX-1);

    if ((uci_lookup_ptr(c, &ptr, param_name, 1) != UCI_OK))
    {
        perror("Parameter not found\n");
        uci_free_context(c);
        return 1;
    }
    ptr.value = value;

    if ((uci_set(c, &ptr) != UCI_OK) ||
            (ptr.o == NULL || ptr.o->v.string == NULL ))
    {
        uci_free_context(c);
        uci_perror(c,"UCI Error set");
        return 1;
    }

    // commit your changes to make sure that UCI values are saved
    if (uci_commit(c, &ptr.p, false) != UCI_OK)
    {
        uci_free_context(c);
        uci_perror(c,"UCI Error commit");
        return 1;
    }

    uci_free_context(c);
    return 0;
}
