#ifndef COMMON_HPP
#define COMMON_HPP

#include <fcntl.h>
#include <sys/ipc.h>

const int BLOCK_SIZE = 4000000;

const key_t key = ftok(__FILE__, 42);

struct info_struct {
	pid_t pid;
	int offset;
	int size;
};

struct info_msgbuf {
	long mtype;
	struct info_struct info;
};

#endif /* COMMON_HPP */
