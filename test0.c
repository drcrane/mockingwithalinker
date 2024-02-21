#include <stdio.h>
#include "service.h"

int main(int argc, char * argv[]) {
	void * context = service_init();
	if (context == NULL) {
		return 1;
	}
	service_dispose(context);
	return 0;
}

