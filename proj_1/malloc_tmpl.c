#include "malloc.h"
#include <sys/queue.h>
#include <sys/mman.h>
// #include <pthread.h>
// #include <errno.h>

#define MB_ALIGNMENT (sizeof(void *) * 2) // 16B, alignment in bytes
#define MA_MAXSIZE                                                             \
  (MB_ALIGNMENT * 32768)                // 512KiB, max requested size of memory
#define MA_THRESHOLD (MA_MAXSIZE / 2)   // 256KiB, TODO: for freeing
#define MB_CANARY_SIZE (sizeof(int8_t)) // 1B, after each block
#define MB_CANARY_VALUE 0x00

typedef struct mem_block mem_block_t;     // 32B
typedef struct mem_arena mem_arena_t;     // 16B+8B+8B+32B = 64B
typedef LIST_ENTRY(mem_block) mb_node_t;  // 16B
typedef LIST_ENTRY(mem_arena) ma_node_t;  // 16B
typedef LIST_HEAD(, mem_block) mb_list_t; // 8B
typedef LIST_HEAD(, mem_arena) ma_list_t; // 8B

typedef struct { // 8B for block metadata
  int64_t canary : 8;
  int64_t mb_allocated : 1; // bit field set iff block is allocated
  int64_t mb_size : 55;     // bit field for size
} mb_boundary_tag_t;
#define MB_BTAG_SIZE (sizeof(mb_boundary_tag_t)) // 8B, boundary tag size

#define MB_PAD (align(MB_BTAG_SIZE, MB_ALIGNMENT) - MB_BTAG_SIZE)
// [8B free? + big? + size][>=16B data + prev + next]
struct mem_block {
  mb_boundary_tag_t head_tag;
  union {
    mb_node_t mb_link;   /* link on free block list, valid if block is free */
    uint64_t mb_data[0]; /* user data pointer, valid if block is allocated */
  };
  mb_boundary_tag_t tail_tag;
};

struct mem_arena {
  ma_node_t ma_link;        /* 16B, link on list of all arenas */
  mb_list_t ma_freeblks;    /* 8B,  list of all free blocks in the arena */
  int64_t is_big_block : 1; /* *   [1b ], true if represents block >512KiB */
  int64_t size : 63;        /* ^8B [63b], real arena size */
  int8_t _pad_[MB_PAD];     /* 8B, pad for first block (due to tag) */
  mem_block_t ma_first;     /* 8B+ >=2B, first block in the arena */
};
#define MA_META_SIZE sizeof(mem_arena_t) - sizeof(mem_block_t) // without block

static ma_list_t *arenas __used = &(ma_list_t){};     // list of all arenas
static ma_list_t *big_blocks __used = &(ma_list_t){}; // list of big blocks
static uint32_t claimed_arenas = 0, claimed_big_blocks = 0;

// claims new area via mmap and returns it. Initializes blocks
// size is a requested memory size MB_ALIGNMENT aligned
static mem_arena_t *__claim_area(size_t size) {
  int8_t is_big_block = size > MA_MAXSIZE ? 1 : 0;
  ma_list_t *arenas_list = is_big_block ? big_blocks : arenas;

  size_t needed_size = MA_META_SIZE + size + 2 * MB_BTAG_SIZE;
  int8_t *arena = mmap(NULL, needed_size, PROT_READ | PROT_WRITE,
                       MAP_PRIVATE | MAP_ANONYMOUS, NULL, NULL);

  if (arena == NULL) { // mmap failed
    errno = 0;         // reset errno set by mmap
    return NULL;       //
  }

  // setup arena
  mem_arena_t *new_arena = arena;
  LIST_INSERT_HEAD(arenas_list, new_arena, ma_link);
  LIST_INIT(&new_arena->ma_freeblks);
  LIST_INSERT_HEAD(&new_arena->ma_freeblks, &new_arena->ma_first, mb_link);
  new_arena->is_big_block = is_big_block;
  new_arena->size = align(needed_size, getpagesize()); // mmap returns pages

  // setup block
  new_arena->ma_first.head_tag.canary = // canary
    new_arena->ma_first.tail_tag.canary = MB_CANARY_VALUE;
  new_arena->ma_first.head_tag.mb_allocated = // allocated
    new_arena->ma_first.tail_tag.mb_allocated = 1;
  new_arena->ma_first.head_tag.mb_size = // size
    new_arena->ma_first.tail_tag.mb_size = size;

  claimed_arenas = is_big_block ? claimed_arenas : ++claimed_arenas;
  claimed_big_blocks = is_big_block ? claimed_big_blocks : ++claimed_big_blocks;

  return new_arena;
}

// returns area capable of holding a block of given size (16B aligned). #FIXME:
// włączyć do my_alloc bezpośrednio
static void *__get_area(size_t size) {
  if (claimed_arenas == 0 || size > MA_MAXSIZE)
    return __claim_area(size);
}

/* This procedure is called before any allocation happens. */
__constructor void __malloc_init(void) {
  __malloc_debug_init();
  LIST_INIT(arenas);
  LIST_INIT(big_blocks);
}

void *__my_malloc(size_t size) {
  debug("%s(%ld)", __func__, size);
  if (size == 0)
    return NULL;

  size =
    align(size, MB_ALIGNMENT); // alligns size to multiplies of MB_ALIGNMENT
  // TODO: if (size <= 8*MB_ALIGNMENT) => bitmap handling

  mem_arena_t *arena = __get_area(size);
  if (arena == NULL)
    return NULL;

  // FIXME: potrzebna logika na przechodzenie listy aren i listy bloków w
  // arenach i na przekraczanie granic. Może wywalić __get_area na rzecz logiki
  // w mallocu bezpośrednio
  mem_block_t *block;
  LIST_FOREACH(block, &arena->ma_freeblks, mb_link) {
    if (block->head_tag.canary != MB_CANARY_VALUE ||
        block->tail_tag.canary != MB_CANARY_VALUE) { // FIXME:
      errno = EILSEQ;
      return NULL;
    } else if (block->head_tag.mb_size >= size) { // first-fit
    }
  }
  return NULL;
}

void *__my_realloc(void *ptr, size_t size) {
  debug("%s(%p, %ld)", __func__, ptr, size);
  return NULL;
}

void __my_free(void *ptr) {
  debug("%s(%p)", __func__, ptr);
}

void *__my_memalign(size_t alignment, size_t size) {
  debug("%s(%ld, %ld)", __func__, alignment, size);
  return NULL;
}

size_t __my_malloc_usable_size(void *ptr) {
  debug("%s(%p)", __func__, ptr);
  return 0;
}

/* DO NOT remove following lines */
__strong_alias(__my_free, cfree);
__strong_alias(__my_free, free);
__strong_alias(__my_malloc, malloc);
__strong_alias(__my_malloc_usable_size, malloc_usable_size);
__strong_alias(__my_memalign, aligned_alloc);
__strong_alias(__my_memalign, memalign);
__strong_alias(__my_realloc, realloc);
