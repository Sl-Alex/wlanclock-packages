#ifndef UCI_READER_H
#define UCI_READER_H

/*! CPP guard */
#ifdef __cplusplus
extern "C" {
#endif

typedef void (*uci_cb_list_element_t)(char* value);

char *uci_reader_get(const char *file, const char *section, const char *key);
int uci_reader_get_list(const char *file, const char *section, const char *key, uci_cb_list_element_t cb);

#ifdef __cplusplus
}
#endif /* End of CPP guard */
#endif /* UCI_READER_H */
