#include <stddef.h>
#include <malloc.h>
#include <string.h>
#include "service.h"

struct mock_service_data {
	int fd;
};

void * service_init() {
	struct mock_service_data * context = malloc(sizeof(*context));
	return (void *)context;
}


int service_get_data(void * context, char * url, char ** data, size_t * data_sz) {
	char * response = strdup("Response from Server");
	*data = response;
	*data_sz = strlen(response);
	return 0;
}

void service_free_data(char * data) {
	free(data);
}

void service_dispose(void * context) {
	free(context);
}

