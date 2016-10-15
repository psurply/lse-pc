#ifndef FRAME_ALLOC_H
#define FRAME_ALLOC_H

#include <memory.h>
#include <types.h>

#define NB_FRAMES 256
#define frame_to_phys(f)	((((f) - frames) << PAGE_OFFSET) + EXTERNAL_RAM)
#define frame_to_virt(f)	((void *) (frame_to_phys(f) + BIOS_BASE))
#define phys_to_frame(p)	(&frames[((p) - EXTERNAL_RAM) >> PAGE_OFFSET])
#define virt_to_frame(v)	phys_to_frame((u32) (v) & ~BIOS_BASE)

struct frame {
	u8 ref_cnt;
	u8 next;
};

extern struct frame frames[];

void init_frames(void);
struct frame *alloc_frame(void);
void free_frame(struct frame *f);
void frame_stats(void);

#endif /* FRAME_ALLOC_H */
