#!/bin/sh
set -x
gcc -o service_impl_curl.o -c service_impl_curl.c
gcc -o service_mock.o -c service_mock.c
gcc -o test0.o -c test0.c
gcc -o test1.o -c test1.c
gcc -o test0_impl test0.o service_impl_curl.o -lcurl
gcc -o test1_impl test1.o service_impl_curl.o -lcurl
gcc -o test1_mock test1.o service_mock.o

