#include "common.h"
#include "common2.hpp"
#include <csignal>
#include <iostream>
#include <string>
#include <sys/wait.h>
#include <unistd.h>

volatile sig_atomic_t flag = 1;
void my_function(int)
{
	std::cout << "signal" << std::endl;
	flag = 0;
}

auto main(int argc, char** argv) -> int
{
	const unsigned threads{argv[1] == nullptr ? 1 : std::stoul(argv[1])};
	auto pids = std::vector<pid_t>(threads);

	std::cout << "Key 1: 0x" << std::hex << key << std::dec << std::endl;

	for (unsigned thread = threads; thread > 0; --thread) {
		pid_t tmp = fork();
		if (tmp == 0) {
			std::cout << "FORKED! " << getpid() << std::endl;

			signal(SIGINT, my_function);

			SharedMemory sm{key};
			float* ptr = sm.attach();

			MessageQueue mq{key};
			struct info_msgbuf im {
			};

			const int size = BLOCK_SIZE / threads;
			const int offset = (thread - 1) * size;

			im.mtype = thread;
			im.info.pid = getpid(); // tu powielamy PID generatora
			im.info.offset
				= offset; // poczatek obszaru z danymi do przetworzenia
			im.info.size = size; // rozmiar obszaru gotowego do przetworzenia

			Random rand{};

			float counter = 0.0;
			while (flag) {
				std::cout << thread
						  << ": Wypelniam pamiec (counter = " << counter << ")"
						  << std::endl;
				for (int i = 0; i < size; i++) {
					ptr[offset + i] = counter + rand.get();
				}
				std::cout << thread << ": Wysylam wiadomosc" << std::endl;
				mq.send(&im);
				if (!flag)
					continue;
				std::cout << thread << ": Ide spac" << std::endl;
				counter += 0.1f;
				struct info_msgbuf tmp {
				};
				mq.receive(&tmp, im.info.pid);
			}
			std::cout << thread << ": done" << std::endl;
			return 0;
		} else {
			std::cout << "pids: " << tmp << std::endl;
			pids[thread] = tmp;
		}
	}
	signal(SIGINT, SIG_IGN); // ignore ctrl+c

	usleep(100 * 1000);

	for (const auto& thread : pids) {
		std::cout << "waiting for " << thread << std::endl;
		waitpid(thread, nullptr, 0);
	}
}
