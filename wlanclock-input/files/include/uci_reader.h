#ifndef UCI_READER_H
#define UCI_READER_H

/*! CPP guard */
#ifdef __cplusplus
extern "C" {
#endif

char *uci_reader_get(char *file, char *section, char *key);
int *uci_reader_get_int(char *file, char *section, char *key);

#ifdef __cplusplus
}
#endif /* End of CPP guard */
#endif /* UCI_READER_H */
