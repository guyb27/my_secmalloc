#define _GNU_SOURCE
#include "my_secmalloc.private.h"
#include <stdio.h>
#include <stdlib.h>
#include <alloca.h>
#include <unistd.h>
#include <sys/mman.h>
#include <string.h>
#include <fcntl.h>

struct chunk *heap = NULL;


size_t heap_size = 4096;

#define CHUNK_SIZE sizeof(struct chunk)

struct chunk *init_heap()
{
	if (heap == NULL)
	{
		heap = (struct chunk*) mmap((void*)(BASE_CHUNK_ADDR), 4096, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANON, -1, 0);

		heap->size = heap_size - sizeof(struct chunk);
		heap->flags = FREE;
	}
	return heap;
}
//4:15:00
struct chunk *get_free_chunk_raw(size_t s)
{
	for (struct chunk *item = heap;
			(size_t)item < (size_t)heap + heap_size;
			item = (struct chunk *)((size_t)item + item->size + sizeof(struct chunk))
	)
	{
		if (item->flags == FREE && item->size >= s)
			return item;
	}
	return NULL;
}
//04:22:49
struct chunk *get_last_chunk_raw()
{
	for (
		struct chunk *item = heap;
		(size_t)item < (size_t)heap + heap_size;
		item = (struct chunk*)((size_t)item + sizeof(struct chunk)+item->size)

	)
	{
		printf("Last chunk check %p size %lu - flag %u\n", item, item->size, item->flags);
		if ((size_t)item+sizeof(struct chunk)+item->size>=(size_t)heap+heap_size)
		{
			printf("ret %p\n", item);
			return item;
		}
		printf("skip\n");
	}
	return NULL;
}

struct chunk *get_free_chunk(size_t s)
{
	if (heap == NULL)
		heap = init_heap();
	printf("HEAP %p\n", heap);
	struct chunk *item=get_free_chunk_raw(s);
	if (item == NULL)
	{
		printf("HERE %p S %lu\n", item, s);
		//Manque d espace memoire... REMAP!!!
		size_t tot_chunk = s+sizeof(struct chunk);//TOTAL CHUNKS
		size_t old_size = heap_size;
		size_t delta_size=((tot_chunk/4096)+((tot_chunk%4096!=0)?1:0))*4096;//Nombre total de memoire qui a ete rajouter
		struct chunk *last_item=get_last_chunk_raw();
		heap_size += delta_size;
		printf("HEAP new size %lu\n", heap_size);
		struct chunk *new_heap = mremap(heap, old_size, heap_size, MREMAP_MAYMOVE);
		printf("HEAP resized %p\n", new_heap);
		if (new_heap != heap)
			return NULL;
		printf("LAST_SIZE %lu - %p\n", delta_size, last_item);
		last_item->size+=delta_size;
		printf("last chunk resized %p: %lu\n", last_item, last_item->size);
		item=get_free_chunk_raw(s);
		printf("item chunk %p\n", item);
	}
	return item;
}

void *my_malloc(size_t size)
{
	(void)size;
	void *ptr;
	//get free chunk
	struct chunk *ch = get_free_chunk(size);
	printf("chunk %p\n", ch);
	//split chunk
	ptr=(void*)((size_t)ch + sizeof(struct chunk));
	printf("ptr %p\n", ptr);
	//get end chunk
	struct chunk *end = (struct chunk*)((size_t)ptr+size);
	printf("end %p\n", end);
	end->flags=FREE;
	end->size=ch->size-(sizeof(struct chunk) + size);
	//Assign free chunk
	ch->flags=BUSY;
	ch->size=size;
	return ptr;
}

void my_free(void *ptr)
{
    // Marquez le chunk comme libre
    struct chunk *ch = (struct chunk *)((size_t)ptr-sizeof(struct chunk));
	// ??? si ptr c est n importe quoi ?
	//TODO: lookp ptr?
    ch->flags = FREE;
	// merge les blocks consecutifs
	for (
		struct chunk *item = heap;
		(size_t)item < (size_t)heap + heap_size;
		item = (struct chunk*)((size_t)item + sizeof(struct chunk)+item->size)

	)
	{
		printf("Chunk check %p size %lu - flag %u\n", item, item->size, item->flags);
		if (item->flags == FREE)
		{
			//voir les blocs consecutifs
			struct chunk *end = item;
			size_t new_size = item->size;
			while (end->flags == FREE && (size_t)end+sizeof(struct chunk)+end->size < (size_t)heap+heap_size)
			{
				end=(struct chunk*)((size_t)end+sizeof(struct chunk)+end->size);
				if (end->flags == FREE)
					new_size+=sizeof(struct chunk)+end->size;
				printf("new size: %lu consecutive chunk %p size %lu - flag %u\n", new_size, end, end->size, end->flags);
			}
			item->size = new_size;
		}
	}
}