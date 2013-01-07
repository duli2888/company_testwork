

struct list_head {
	struct list_head *next, *prev;
};

#define LIST_HEAD_INIT(name) { &(name), &(name)}

#define LIST_HEAD(name) struct list_head name = LIST_HEAD_INIT(name)


static inline void __list_add(struct list_head *new,
			      struct list_head *prev,
			      struct list_head *next)
{
	next->prev = new;
	new->next = next;
	new->prev = prev;
	prev->next = new;
}

static inline void list_add(struct list_head *new, struct list_head *head)
{
	__list_add(new, head, head->next);
}
#define __list_for_each(pos, head) for(pos = (head)->next; pos != (head); pos = pos->next)


#define offsetof(TYPE, MEMBER) ((size_t) &((TYPE *)0)->MEMBER)

#define container_of(ptr, type, member) ({(type *)((char *)ptr - offsetof(type,member));})


#define list_entry(ptr, type, member) container_of(ptr, type, member)




static inline void __list_del(struct list_head * prev, struct list_head * next)
{
	    next->prev = prev;
	    prev->next = next;
}


