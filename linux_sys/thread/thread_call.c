/*
 * (c) 2008-2009 Author(s)
 *     economic rights: Technische Universit盲t Dresden (Germany)
 *
 * This file is part of TUD:OS and distributed under the terms of the
 * GNU General Public License 2.
 * Please see the COPYING-GPL-2 file for details.
 */

#include <unistd.h>
#include <stdio.h>
#include <pthread.h>

static void *thread1_fn(void *arg)
{
	(void)arg;
	int count = 0;
	while (1) {
		count++;
		printf("[1]:  %d\n", count);
	}
	return NULL;
}

static void *thread2_fn(void *arg)
{
	(void)arg;
	int count = 0;
	while (1) {
		count++;
		printf("[2]:  %d\n", count);
	}
	return NULL;
}

static void *thread3_fn(void *arg)
{
	(void)arg;
	int count = 0;
	while (1) {
		count++;
		printf("[3]:  %d\n", count);
	}
	return NULL;
}

static void *thread4_fn(void *arg)
{
	(void)arg;
	int count = 0;
	while (1) {
		count++;
		printf("[4]:  %d\n", count);
	}
	return NULL;
}

static void *thread5_fn(void *arg)
{
	(void)arg;
	int count = 0;
	while (1) {
		count++;
		printf("[5]:  %d\n", count);
	}
	return NULL;
}

static void *thread6_fn(void *arg)
{
	(void)arg;
	int count = 0;
	while (1) {
		count++;
		printf("[6]:  %d\n", count);
	}
	return NULL;
}

static void *thread7_fn(void *arg)
{
	(void)arg;
	int count = 0;
	while (1) {
		count++;
		printf("[7]:  %d\n", count);
	}
	return NULL;
}

int main(void)
{
	static pthread_t t2, t3, t4, t5, t6, t7;
	if (pthread_create(&t2, NULL, thread2_fn, NULL)) {
		fprintf(stderr, "Thread creation failed\n");
		return 1;
	}

	if (pthread_create(&t3, NULL, thread3_fn, NULL)) {
		fprintf(stderr, "Thread creation failed\n");
		return 1;
	}

	if (pthread_create(&t4, NULL, thread4_fn, NULL)) {
		fprintf(stderr, "Thread creation failed\n");
		return 1;
	}

	if (pthread_create(&t5, NULL, thread5_fn, NULL)) {
		fprintf(stderr, "Thread creation failed\n");
		return 1;
	}

	if (pthread_create(&t6, NULL, thread6_fn, NULL)) {
		fprintf(stderr, "Thread creation failed\n");
		return 1;
	}

	if (pthread_create(&t7, NULL, thread7_fn, NULL)) {
		fprintf(stderr, "Thread creation failed\n");
		return 1;
	}

	thread1_fn(NULL);
	return 0;
}
