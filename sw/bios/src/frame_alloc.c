#include <bios.h>
#include <stdio.h>
#include <frame_alloc.h>


static u8 first_free_frame;
struct frame frames[NB_FRAMES];

struct frame *alloc_frame(void)
{
	for (u8 i = first_free_frame; i; i = frames[i].next) {
		if (frames[i].ref_cnt == 0) {
			frames[i].ref_cnt++;
			first_free_frame = frames[i].next;
			return &frames[i];
		}
	}
	panic("No memory available");
	return NULL;
}

void free_frame(struct frame *f)
{
	u8 idx = f - frames;

	f->ref_cnt++;
	f->next = first_free_frame;
	first_free_frame = idx;
}

void frame_stats(void)
{
	unsigned int free = 0;

	for (u8 i = first_free_frame; i; i = frames[i].next)
		++free;

	printf("Mem:\n\tUsed: %d x 4K\n\tFree: %d x 4K\n", NB_FRAMES - free, free);
}

void init_frames(void)
{
	for (unsigned int i = 0; i < NB_FRAMES; ++i) {
		frames[i].ref_cnt = 0;
		frames[i].next = i + 1;
	}

	/* BIOS page directory */
	frames[0].ref_cnt++;

	/* BIOS page table */
	frames[1].ref_cnt++;

	first_free_frame = 2;
}
