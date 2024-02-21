#include <stddef.h>

void * service_init();
int service_get_data(void * context, char * url, char ** data, size_t * data_sz);
void service_free_data(char * data);
void service_dispose(void * context);

