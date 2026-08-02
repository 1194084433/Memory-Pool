#include<iostream>
#include<string>
#include<thread>
#include "memory-pool.h"
#include<vector>
std::mutex cout_mutex;
class TestClass
{
private:
	int id;
public:
	TestClass(int id)
		:id(id)
	{
		std::lock_guard<std::mutex> lock(cout_mutex);
		std::cout << "Construct class: " << "id: " << id << std::endl;
	}

	~TestClass()
	{
		std::cout << "Delete class: id: " << id << std::endl;
	}
	void print() const;



};
void TestClass::print() const
{
	std::cout << "ID: " << id << std::endl;
}

void testSingleThread()
{
	std::cout << "！！！！！！SingleThread Test！！！！！！！！" << std::endl;
	TestClass* p1 = memoryPool::HashBucket::newElement<TestClass>(100);
	memoryPool::HashBucket::deleteElement<TestClass>(p1);

}

void ThreadTask(int threadId)
{
	for (int i = 0; i < 3; i++)
	{
		TestClass* p = memoryPool::HashBucket::newElement<TestClass>(threadId);
		memoryPool::HashBucket::deleteElement(p);
		

	}
}
void testMultiThread()
{
	std::cout << "！！！！！！！MultThread Test！！！！！！！！" << std::endl;
	std::vector<std::thread> threads;
	for (int i = 0; i < 3; i++)
	{
		threads.emplace_back(ThreadTask, i);
	}
	for (auto& t : threads)
	{
		t.join();
	}

}

int main()
{
	memoryPool::HashBucket::initMemoryPool();
	testSingleThread();
	testMultiThread();

}