#ifndef UCI_READER_H
#define UCI_READER_H

/*! CPP guard */
#ifdef __cplusplus
extern "C" {
#endif

void uci_reader_get(char *file, char *section, char *param, char **value);

#ifdef __cplusplus
}
#endif /* End of CPP guard */
#endif /* UCI_READER_H */
