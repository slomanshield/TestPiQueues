#include <mutex>
#include <list>
#include <chrono>
#include <ratio>
#include <iostream>
#include <condition_variable>
#include <queue>
#include <unordered_map>
#include <exception>
#include <thread>

typedef std::chrono::milliseconds Milli;

/* To gaurntee the thread ids in the list are valid the writer of the object should wrap all the code in the 
   try catch inside the while loop for their thread routine so on break it can de register */

namespace QueueWrapper 
{
	#define DEFAULT_MAX_OUTSTANDING 100
	#define UNLIMITED_MAX_OUTSTANDING -1
	#define DEFAULT_MINIMUM_READERS 1

	typedef std::unordered_map< std::string, void*>::const_iterator QueueIterator;
	typedef std::list<std::thread::id>::const_iterator ThreadIdListInterator;

	template<typename T>
	class Queue
	{
		public:
			Queue(long long max_out_standing,long minimum_readers_req);
			Queue();
			~Queue();
			bool WaitForQueueData(int milliTimeOut, std::unique_lock<std::mutex>* pLK);
			void IncrementEventCount();
			void DecrementEventCount();
			long long GetMaxOutStanding();
			long GetMinimumReaders();
			ThreadIdListInterator FindThreadId(std::thread::id threadId);
			std::queue<T> mainQueue;
			std::mutex m;
			std::condition_variable cv;

			std::list<std::thread::id> readerThreadIdList;
			std::mutex m_ThreadIdList;
		private:
			long long max_out_standing;
			long minimum_readers_req;
			unsigned long long event_count;
	};

	class QueueManager
	{
		public:
			static QueueManager* Instance();
			template<typename T>
			T GetDataFromQueue(std::string* queueName, int milliTimeOut, int* ccOut);
			template<typename T>
			int PutDataOnQueue(std::string* queueName, T data);
			template<typename T>
			int FindQueue(std::string* queueName, Queue<T>** pQueueOut);
			template<typename T>
			int GetQueuesize(std::string* queueName, size_t* queueSizeOut);
			template<typename T>
			int CreateQueue(std::string* queueName, long long* pMaxOutstanding, long* pMinimumReadersReq);
			template<typename T>
			int DeleteQueue(std::string* queueName,bool outstandingOverride);
			template<typename T>
			int RegisterThreadToQueue(std::string* queueName, std::thread::id threadId);
			template<typename T>
			int RemoveThreadFromQueue(std::string* queueName, std::thread::id threadId);
			template<typename T>
			int ReadersExistForQueue(Queue<T>* pQueue);
		private:
			QueueManager();
			~QueueManager();
			static QueueManager* pQueueManagerInstance;
			std::unordered_map< std::string, void*> queueMap;
			std::mutex m_queues;
	};

	enum error_codes
	{
		QUEUE_SUCCESS = 0,
		QUEUE_NOT_FOUND,
		QUEUE_TIMEOUT,
		QUEUE_MAX_OUTSTANDING,
		QUEUE_ALREADY_DEFINED,
		QUEUE_HAS_DATA,
		THREAD_ID_EXISTS,
		THREAD_ID_NOT_FOUND,
		NO_READERS_FOR_QUEUE,
		NO_MINIMUM_READERS_FOR_QUEUE

	};

	
};

#include "QueueWrapper_Impl.h"