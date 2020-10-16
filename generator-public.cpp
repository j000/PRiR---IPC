#include "common.h"
#include "common2.hpp"
#include <csignal>
#include <iostream>
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
	struct info_msgbuf im {
	};

	int offset = 0;
	int size = BLOCK_SIZE;

	im.mtype = getpid(); // typ wiadomosci = PID generatora
	im.info.pid = getpid(); // tu powielamy PID generatora
	im.info.offset = offset; // poczatek obszaru z danymi do przetworzenia
	im.info.size = size; // rozmiar obszaru gotowego do przetworzenia

	Random rand{};

	float counter = 0.0;
	while (flag) {
		std::cout << "Wypelniam pamiec (counter = " << counter << ")" << std::endl;
		for (int i = 0; i < size; i++) {
			ptr[offset + i] = counter + rand.get();
		}
		std::cout << "Wysylam wiadomosc" << std::endl;
		mq.send(&im);
		if (!flag)
			continue;
		std::cout << "Ide spac" << std::endl;
		counter += 0.1f;
		mq.receive(&im, getpid());
	}
	std::cout << std::endl;
}
