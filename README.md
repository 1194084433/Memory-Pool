# Memory Pool - 高性能内存池


## 📖 简介

Memory Pool 是一个高性能的 C++ 内存池实现，通过预分配大块内存并自定义内存管理策略，显著减少频繁 `new/delete` 带来的性能开销和内存碎片。

**V1.0 核心特性：**
- ✅ 基于 Hash Bucket 的空闲块管理
- ✅ 线程安全（`std::mutex` 保护）
- ✅ 支持任意类型对象构造/析构
- ✅ 零内存碎片（固定大小块分配）
- ✅ RAII 自动内存管理

## 🚀 快速开始

### 环境要求
- C++11 及以上编译器（MSVC / GCC / Clang）
- CMake 3.10+（可选）

### 使用示例

```cpp
#include "memory-pool.h"

// 1. 初始化内存池
memoryPool::HashBucket::initMemoryPool();

// 2. 分配对象（自动构造）
TestClass* obj = memoryPool::HashBucket::newElement<TestClass>(100);

// 3. 使用对象
obj->print();

// 4. 释放对象（自动析构）
memoryPool::HashBucket::deleteElement(obj);
```


## 实现细节


### class MemoryPool


#### 成员变量
int BlockSize_ :内存块大小
int SlotSize_:槽大小
Slot* firstBlock_:指向内存池管理的首个实际内存块
Slot* curSlot_:当前未被使用的槽
Slot* freeList_:指向空闲的槽
Slot* lastSlot_:指向当前内存块中最后能存放元素的位置


#### 成员函数
##### MemoryPool(size_t BlockSize=4096)
功能：构造函数，根据所需大小创建内存池


##### ~MemoryPool()
功能：析构函数，释放被分配的内存
实现：通过cur指针从firstBlock_开始往后遍历所有Slot，依次释放空间


##### void init(size_t size)
功能：初始化MemoryPool


##### void* allocate()
功能：分配新的Slot，返回指向当前可用Slot指针
实现：
(1) 通过freeList_判断当前是否有空闲Slot，若有，返回指向空闲Slot的指针
(2) 若freeList_指向空，先判断当前块是否已经满了，若满了则调用allocateNewBlock()来分配新的块。
(3) 更新curSlot指向下一个可用Slot的首地址，返回被使用Slot的首地址。


##### void deallocate()
功能：移动指针，但不释放当前Slot内存


##### void allocateNewBlock()
功能：分配新Block
实现：
(1)更新firstBlock_指向新Block的首字节
(2)利用padPointer()函数实现curSlot_的字节对齐
(3)更新lastSlot与freeList_指针


##### padPointer(char* p,size_t align)
功能：实现字节对齐


### class HashBucket
#### 成员函数
##### getMemoryPool(int index)
功能；初始化整个HashBucket，根据索引访问对应内存池
实现：创建静态数组memoryPool[MEMORY_POOL_NUM]，通过索引访问。


##### void initMemoryPool()
功能：初始化HashBucket，令memorypool中的Slot大小有规律


##### void* useMemory(size_t)
功能：分配内存空间
实现：
(1)根据内存大小判断，超过512字节；用new
(2)通过getMemoryPool获取具有合适Slot大小的内存池进行内存分配


##### void freeMemory()
功能：释放内存


##### newElement(Args&&... args)
功能：提供用户使用内存接口
实现：用模板函数接受不同类型，调用useMemory()函数分配内存


##### void deleteElement(T* p)
功能：释放当前内存