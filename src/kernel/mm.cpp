#include <kernel/list.hpp>
#include <kernel/mm.hpp>
#include <kernel/raii.hpp>
#include <kernel/spinlock.hpp>
#include <kernel/types.hpp>

void* operator new(usize size)
{
    void* ptr = kmem_alloc(size);
    return ptr;
}

void* operator new[](usize size)
{
    void* ptr = kmem_alloc(size);
    return ptr;
}

void* operator new(usize size, void* ptr)
{
    return ptr;    
}

void* operator new[](usize size, void* ptr)
{
    return ptr;
}

void operator delete(void* ptr) noexcept		{ kmem_free(ptr); }
void operator delete(void* ptr, usize size) noexcept	{ kmem_free(ptr); }
void operator delete[](void* ptr) noexcept		{ kmem_free(ptr); }
void operator delete[](void* ptr, usize size) noexcept	{ kmem_free(ptr); }

static constexpr u32 KMEM_MIN_BLOCK_SIZE	= 16;
static constexpr u32 KMEM_HEADER_SIZE		= 8;

static constexpr u32 KMEM_INUSE			= (1 << 0);
static constexpr u32 KMEM_PREV_INUSE		= (1 << 1);
static constexpr u32 KMEM_SIZE_MASK		= ~0x7;

struct Block
{    
    u32 prev_size;
    u32 size_flags;
    list_head free_list;
};

static Spinlock kmem_lock;
static List<Block, &Block::free_list> kmem_bins[28];

static inline uint indexof(usize size)
{
    return __builtin_ctz(size) - 4;
}

void* kmem_alloc(usize size)
{    
    usize min_size = (size + 0x7) & 0x7;
    uint idx = indexof(min_size);
    LockGuard g { kmem_lock };

    Block* block = nullptr;
    usize best_fit = -1;
    
    for (; idx < 28; idx++)
    {
	for (auto it = kmem_bins[idx].iterator(); it; ++it)
	{
	    usize block_size = it->size_flags & KMEM_SIZE_MASK;
	    isize rem_size = ((isize) block_size) - min_size;
	    if (rem_size < 0)
	    {
		continue;
	    }
	    else if (rem_size < (isize) KMEM_MIN_BLOCK_SIZE)
	    {
		block = &(*it);
		kmem_bins[idx].remove(block);
		block->size_flags = block_size | KMEM_INUSE;
		Block* high = (Block*) (((uptr) block) + block_size);
		high->size_flags |= KMEM_PREV_INUSE;
		
		return (void*) (((uptr) block) + KMEM_HEADER_SIZE);
	    }
	    else if (block_size < best_fit)
	    {
		block = &(*it);
		best_fit = block_size;		
	    }
	}
	if (block != nullptr)
	{
	    usize rem_size = best_fit - min_size;	    
	    kmem_bins[idx].remove(block);
	    block->size_flags = min_size | KMEM_PREV_INUSE | KMEM_INUSE;
	    Block* rem = (Block*) (((uptr) block) + min_size);
	    rem->prev_size = min_size;
	    rem->size_flags = rem_size | KMEM_PREV_INUSE;
	    kmem_bins[indexof(rem_size)].push_front(rem);
	    
	    return (void*) (((uptr) block) + KMEM_HEADER_SIZE);
	}
    }
    
    return nullptr;
}

void kmem_free(void* ptr)
{
    Block* block = (Block*) (((uptr) ptr) - KMEM_HEADER_SIZE);
    usize block_size = block->size_flags & KMEM_SIZE_MASK;
    Block* high = (Block*) (((uptr) block) + block_size);

    LockGuard g { kmem_lock };
    
    if (!(high->size_flags & KMEM_INUSE))
    {
	usize high_size = high->size_flags & KMEM_SIZE_MASK;
	kmem_bins[indexof(high_size)].remove(high);
	block_size += high_size;	
    }
    if (!(block->size_flags & KMEM_PREV_INUSE))
    {
	usize low_size = block->prev_size;
	Block* low = (Block*) (((uptr) block) - low_size);
	kmem_bins[indexof(low_size)].remove(low);
	block_size += low_size;
	block = low;	
    }
    block->size_flags = block_size | KMEM_PREV_INUSE;
    kmem_bins[indexof(block_size)].push_front(block);    
}

void kmem_init(uptr start, uptr end)
{
    Block* block = (Block*) start;
    usize block_size = end - start - KMEM_HEADER_SIZE;
    block->prev_size = 0;
    block->size_flags = block_size | KMEM_PREV_INUSE;

    Block* guard = (Block*) (end - KMEM_HEADER_SIZE);
    guard->prev_size = block_size;
    guard->size_flags = 0 | KMEM_INUSE;

    kmem_bins[indexof(block_size)].push_front(block);
}
