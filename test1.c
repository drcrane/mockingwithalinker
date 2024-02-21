#include <stdio.h>
#include "service.h"

int main(int argc, char * argv[]) {
	void * context = service_init();
	if (context == NULL) {
		return 1;
	}
	char * data = NULL;
	size_t data_sz = 0;
	int rc = service_get_data(context, "https://google.com", &data, &data_sz);
	if (rc != 0 || data == NULL || data_sz == 0) {
		fprintf(stderr, "%d\n", rc);
		return 1;
	}
	service_free_data(data);
	service_dispose(context);
	return 0;
}

