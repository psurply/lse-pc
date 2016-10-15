#ifndef RESET_H_
# define RESET_H_

# include <memory.h>

/* Value used for each segment registers before switching to protected mode */
# define SEGMENT	0xF000

/* Base stack address relative to the segment */
# define BASE_SEG_STACK	(BASE_STACK - (SEGMENT << 4))

/* Base code address relative to the segment */
# define BASE_SEG_CODE	(BASE_CODE - (SEGMENT << 4))

#endif /* !RESET_H_ */
