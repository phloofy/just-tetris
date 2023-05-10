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

void* kmem_alloc(usize size)
{    
    usize min_size = (size + 0x7) & 0x7;
    uint idx = __builtin_ctz(min_size) - 4;
    LockGuard g { kmem_lock };
    for (; idx < 28; idx++)
    {
	Block* block = kmem_bins[idx].pop_front();
	if (block != nullptr)
	{
	    usize block_size = block->size_flags & KMEM_SIZE_MASK;
	    usize rem_size = block_size - min_size;
	    if (rem_size < KMEM_MIN_BLOCK_SIZE)
	    {
		block->size_flags = block_size | KMEM_INUSE;
		Block* high = (Block*) (((uptr) block) + block_size);
		high->size_flags |= KMEM_PREV_INUSE;
	    }
	    else
	    {
		block->size_flags = min_size | KMEM_PREV_INUSE | KMEM_INUSE;
		Block* rem = (Block*) (((uptr) block) + min_size);
		rem->prev_size = min_size;
		rem->size_flags = rem_size | KMEM_PREV_INUSE;
		kmem_bins[__builtin_ctz(rem_size) - 4].push_front(rem);
	    }
	    
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
	kmem_bins[__builtin_ctz(high_size) - 4].remove(high);
	block_size += high_size;	
    }
    if (!(block->size_flags & KMEM_PREV_INUSE))
    {
	usize low_size = block->prev_size;
	Block* low = (Block*) (((uptr) block) - low_size);
	kmem_bins[__builtin_ctz(low_size) - 4].remove(low);
	block_size += low_size;
	block = low;	
    }
    block->size_flags = block_size | KMEM_PREV_INUSE;
    kmem_bins[__builtin_ctz(block_size) - 4].push_front(block);    
}
