# Testing and Stubbing

This set of talks will discuss the various ways that functions can be mocked
with varying impact on production code. The first will discuss the mocking of
C functions via symbols, that are exposed via compiler to the linker.

For the example code to work `gcc` and `curl-dev` are required.

## Test Driven Development

The basic philosophy behind **tdd** is that code should only be written to
satisfy the test. So it follows that when performing **tdd** tests should be
written before any implementation, of course a test without an implementation
to rely upon will not compile but that is ok because a test that does not
compile is a failing test. The underlying aim of writing the test before the
implementation is to ensure code coverage. Complete code coverage can
sometimes be challenging without mocked functions.

One way this can be accomplished is to write an application as a library, the
tests and the application may then be linked with the library allowing the
application executables and the tests to be created.

In **tdd** style implementation code should not be written unless there is a
test to test it... but at least an API must be defined upon which a test can
be written.

The portion of the library under development is intended to download some data
from a server. The API for this component is defined in `service.h`.

### Using the Library

First the library must be initialised by calling `void * service_init();`
Then use the returned context to request the data from the server
`int service_get_data(void * context, char * url, char ** dst, size_t ** dst_size);`
free any returned data `void service_free_data(char * data);`
finally, clean up: `service_dispose(void * context);`.

To follow **tdd** first the test should be written in `test0.c`:

```c
#include "service.h"

int main(int argc, char * argv[]) {
    void * context = service_init();
    if (context == NULL) {
        return 1;
    }
    // not strictly tdd but required since otherwise the test will be
    // leaky
    service_dispose(context);
    return 0;
}
```

Success or failure indications: In traditional Unix style success is indicated
by an exit code of 0. The exit code may be used by testing frameworks or
scripts to determine test success. In the bash shell the return code may be
printed with `echo $?`:

```sh
$ true
$ echo $?
0
$ false
$ echo $?
1
```

Of course this will not compile without the implementation of service, for
completeness there are multiple implementations that will be used for
various techniques, for now these are the files that are required to compile
the test:

```
service.h
service_impl_curl.c
```

To compile `test0` with the curl implementation:

    $ gcc -o service_impl_curl.o -c service_impl_curl.c
    $ gcc -o test0.o -c test0.c
    $ gcc -o test0 test0.o service_impl_curl.o -lcurl
    $ valgrind ./test0
    ...
    $ echo $?
    0

Valgrind is used here to check for leaks. The last echo will print the exit
code.

`test0` is fine as it does not actually make a network call and so it is
appropriate to be included in the test suite as-is.

Now `test1`:

```c
#include "service.h"

int main(int argc, char * argv[]) {
    void * context = service_init();
    if (context == NULL) {
        return 1;
    }
    char * data = NULL;
    size_t data_sz = 0;
    int rc = service_get_data(context, "https://google.com/", &data, &data_sz);
    if (rc != 0 || data == NULL || data_sz == 0) {
        return 1;
    }
    service_free_data(data);
    data = NULL;
    service_dispose(context);
    return 0;
}
```

    $ gcc -o service_impl_curl.o -c service_impl_curl.c
    $ gcc -o test1.o -c test1.c
    $ gcc -o test1 test1.o service_impl_curl.o -lcurl
    $ valgrind ./test1
    ...
    $ echo $?
    0

This test will make a network call to `https://google.com` which is great for
when the programme is in use but not so great for testing.

### Implementing a Mock

To implement a mock all the functions in `service_impl_curl` must be
re-implemented and return the expected values.

    $ gcc -o service_mock.o -c service_mock.c
    $ gcc -o test1 test1.o service_mock.o
    $ valgrind ./test1

This has linked the `service_mock` to `test1` as expected and the test should
still be successful.

## Summary

This method is quite useful but requires a lot of work and to test error cases
such as a return value of -1 a different `service_mock` file would have to be
implemented for each return code. In summary it is a valid method but too much
work. The technique above is just a refresher on how a linker works.

