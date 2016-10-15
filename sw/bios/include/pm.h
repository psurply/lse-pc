#ifndef PM_H_
# define PM_H_

#define CR0_PE	1

#define CODE_TYPE	0xA
#define DATA_TYPE	0x2

/* Get a segment selector */
# define SELECTOR(Entry, Rpl)	((Entry << 3) + Rpl)
# define CODE_SELECTOR		SELECTOR(1, 0)
# define DATA_SELECTOR		SELECTOR(2, 0)

# define SET_LIMIT(Entry, Limit)	\
	((Entry)			\
	 | ((Limit) & 0xFFFF)		\
	 | (((Limit) & 0xF0000) << 32))

# define SET_BASE(Entry, Base)			\
	((Entry)				\
	 | (((Base) & 0xFFFF) << 16)		\
	 | (((Base) & 0xFF0000) << 16)		\
	 | (((Base) & 0xFF000000) << 32))

# define SET_TYPE(Entry, Type)		((Entry) | (((Type) & 0xF) << 40))
# define SET_DESCR_TYPE(Entry, Descr)	((Entry) | (((Descr) & 0x1) << 44))
# define SET_DPL(Entry, DPL)		((Entry) | (((DPL) & 0x3) << 45))
# define SET_PRESENT(Entry, P)		((Entry) | (((P) & 0x1) << 47))
# define SET_AVL(Entry, Avl)		((Entry) | (((Avl) & 0x1) << 52))
# define SET_OP_SIZE(Entry, Op)		((Entry) | (((Op) & 0x1) << 54))
# define SET_GRANULARITY(Entry, Gran)	((Entry) | (((Gran) & 0x1) << 55))

/* Get a GDT entry from the given parameters */
# define GDT_ENTRY(Limit, Base, Type, Descr, DPL, Avl, Op, Gran)	\
	(SET_LIMIT(0, Limit)		\
	 | SET_BASE(0, Base)		\
	 | SET_TYPE(0, Type)		\
	 | SET_DESCR_TYPE(0, Descr)	\
	 | SET_DPL(0, DPL)		\
	 | SET_PRESENT(0, 1)		\
	 | SET_AVL(0, Avl)		\
	 | SET_OP_SIZE(0, Op)		\
	 | SET_GRANULARITY(0, Gran))

#endif /* !PM_H_ */
