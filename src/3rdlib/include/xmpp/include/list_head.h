/*
 * DESCRIPTION:	Copyright(c) 2012-2012 Xiamen Yealink Network Technology Co,.Ltd
 * AUTHOR:	dsw
 * HISTORY:
 * DATE:	2012-8-10
 */

#ifndef __LIST_HEAD_H
#define __LIST_HEAD_H

/* See include/linux/list.h */

/*@@@ Sweid
 * modify new to _new to prevent g++ compiler errors
 */
#undef offsetof
#define offsetof(TYPE,MEMBER) ((size_t) &((TYPE *)0)->MEMBER)

/**
 * container_of - cast a member of a structure out to the containing structure
 * @ptr:	the pointer to the member
 * @type:	the type of the container struct this is embedded in.
 * @member:	the name of the member within the struct.
 *
 */
#define container_of(ptr, type, member)  ({	\
	    const typeof( ((type *)0)->member ) *__mptr = (ptr);	\
	    (type *)( (char *)__mptr - offsetof(type,member) ); })


/*
 * These are non-NULL pointers that will result in page faults
 * under normal circumstances, used to verify that nobody uses
 * non-initialized list entries.
 */
struct list_head;

#ifdef __cplusplus
#define LIST_POISON1	((struct list_head *) 0x00100100)
#define LIST_POISON2	((struct list_head *) 0x00200200)
#else
#define LIST_POISON1	((void *) 0x00100100)
#define LIST_POISON2	((void *) 0x00200200)
#endif

/*
 * Simple doubly linked list implementation
 */

struct list_head
{
    struct list_head *next, 
		     *prev;
};

#define LIST_HEAD_INIT(name) { &(name), &(name) }
#define LIST_HEAD(name)	struct list_head name = LIST_HEAD_INIT(name)

static	inline	void	INIT_LIST_HEAD(struct list_head *list)
{
    list->next = list;
    list->prev = list;
}

/*
 * Insert a new entry between two known consecutive entries.
 *
 * This is only for internal list manipulication where we known
 * the prev/next entries already!
 */
static	inline	void	__list_add(struct list_head *_new, 
				   struct list_head *prev,
				   struct list_head *next)
{
    next->prev = _new;
    _new->next = next;
    _new->prev = prev;
    prev->next = _new;
}

/**
 * list_add - add a new entry
 * @_new: new entry to be added
 * @head: list head to add it after
 *
 * Insert a new entry after the specified head.
 * This is good for implementing stacks.
 */
static	inline	void	list_add(struct list_head *_new, struct list_head *head)
{
    __list_add(_new, head, head->next);
}

/**
 * list_add_tail - add a _new entry
 * @_new: _new entry to be added
 * @head: list head to add it before
 *
 * Insert a _new entry before the specified head.
 * This is userful for implementing queues.
 */
static	inline	void	list_add_tail(struct list_head *_new, struct list_head *head)
{
    __list_add(_new, head->prev, head);
}

/*
 * Delete a list entry by making the prev/next entries
 * point to each other.
 *
 * This is only for internal list manipulation where we known
 * the prev/next entries already!
 */
static	inline	void	__list_del(struct list_head *prev, struct list_head * next)
{
    next->prev  = prev;
    prev->next = next;
}

/**
 * list_del - deletes entry from list.
 * @entry: the eleemnt to delete from the list.
 * Note: list_empty on entry does not return true after this, the entry is
 * in an undefined state.
 */
static	inline	void	list_del(struct list_head *entry)
{
    __list_del(entry->prev, entry->next);
    entry->next = LIST_POISON1;
    entry->prev = LIST_POISON2;
}

/**
 * list_replace - replace old entry by _new one
 * @old: the element to be replaced
 * @_new: the _new element to insert
 * Note: if 'old' was empty, it will be overwritten.
 */
static	inline	void	list_replace(struct list_head *old, struct list_head *_new)
{
    _new->next = old->next;
    _new->next->prev = _new;
    _new->prev = old->prev;
    _new->prev->next = _new;
}

/**
 * list_del_init - deletes entry from list and reinitialize it.
 * @entry: the element to delete from the list.
 */
static	inline	void	list_replace_init(struct list_head *old, struct list_head *_new)
{
    list_replace(old, _new);
    INIT_LIST_HEAD(old);
}

/**
 * list_move - delete from one list and add as another's head
 * @list: the entry to move
 * @head: the head that will precede our entry
 */
static	inline	void	list_move(struct list_head *list, struct list_head *head)
{
    __list_del(list->prev, list->next);
    list_add(list, head);
}

/**
 * list_move_tail - delete from one list and add as another's tail
 * @list: list: the entry to move
 * @head: the head that will follow our entry
 */
static	inline	void	list_move_tail(struct list_head *list, struct list_head *head)
{
    __list_del(list->prev, list->next);
    list_add_tail(list, head);
}

/**
 * list_is_last - tests whether @list is the last entry in list @head
 * @list: the entry to test
 * @head: the head of the list
 */
static	inline	int	list_is_last(const struct list_head *list, 
				     const struct list_head *head)
{
    return list->next == head;
}

/**
 * list_empty - tests whether a list is empty
 * @head: the list to test.
 */
static	inline	int	list_empty(const struct list_head *head)
{
    return head->next == head;
}

/**
 * list_is_singular - tests whether a list has just one entyr.
 * @head: the list to test.
 */
static	inline	int	list_is_singular(const struct list_head *head)
{
    return !list_empty(head) && (head->next == head->prev);
}

static	inline	void	__list_splice(struct list_head *list, 
				      struct list_head *head)
{
    struct list_head *first = list->next;
    struct list_head *last = list->prev;
    struct list_head *at = head->next;

    first->prev = head;
    head->next = first;

    last->next = at;
    at->prev = last;
}

/**
 * list_splice - join two lists
 * @list: the _new list to add.
 * @head: the place to add it in the first list.
 */
static inline void list_splice(struct list_head *list, struct list_head *head)
{
    if(!list_empty(list))
	__list_splice(list, head);
}


/**
 * list_splice_init - join two lists and reinitialize the emptied list.
 * @list: the _new list to add.
 * @head: the place to add it in the first list.
 *
 * The list at @list is reinitialized
 */
static inline void list_splice_init(struct list_head *list, struct list_head *head)
{
    if(!list_empty(list)) 
    {
	__list_splice(list, head);
	INIT_LIST_HEAD(list);
    }
}


/**
 * list_entry - get the struct for this entry
 * @ptr:	the &struct list_head pointer
 * @type:	the type of the struct this is embedded in.
 * @member:	the name of the list_struct within the struct.
 */
#define	list_entry(ptr, type, member) container_of(ptr, type, member)

/**
 * list_first_entry - get the first element from a list
 * @ptr:	the list head to take the element from.
 * @type:	the type of the struct this is embedded in.
 * @member:	the name of the list_struct within the struct.
 *
 * Note, that list is expected to be not empty.
 */
#define list_first_entry(ptr, type, member) list_entry((ptr)->next, type, member)


/**
 * list_for_each	-	iterate over a list
 * @pos:	the &struct list_head to use as a loop cursor.
 * @head:	the head for your list.
 *
 * Note:	the the original linux kernel list implementation
 *		has prefetch , but I ignore it here
 */
#define list_for_each(pos, head)	\
    for (pos = (head)->next; /*prefetch(pos->next),*/ pos != (head); \
	    pos = pos->next)

/**
 * __list_for_each	-	iterate over a list
 * @pos:	the &struct list_head to use as a loop cursor.
 * @head:	the head for your list.
 *
 * This variant differs from list_for_each() in that it's the
 * simplest possible list iteration code, no prefetching is done.
 * Use this for code that knowns the list to be very short (empty
 * or 1 entry) most of the time.
 */
#define __list_for_each(pos, head)	\
    for (pos = (head)->next; pos != (head); pos = pos->next)


/**
 * list_for_each_prev	-	iterate over a list backwards
 * @pos:	the &struct list_head to use as a loop cursor.
 * @head:	the head for your list.
 *
 * Note:	the original linux kernel list implementation
 *		has prefetch, but here I ignore it
 */
#define list_for_each_prev(pos, head) for (pos = (head)->prev; /*prefetch(pos->prev),*/ pos != (head); pos = pos->prev)

/**
 * list_for_each_entry -	iterate over list of given type
 * @pos:	the type * to use as a loop cursor.
 * @head:	the head for your list.
 * @member:	the name of the list_struct within the struct.
 *
 *
 * Note:	the original linux kernel list implementation
 *		has prefetch, but here I ignore it
 */
#define list_for_each_entry(pos, head, member)	\
    for(pos = list_entry((head)->next, typeof(*pos), member);	\
	    /*prefetch(pos->member.next),*/ &pos->member != (head);	\
	    pos = list_entry(pos->member.next, typeof(*pos), member))

/**
 * list_for_each_entry_reverse - iterate backwards over list of given type.
 * @pos:	the type * to use as a loop cursor.
 * @head:	the head for your list.
 * @member:	the name of the list_struct within the struct.
 */
#define list_for_each_entry_reverse(pos, head, member)	\
    for(pos = list_entry((head)->prev, typeof(*pos), member);	\
	/*prefetch(pos->member.prev),*/ &pos->member != (head);	\
	pos = list_entry(pos->member.prev, typeof(*pos), member))



#endif	/* __LIST_HEAD_H */
