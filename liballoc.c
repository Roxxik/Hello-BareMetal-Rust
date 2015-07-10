/**  Durand's Ridiculously Amazing Super Duper Memory functions.  */
#include <stdint.h>

#include "libBareMetal.h"

#define LIBALLOC_MAGIC	0xc001c0de
#define MAXCOMPLETE		5
#define MAXEXP	32
#define MINEXP	8

#define MODE_BEST			0
#define MODE_INSTANT		1

#define MODE	MODE_BEST

#define NULL ((void*) 0)


/** This is a boundary tag which is prepended to the
 * page or section of a page which we have allocated. It is
 * used to identify valid memory blocks that the
 * application is trying to free.
 */
struct	boundary_tag
{
	unsigned int magic;			//< It's a kind of ...
	unsigned int size; 			//< Requested size.
	unsigned int real_size;		//< Actual size.
	int index;					//< Location in the page table.

	struct boundary_tag *split_left;	//< Linked-list info for broken pages.
	struct boundary_tag *split_right;	//< The same.

	struct boundary_tag *next;	//< Linked list info.
	struct boundary_tag *prev;	//< Linked list info.
};


struct boundary_tag* l_freePages[MAXEXP];		//< Allowing for 2^MAXEXP blocks
int 				 l_completePages[MAXEXP];	//< Allowing for 2^MAXEXP blocks


static int l_initialized = 0;			//< Flag to indicate initialization.
static int l_pageSize  = 4096;			//< Individual page size
static int l_pageCount = 16;			//< Minimum number of pages to allocate.

// *********** BAREMETAL SPECIFIC *******************************
/** This function is supposed to lock the memory data structures. It
 * could be as simple as disabling interrupts or acquiring a spinlock.
 * It's up to you to decide.
 *
 * \return 0 if the lock was acquired successfully. Anything else is
 * failure.
 */
int liballoc_lock() {
    //currently single threaded
    return 0;
}

/** This function unlocks what was previously locked by the liballoc_lock
 * function.  If it disabled interrupts, it enables interrupts. If it
 * had acquiried a spinlock, it releases the spinlock. etc.
 *
 * \return 0 if the lock was successfully released.
 */
int liballoc_unlock() {
    //currently single threaded
    return 0;
}

/** This is the hook into the local system which allocates pages. It
 * accepts an integer parameter which is the number of pages
 * required.  The page size was set up in the liballoc_init function.
 *
 * \return NULL if the pages were not allocated.
 * \return A pointer to the allocated memory.
 */
void* liballoc_alloc(int n) {
    return b_mem_allocate(n);
}

/** This frees previously allocated memory. The void* parameter passed
 * to the function is the exact same value returned from a previous
 * liballoc_alloc call.
 *
 * The integer value is the number of pages to free.
 *
 * \return 0 if the memory was successfully freed.
 */
int liballoc_free(void *mem, int n) {
    int freed = b_mem_release(mem,n);
    return freed != n;
}


// ***********   HELPER FUNCTIONS  *******************************

/** Returns the exponent required to manage 'size' amount of memory.
 *
 *  Returns n where  2^n <= size < 2^(n+1)
 */
static inline int getexp( unsigned int size )
{
	if ( size < (1<<MINEXP) )
	{
		return -1;	// Smaller than the quantum.
	}


	int shift = MINEXP;

	while ( shift < MAXEXP )
	{
		if ( (1<<shift) > size ) break;
		shift += 1;
	}
	return shift - 1;
}


static void* 	liballoc_memset(void* s, int c, uint64_t n)
{
	int i;
	for ( i = 0; i < n ; i++)
		((char*)s)[i] = c;

	return s;
}

static void* 	liballoc_memcpy(void* s1, const void* s2, uint64_t n)
{
  char *cdest;
  char *csrc;
  unsigned int *ldest = (unsigned int*)s1;
  unsigned int *lsrc  = (unsigned int*)s2;

  while ( n >= sizeof(unsigned int) )
  {
      *ldest++ = *lsrc++;
	  n -= sizeof(unsigned int);
  }

  cdest = (char*)ldest;
  csrc  = (char*)lsrc;

  while ( n > 0 )
  {
      *cdest++ = *csrc++;
	  n -= 1;
  }

  return s1;
}



static inline void insert_tag( struct boundary_tag *tag, int index )
{
	int realIndex;

	if ( index < 0 )
	{
		realIndex = getexp( tag->real_size - sizeof(struct boundary_tag) );
		if ( realIndex < MINEXP ) realIndex = MINEXP;
	}
	else
		realIndex = index;

	tag->index = realIndex;

	if ( l_freePages[ realIndex ] != NULL )
	{
		l_freePages[ realIndex ]->prev = tag;
		tag->next = l_freePages[ realIndex ];
	}

	l_freePages[ realIndex ] = tag;
}

static inline void remove_tag( struct boundary_tag *tag )
{
	if ( l_freePages[ tag->index ] == tag ) l_freePages[ tag->index ] = tag->next;

	if ( tag->prev != NULL ) tag->prev->next = tag->next;
	if ( tag->next != NULL ) tag->next->prev = tag->prev;

	tag->next = NULL;
	tag->prev = NULL;
	tag->index = -1;
}


static inline struct boundary_tag* melt_left( struct boundary_tag *tag )
{
	struct boundary_tag *left = tag->split_left;

	left->real_size   += tag->real_size;
	left->split_right  = tag->split_right;

	if ( tag->split_right != NULL ) tag->split_right->split_left = left;

	return left;
}


static inline struct boundary_tag* absorb_right( struct boundary_tag *tag )
{
	struct boundary_tag *right = tag->split_right;

		remove_tag( right );		// Remove right from free pages.

		tag->real_size   += right->real_size;

		tag->split_right  = right->split_right;
		if ( right->split_right != NULL )
					right->split_right->split_left = tag;

	return tag;
}

static inline struct boundary_tag* split_tag( struct boundary_tag* tag )
{
	unsigned int remainder = tag->real_size - sizeof(struct boundary_tag) - tag->size;

	struct boundary_tag *new_tag =
				   (struct boundary_tag*)((unsigned int)tag + sizeof(struct boundary_tag) + tag->size);

						new_tag->magic = LIBALLOC_MAGIC;
						new_tag->real_size = remainder;

						new_tag->next = NULL;
						new_tag->prev = NULL;

						new_tag->split_left = tag;
						new_tag->split_right = tag->split_right;

						if (new_tag->split_right != NULL) new_tag->split_right->split_left = new_tag;
						tag->split_right = new_tag;

						tag->real_size -= new_tag->real_size;

						insert_tag( new_tag, -1 );

	return new_tag;
}


// ***************************************************************




static struct boundary_tag* allocate_new_tag( unsigned int size )
{
	unsigned int pages;
	unsigned int usage;
	struct boundary_tag *tag;

		// This is how much space is required.
		usage  = size + sizeof(struct boundary_tag);

				// Perfect amount of space
		pages = usage / l_pageSize;
		if ( (usage % l_pageSize) != 0 ) pages += 1;

		// Make sure it's >= the minimum size.
		if ( pages < l_pageCount ) pages = l_pageCount;

		tag = (struct boundary_tag*)liballoc_alloc( pages );

		if ( tag == NULL ) return NULL;	// uh oh, we ran out of memory.

				tag->magic 		= LIBALLOC_MAGIC;
				tag->size 		= size;
				tag->real_size 	= pages * l_pageSize;
				tag->index 		= -1;

				tag->next		= NULL;
				tag->prev		= NULL;
				tag->split_left 	= NULL;
				tag->split_right 	= NULL;
      return tag;
}



void *malloc(uint64_t size)
{
	int index;
	void *ptr;
	struct boundary_tag *tag = NULL;

	liballoc_lock();

		if ( l_initialized == 0 )
		{
			for ( index = 0; index < MAXEXP; index++ )
			{
				l_freePages[index] = NULL;
				l_completePages[index] = 0;
			}
			l_initialized = 1;
		}

		index = getexp( size ) + MODE;
		if ( index < MINEXP ) index = MINEXP;


		// Find one big enough.
			tag = l_freePages[ index ];				// Start at the front of the list.
			while ( tag != NULL )
			{
					// If there's enough space in this tag.
				if ( (tag->real_size - sizeof(struct boundary_tag))
								>= (size + sizeof(struct boundary_tag) ) )
				{
					break;
				}

				tag = tag->next;
			}


			// No page found. Make one.
			if ( tag == NULL )
			{
				if ( (tag = allocate_new_tag( size )) == NULL )
				{
					liballoc_unlock();
					return NULL;
				}

				index = getexp( tag->real_size - sizeof(struct boundary_tag) );
			}
			else
			{
				remove_tag( tag );

				if ( (tag->split_left == NULL) && (tag->split_right == NULL) )
					l_completePages[ index ] -= 1;
			}

		// We have a free page.  Remove it from the free pages list.

		tag->size = size;

		// Removed... see if we can re-use the excess space.

		unsigned int remainder = tag->real_size - size - sizeof( struct boundary_tag ) * 2; // Support a new tag + remainder

		if ( ((int)(remainder) > 0) /*&& ( (tag->real_size - remainder) >= (1<<MINEXP))*/ )
		{
			int childIndex = getexp( remainder );

			if ( childIndex >= 0 )
			{
				struct boundary_tag *new_tag = split_tag( tag );

				new_tag = new_tag;	// Get around the compiler warning about unused variables.
			}
		}



	ptr = (void*)((unsigned int)tag + sizeof( struct boundary_tag ) );

	liballoc_unlock();
	return ptr;
}





void free(void *ptr)
{
	int index;
	struct boundary_tag *tag;

	if ( ptr == NULL ) return;

	liballoc_lock();


		tag = (struct boundary_tag*)((unsigned int)ptr - sizeof( struct boundary_tag ));

		if ( tag->magic != LIBALLOC_MAGIC )
		{
			liballoc_unlock();		// release the lock
			return;
		}



		// MELT LEFT...
		while ( (tag->split_left != NULL) && (tag->split_left->index >= 0) )
		{
			tag = melt_left( tag );
			remove_tag( tag );
		}

		// MELT RIGHT...
		while ( (tag->split_right != NULL) && (tag->split_right->index >= 0) )
		{
			tag = absorb_right( tag );
		}


		// Where is it going back to?
		index = getexp( tag->real_size - sizeof(struct boundary_tag) );
		if ( index < MINEXP ) index = MINEXP;

		// A whole, empty block?
		if ( (tag->split_left == NULL) && (tag->split_right == NULL) )
		{

			if ( l_completePages[ index ] == MAXCOMPLETE )
			{
				// Too many standing by to keep. Free this one.
				unsigned int pages = tag->real_size / l_pageSize;

				if ( (tag->real_size % l_pageSize) != 0 ) pages += 1;
				if ( pages < l_pageCount ) pages = l_pageCount;

				liballoc_free( tag, pages );
				liballoc_unlock();
				return;
			}


			l_completePages[ index ] += 1;	// Increase the count of complete pages.
		}


		// ..........


	insert_tag( tag, index );
	liballoc_unlock();
}




void* calloc(uint64_t nobj, uint64_t size)
{
       int real_size;
       void *p;

       real_size = nobj * size;

       p = malloc( real_size );

       liballoc_memset( p, 0, real_size );

       return p;
}



void*   realloc(void *p, uint64_t size)
{
	void *ptr;
	struct boundary_tag *tag;
	int real_size;

	if ( size == 0 )
	{
		free( p );
		return NULL;
	}
	if ( p == NULL ) return malloc( size );

	liballoc_lock();		// lockit
	tag = (struct boundary_tag*)((unsigned int)p - sizeof( struct boundary_tag ));
	real_size = tag->size;
	liballoc_unlock();

	if ( real_size > size ) real_size = size;

	ptr = malloc( size );
	liballoc_memcpy( ptr, p, real_size );
	free( p );

	return ptr;
}
