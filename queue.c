#include "queue.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "harness.h"
#include "natsort/strnatcmp.h"
/* private functions for sort */
static list_ele_t *merge(list_ele_t *left, list_ele_t *right);
static list_ele_t *merge_sort(list_ele_t *head);

/*  conditional compilation for merge sort */
#define RECURSIVE_MERGEx
/*
 * Create empty queue.
 * Return NULL if could not allocate space.
 */
queue_t *q_new()
{
    queue_t *q = malloc(sizeof(queue_t));
    if (!q)
        return NULL;
    q->size = 0;
    q->head = NULL;
    q->tail = NULL;
    return q;
}

/* Free all storage used by queue */
void q_free(queue_t *q)
{
    if (!q)
        return;
    while (q->head) {
        list_ele_t *tmp = q->head;
        q->head = q->head->next;
        free(tmp->value);
        free(tmp);
    }
    free(q);
}

/*
 * Attempt to insert element at head of queue.
 * Return true if successful.
 * Return false if q is NULL or could not allocate space.
 * Argument s points to the string to be stored.
 * The function must explicitly allocate space and copy the string into it.
 */
bool q_insert_head(queue_t *q, char *s)
{
    list_ele_t *newh;
    if (!q)
        return false;
    newh = malloc(sizeof(list_ele_t));
    if (!newh)
        return false;
    newh->value = malloc(sizeof(char) * (strlen(s) + 1));
    if (!newh->value) {
        free(newh);
        return false;
    }
    snprintf(newh->value, strlen(s) + 1, "%s", s);
    newh->next = q->head;
    q->head = newh;
    if (!q->tail) {
        q->tail = newh;
    }
    q->size++;
    return true;
}

/*
 * Attempt to insert element at tail of queue.
 * Return true if successful.
 * Return false if q is NULL or could not allocate space.
 * Argument s points to the string to be stored.
 * The function must explicitly allocate space and copy the string into it.
 */
bool q_insert_tail(queue_t *q, char *s)
{
    list_ele_t *newt;
    if (!q)
        return false;
    newt = malloc(sizeof(list_ele_t));
    if (!newt)
        return false;
    newt->value = malloc(sizeof(char) * (strlen(s) + 1));
    if (!newt->value) {
        free(newt);
        return false;
    }
    snprintf(newt->value, strlen(s) + 1, "%s", s);
    newt->next = NULL;
    q->size++;
    if (!q->tail) {
        q->head = newt;
        q->tail = newt;
        return true;
    }
    q->tail->next = newt;
    q->tail = newt;
    return true;
}

/*
 * Attempt to remove element from head of queue.
 * Return true if successful.
 * Return false if queue is NULL or empty.
 * If sp is non-NULL and an element is removed, copy the removed string to *sp
 * (up to a maximum of bufsize-1 characters, plus a null terminator.)
 * The space used by the list element and the string should be freed.
 */
bool q_remove_head(queue_t *q, char *sp, size_t bufsize)
{
    if (!q || !q->head)
        return false;
    list_ele_t *tmp;
    tmp = q->head;
    if (sp) {
        snprintf(sp, bufsize, "%s", tmp->value);
    }
    q->head = q->head->next;
    free(tmp->value);
    free(tmp);
    q->size--;
    return true;
}

/*
 * Return number of elements in queue.
 * Return 0 if q is NULL or empty
 */
int q_size(queue_t *q)
{
    if (!q)
        return 0;
    return q->size;
}

/*
 * Reverse elements in queue
 * No effect if q is NULL or empty
 * This function should not allocate or free any list elements
 * (e.g., by calling q_insert_head, q_insert_tail, or q_remove_head).
 * It should rearrange the existing ones.
 */
void q_reverse(queue_t *q)
{
    if (!q || q->size == 0)
        return;
    /*idea : move all the elements behide to the tail */

    list_ele_t *tmp;
    q->tail->next = q->head;

    while (q->head->next != q->tail) {
        tmp = q->head->next;
        q->head->next = tmp->next;
        tmp->next = q->tail->next;
        q->tail->next = tmp;
    }
    q->tail = q->head;
    q->head = q->head->next;
    q->tail->next = NULL;
}

/*
 * Sort elements of queue in ascending order
 * No effect if q is NULL or empty. In addition, if q has only one
 * element, do nothing.
 */
void q_sort(queue_t *q)
{
    if (!q || !q->head) /* ignore NULL and empty queue */
        return;
    if (!q->head->next)
        return;
    q->head = merge_sort(q->head);
    while (q->tail->next) { /* update the tail pointer */
        q->tail = q->tail->next;
    }
}

/* merge sort algorithm */
static list_ele_t *merge_sort(list_ele_t *head)
{
    /* merge sort */
    if (!head || !head->next)
        return head;
    list_ele_t *slow = head;
    list_ele_t *fast = head->next;
    /* split list */
    while (fast && fast->next) {
        slow = slow->next;
        fast = fast->next->next;
    }
    fast = slow->next;
    slow->next = NULL;
    /* sort each list */
    list_ele_t *left = merge_sort(head);
    list_ele_t *right = merge_sort(fast);
    /* merge sorted left and sorted right */
    return merge(left, right);
}

#ifdef RECURSIVE_MERGE
/* recursive merge */
static list_ele_t *merge(list_ele_t *left, list_ele_t *right)
{
    if (!left)
        return right;
    if (!right)
        return left;

    if (strnatcmp(left->value, right->value) < 0) {
        left->next = merge(left->next, right);
        return left;
    } else {
        right->next = merge(left, right->next);
        return right;
    }
}
#else
/* iterative merge */
static list_ele_t *merge(list_ele_t *left, list_ele_t *right)
{
    if (!left)
        return right;
    if (!right)
        return left;

    list_ele_t *head = NULL; /* pseudo head */
    list_ele_t *tmp = NULL;
    /* decide the first element and use it as pseudo head */
    if (strnatcmp(left->value, right->value) < 0) {
        head = left;
        left = left->next;
    } else {
        head = right;
        right = right->next;
    }
    /* merge remaining elements to pseudo head */
    tmp = head;
    while (left && right) {
        if (strnatcmp(left->value, right->value) < 0) {
            tmp->next = left;
            left = left->next;

        } else {
            tmp->next = right;
            right = right->next;
        }
        tmp = tmp->next;
    }
    if (left)
        tmp->next = left;
    if (right)
        tmp->next = right;
    return head;
}
#endif