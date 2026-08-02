#include<mutex>
#include<cstddef>
#include<utility>
namespace memoryPool
{
#define MEMORY_POOL_NUM 64
#define SLOT_BASE_SIZE 8
#define MAX_SLOT_SIZE 512

struct Slot
{
	Slot* next;
};

class MemoryPool
{
public:
	MemoryPool(size_t BlockSize = 4096);
	~MemoryPool();

	void init(size_t);
	void* allocate();
	void deallocate(void*);

private:
	void allocateNewBlock();
	size_t padPointer(char* p, size_t align);

private:
	int BlockSize_;//内存块大小
	int SlotSize_;//槽大小
	Slot* firstBlock_;//内存池管理的首个实际内存块
	Slot* curSlot_;//当前未被使用的槽
	Slot* freeList_;//指向空闲的槽
	Slot* lastSlot_;//作为当前内存块中最后能够存放元素的位置标识
	std::mutex mutexForFreeList_;
	std::mutex mutexForBlock_;
};


class HashBucket
{
public:
	static void initMemoryPool();
	static MemoryPool& getMemoryPool(int index);

	static void* useMemory(size_t size)
	{
		if (size <= 0)
			return nullptr;
		if (size > MAX_SLOT_SIZE)
			return operator new(size);//只分配原始内存，不构造对象

		return getMemoryPool(((size + 7) / SLOT_BASE_SIZE) - 1).allocate();
	}
	static void freeMemory(void* ptr, size_t size)
	{
		if (!ptr)
			return;
		if (size > MAX_SLOT_SIZE)
		{
			operator delete(ptr);
			return;
		}

		getMemoryPool(((size + 7) / SLOT_BASE_SIZE) - 1).deallocate(ptr);

	}

	template<typename T,typename... Args>
	static T* newElement(Args&&... args)
	{
		T* p = nullptr;

		if ((p = reinterpret_cast<T*>(HashBucket::useMemory(sizeof(T)))) != nullptr)
			new(p) T(std::forward<Args>(args)...);
		return p;
	}

	template<typename T>
	static void deleteElement(T* p)
	{
		p->~T();

		HashBucket::freeMemory(reinterpret_cast<void*>(p), sizeof(T));//reinterpret_cast强制类型转换

	}

};
}