
#ifndef BLOCK_H
#define BLOCK_H

#define BLOCK_LIST( f ) f( 0, NULL ) f( 1, "b1x1x1r" ) f( 2, "b1x1x1g" ) f( 3, "b1x1x1b" )

#define BLOCK_MAX 4

#define BLOCK_LIST_FILENAMES( idx, filename ) filename,

/*! \brief Width of a single 1x1 block bitmap. */
#define BLOCK_PX_W               16
/*! \brief Height of a single 1x1 block bitmap. */
#define BLOCK_PX_H               16
/*! \brief Isometric offset modifier for drawing blocks. */
#define BLOCK_PX_OFFSET          8

#ifdef BLOCKS_C
const char* gc_block_filenames[] = {
   BLOCK_LIST( BLOCK_LIST_FILENAMES )
};
#else
extern const char* gc_block_filenames[];
#endif /* BLOCKS_C */

#endif /* !BLOCK_H */

