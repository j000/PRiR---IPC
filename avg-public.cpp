#include "common.h"
#include "common2.hpp"
#include <iostream>
#include <csignal>
#include <unistd.h>

volatile sig_atomic_t flag = 1;
void my_function(int)
{
	flag = 0;
}

auto main() -> int
{
	signal(SIGINT, my_function);

	std::cout << "Key 1: 0x" << std::hex << key << std::endl;

	SharedMemory sm{key};
	float* ptr = sm.attach();

	MessageQueue mq{key};
	struct info_msgbuf im{};

	while (flag) {
		std::cout << "Odbieramy" << std::endl;
		mq.receive(&im);

		if (!flag)
			continue;

		auto pid = im.info.pid;

		std::cout << "Odebrane: PID : " << im.info.pid // PID nadawcy
			 << "       OFFSET : " << im.info.offset
			 << "         SIZE : " << im.info.size << std::endl;

		std::cout << "Licze srednia: ";

		float s = 0.0;
		for (int i = 0; i < im.info.size; i++) {
			s += ptr[im.info.offset + i];
		}
		std::cout << (s / im.info.size) << std::endl;

		std::cout << "Powiadamiam generator " << pid << std::endl;
		mq.send(&im, pid);
	}
	std::cout << std::endl;
}
