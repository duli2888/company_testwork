#include <stdio.h>
#include <stdlib.h>
#include "list.h"
#define NAMESIZE 32

struct score
{
	int id;
	char name[NAMESIZE];
	int math;
	struct list_head node;
};

static void print_s(const void *record)
{
	const struct score *r = record;
	printf("[id]=%d, [name]=%s, [math]=%d\n", r->id, r->name, r->math);
}

int main()
{
	LIST_HEAD(head);
	struct list_head *cur;
	int i;
	struct score *tmp, *data;

	for(i = 0; i < 6; i++) {
		tmp = malloc(sizeof(*tmp));
		if (tmp == NULL) {
			return -1;
		}
		tmp->id = i;
		tmp->math = 100 - i;
		snprintf(tmp->name, NAMESIZE, "stu%d", i);
		list_add(&tmp->node, &head);
	}
	__list_for_each(cur, &head)
	{
		data = list_entry(cur, struct score, node);
		print_s(data);
	}

	__list_del(head.next->next->prev,head.next->next->next);

	printf("===========[after delete]====\n");

	__list_for_each(cur, &head)
	{
		data = list_entry(cur, struct score, node);
		print_s(data);
	}
	return 0;

}
