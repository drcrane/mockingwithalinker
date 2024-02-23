#define CURL_STATICLIB
#include <curl/curl.h>
#include <malloc.h>
#include <string.h>
#include "service.h"

void * service_init() {
	curl_global_init(CURL_GLOBAL_DEFAULT);
	CURL * curl = curl_easy_init();
	return (void *)curl;
}

struct buffer {
	char * ptr;
	size_t len;
};

/* function is binary safe but will always ensure the data is terminated with \0 */
static size_t writefunction(void * ptr, size_t size, size_t nmemb, struct buffer * buf) {
	size_t block_sz = size * nmemb;
	size_t new_len = buf->len + block_sz;
	buf->ptr = realloc(buf->ptr, new_len + 1);
	if (buf->ptr == NULL) {
		return 0;
	}
	memcpy(buf->ptr + buf->len, ptr, block_sz);
	buf->ptr[new_len] = '\0';
	buf->len = new_len;
	return block_sz;
}

int service_get_data(void * context, char * url, char ** data, size_t * data_sz) {
	int res = 0;
	CURL * curl = (CURL *)context;
	CURLU * curlu_url = curl_url();
	CURLUcode curlu_rc = curl_url_set(curlu_url, CURLUPART_URL, url, 0);
	CURLcode curl_rc;
	long response_code = 0;
	struct buffer buf = {.ptr = NULL, .len = 0};
	if (curlu_rc != CURLUE_OK) {
		res = -1;
		goto finish;
	}
	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writefunction);
	curl_easy_setopt(curl, CURLOPT_WRITEDATA, &buf);
	curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
	curl_rc = curl_easy_setopt(curl, CURLOPT_CURLU, curlu_url);
	if (curl_rc != CURLE_OK) {
		res = -2;
		goto finish;
	}
	curl_rc = curl_easy_perform(curl);
	if (curl_rc != CURLE_OK) {
		res = -3;
		goto finish;
	}
	curl_rc = curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &response_code);
	if (curl_rc != CURLE_OK) {
		res = -4;
		goto finish;
	}
	if (response_code != 200) {
		res = response_code;
	}
	*data = buf.ptr;
	*data_sz = buf.len;
finish:
	if (curlu_url) {
		curl_url_cleanup(curlu_url);
	}
	return res;
}

void service_free_data(char * data) {
	free(data);
}

void service_dispose(void * context) {
	CURL * curl = (CURL *)context;
	curl_easy_cleanup(curl);
	curl_global_cleanup();
}

