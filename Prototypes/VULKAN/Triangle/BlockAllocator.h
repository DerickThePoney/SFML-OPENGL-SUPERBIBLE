#pragma once

template<size_t Siz, size_t alignment, size_t BlockNum>
class BlockAllocator
{
public:
	BlockAllocator()
	{
		pMemoryBuffer = (unsigned char*)_aligned_malloc(Siz * BlockNum, alignment);
		FillFreeList();
	}

	~BlockAllocator()
	{
		_aligned_free(pMemoryBuffer);
	}

	void* Allocate();
	void Free(void * pMemory);

private:
	void FillFreeList()
	{
		for (size_t i = 0; i < BlockNum; ++i)
		{
			freeList.push(i);
		}
	}

private:
	unsigned char* pMemoryBuffer;
	std::queue<size_t> freeList;
};

template<size_t Siz, size_t alignment, size_t BlockNum>
inline void* BlockAllocator<Siz, alignment, BlockNum>::Allocate()
{
	if (freeList.empty()) return NULL;
	size_t idx = freeList.front();
	freeList.pop();
	return pMemoryBuffer + idx * Siz;
}

template<size_t Siz, size_t alignment, size_t BlockNum>
inline void BlockAllocator<Siz, alignment, BlockNum>::Free(void * pMemory)
{
	size_t idx = ((unsigned char*)pMemory - pMemoryBuffer) / (Siz);
	freeList.push(idx);
}
