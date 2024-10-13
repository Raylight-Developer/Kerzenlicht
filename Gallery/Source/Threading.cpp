#include "Threading.hpp"

void Database_Thread::run() {
	while (true) {
		semaphore.lock();

		while (!work_queue.isEmpty()) {
			const DB::Query& queuedWork = work_queue.dequeue();
			semaphore.unlock();
			processWork(queuedWork);
			semaphore.lock();
		}

		if (work_queue.isEmpty()) {
			condition.wait(&semaphore);
		}

		semaphore.unlock();
	}
}

void Database_Thread::processWork(const DB::Query& i_work) {
	
}

void Database_Thread::queueWork(const DB::Query& i_work) {
	QMutexLocker locker(&semaphore);
	work_queue.enqueue(i_work);
	condition.wakeOne();
}

void Database_Thread::cancelWork(const DB::Query_Type& i_type) {
	QMutexLocker locker(&semaphore);
	for (int i = 0; i < work_queue.size(); i++) {
		const DB::Query currentItem = work_queue.dequeue();
		if (currentItem.request_id.type != i_type) {
			work_queue.enqueue(currentItem);
		}
	}
	condition.wakeOne();
}