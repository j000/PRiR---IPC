#ifndef COMMON2_HPP
#define COMMON2_HPP

#include "common.h"
#include <iostream>
#include <random>
#include <stdexcept>
#include <sys/msg.h>
#include <sys/shm.h>

class SharedMemory {
public:
	SharedMemory(int key) : m_key{key}
	{
		m_shmid = shmget(m_key, BLOCK_SIZE * sizeof(float), 0642 | IPC_CREAT);
		if (m_shmid == -1)
			throw std::runtime_error("Cannot create shared SharedMemoryory");
	}
	~SharedMemory() noexcept(false)
	{
		// if (shmctl(m_shmid, IPC_RMID, NULL) == -1)
		// 	throw std::runtime_error("Shared memory deletion failed");
		std::cerr << "ipcrm shm " << m_shmid << std::endl;
	}

	float* attach()
	{
		void* ptr = shmat(m_shmid, nullptr, 0);
		if (ptr == reinterpret_cast<void*>(-1))
			throw std::runtime_error("Cannot attach to shared memory");
		return reinterpret_cast<float*>(ptr);
	}

private:
	const int m_key{0};
	int m_shmid{0};
};

class MessageQueue {
public:
	MessageQueue(int key) : m_key{key}
	{
		create();
	}
	~MessageQueue() noexcept(false)
	{
		/* TODO: this removes queue even when in use! */
		// if (msgctl(m_msgid, IPC_RMID, NULL) == -1)
		// 	throw std::runtime_error("Message queue deletion failed");
		std::cerr << "ipcrm msg " << m_msgid << std::endl;
	}
	auto receive(struct info_msgbuf* const buf, const int type = 1) -> void
	{
		if (msgrcv(m_msgid, buf, m_msgsz, type, 0) == -1) {
			// if (errno == EIDRM) {
			// 	create();
			// 	return receive(buf);
			// }
			std::cout << "Message queue receive failed" << std::endl;
		}
	}
	auto send(struct info_msgbuf* const buf, const int type = 1) -> void
	{
		buf->mtype = type;
		if (msgsnd(m_msgid, buf, m_msgsz, 0) == -1) {
			// if (errno == EIDRM) {
			// 	create();
			// 	return send(buf);
			// }
			std::cout << "Message queue send failed" << std::endl;
		}
	}

private:
	const int m_key{0};
	int m_msgid{0};
	const int m_msgsz{sizeof(struct info_struct)};

	auto create() -> void
	{
		m_msgid = msgget(m_key, 0642 | IPC_CREAT);
	}
};

class Random {
public:
	auto get() -> float
	{
		return m_dis(m_gen);
	}

private:
	std::random_device m_rd{};
	std::mt19937 m_gen{m_rd()};
	std::uniform_real_distribution<float> m_dis{-0.5, 0.5};
};

#endif /* COMMON2_HPP */
