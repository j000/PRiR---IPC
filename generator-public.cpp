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
	flag = 0;
}

auto main(int argc, char** argv) -> int
{
	const unsigned threads{argc == 1 ? 1 : std::stoul(argv[1])};
	auto pids = std::vector<pid_t>(threads);

	std::cout << "Key 1: 0x" << std::hex << key << std::dec << std::endl;

	for (unsigned thread = threads; thread > 0; --thread) {
		pid_t tmp = fork();
		if (tmp == 0) {
			{
				struct sigaction new_action {
				};
				new_action.sa_handler = my_function;
				sigfillset(&new_action.sa_mask);
				new_action.sa_flags = 0;
				sigaction(SIGINT, &new_action, nullptr);
			}

			SharedMemory sm{key};
			float* ptr = sm.attach();

			MessageQueue mq{key};
			struct info_msgbuf im {
			};

			const int size = BLOCK_SIZE / threads;
			const int offset = (thread - 1) * size;

			im.mtype = thread;
			im.info.pid = getpid();
			im.info.offset = offset;
			im.info.size = size;

			Random rand{};

			float counter = 0.0;
			while (flag) {
				std::cout << thread
						  << ": Wypelniam pamiec (counter = " << counter << ")"
						  << std::endl;
				for (int i = 0; i < size; i++) {
					ptr[offset + i] = counter + rand.get();
				}
				// std::cout << thread << ": Wysylam wiadomosc" << std::endl;
				mq.send(&im);
				// std::cout << thread << ": Ide spac" << std::endl;
				counter += 0.1f;
				struct info_msgbuf tmp {
				};
				mq.receive(&tmp, im.info.pid);
			}
			std::cout << thread << ": done" << std::endl;
			return 0;
		} else {
			pids[thread] = tmp;
		}
	}
	// ignore ctrl+c
	// signal(SIGINT, SIG_IGN);
	{
		struct sigaction new_action {
		};
		new_action.sa_handler = SIG_IGN;
		sigfillset(&new_action.sa_mask);
		new_action.sa_flags = 0;
		sigaction(SIGINT, &new_action, nullptr);
	}

	usleep(100 * 1000);

	for (const auto& thread : pids) {
		waitpid(thread, nullptr, 0);
	}
}
