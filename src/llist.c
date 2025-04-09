#include "llist.h"
#include "llist_internal.h"
#include <stdlib.h>
#include <string.h>
#include <errno.h>

int llist_errno = 0;

/* Helper Functions */
static int lock_list(llist_t *list) {
    if (!list || !list->thread_safe) return 0;
#ifdef _WIN32
    EnterCriticalSection(&list->lock);
#else
    if (pthread_mutex_lock(&list->lock)) return -1;
#endif
    return 0;
}

static int unlock_list(llist_t *list) {
    if (!list || !list->thread_safe) return 0;
#ifdef _WIN32
    LeaveCriticalSection(&list->lock);
#else
    if (pthread_mutex_unlock(&list->lock)) return -1;
#endif
    return 0;
}

static list_node_t *create_node(llist_node_t data) {
    list_node_t *node = malloc(sizeof(list_node_t));
    if (!node) return NULL;
    node->data = data;
    node->next = NULL;
    return node;
}

/* Core Functions */
LLIST_API llist_t *llist_create(unsigned int flags) {
    llist_t *list = malloc(sizeof(llist_t));
    if (!list) {
        llist_errno = LLIST_MALLOC_ERROR;
        return NULL;
    }
    memset(list, 0, sizeof(llist_t));
    list->thread_safe = (flags & MT_SUPPORT_TRUE) ? 1 : 0;

    if (list->thread_safe) {
#ifdef _WIN32
        InitializeCriticalSection(&list->lock);
#else
        if (pthread_mutex_init(&list->lock, NULL)) {
            free(list);
            llist_errno = LLIST_MULTITHREAD_ISSUE;
            return NULL;
        }
#endif
    }
    return list;
}

LLIST_API int llist_destroy(llist_t *list, int destroy_nodes, node_dtor_t destructor) {
    if (!list) {
        llist_errno = LLIST_NULL_ARGUMENT;
        return -1;
    }

    if (lock_list(list)) {
        llist_errno = LLIST_MULTITHREAD_ISSUE;
        return -1;
    }

    list_node_t *current = list->head;
    while (current) {
        list_node_t *next = current->next;
        if (destroy_nodes) {
            if (destructor) destructor(current->data);
            else free(current->data);
        }
        free(current);
        current = next;
    }

    if (list->thread_safe) {
#ifdef _WIN32
        DeleteCriticalSection(&list->lock);
#else
        pthread_mutex_destroy(&list->lock);
#endif
    }
    unlock_list(list);
    free(list);
    return 0;
}

LLIST_API int llist_add_node(llist_t *list, llist_node_t node, int flags) {
    if (!list || !node) {
        llist_errno = LLIST_NULL_ARGUMENT;
        return -1;
    }

    if (lock_list(list)) {
        llist_errno = LLIST_MULTITHREAD_ISSUE;
        return -1;
    }

    list_node_t *new_node = create_node(node);
    if (!new_node) {
        unlock_list(list);
        llist_errno = LLIST_MALLOC_ERROR;
        return -1;
    }

    if (flags & ADD_NODE_FRONT) {
        new_node->next = list->head;
        list->head = new_node;
    } else {
        if (!list->head) {
            list->head = new_node;
        } else {
            list_node_t *current = list->head;
            while (current->next) current = current->next;
            current->next = new_node;
        }
    }
    list->size++;
    unlock_list(list);
    return 0;
}

LLIST_API int llist_insert_node(llist_t *list, llist_node_t node, 
                              node_ident_t identifier, void *arg, int flags) {
    if (!list || !node || !identifier) {
        llist_errno = LLIST_NULL_ARGUMENT;
        return -1;
    }

    if (lock_list(list)) {
        llist_errno = LLIST_MULTITHREAD_ISSUE;
        return -1;
    }

    list_node_t *new_node = create_node(node);
    if (!new_node) {
        unlock_list(list);
        llist_errno = LLIST_MALLOC_ERROR;
        return -1;
    }

    list_node_t *current = list->head;
    list_node_t *prev = NULL;
    int found = 0;

    while (current && !found) {
        if (identifier(current->data, arg)) {
            found = 1;
            break;
        }
        prev = current;
        current = current->next;
    }

    if (!found) {
        free(new_node);
        unlock_list(list);
        llist_errno = LLIST_NODE_NOT_FOUND;
        return -1;
    }

    if (flags & ADD_NODE_BEFORE) {
        if (prev) {
            prev->next = new_node;
        } else {
            list->head = new_node;
        }
        new_node->next = current;
    } else {
        new_node->next = current->next;
        current->next = new_node;
    }
    list->size++;
    unlock_list(list);
    return 0;
}

LLIST_API int llist_remove_node(llist_t *list, node_ident_t identifier, void *arg,
                              int destroy_node, node_dtor_t destructor) {
    if (!list || !identifier) {
        llist_errno = LLIST_NULL_ARGUMENT;
        return -1;
    }

    if (lock_list(list)) {
        llist_errno = LLIST_MULTITHREAD_ISSUE;
        return -1;
    }

    list_node_t *current = list->head;
    list_node_t *prev = NULL;
    int found = 0;

    while (current && !found) {
        if (identifier(current->data, arg)) {
            found = 1;
            break;
        }
        prev = current;
        current = current->next;
    }

    if (!found) {
        unlock_list(list);
        llist_errno = LLIST_NODE_NOT_FOUND;
        return -1;
    }

    if (prev) {
        prev->next = current->next;
    } else {
        list->head = current->next;
    }

    if (destroy_node) {
        if (destructor) destructor(current->data);
        else free(current->data);
    }
    free(current);
    list->size--;
    unlock_list(list);
    return 0;
}

LLIST_API llist_node_t llist_find_node(llist_t *list, node_ident_t identifier, void *arg) {
    if (!list || !identifier) {
        llist_errno = LLIST_NULL_ARGUMENT;
        return NULL;
    }

    if (lock_list(list)) {
        llist_errno = LLIST_MULTITHREAD_ISSUE;
        return NULL;
    }

    list_node_t *current = list->head;
    llist_node_t result = NULL;

    while (current) {
        if (identifier(current->data, arg)) {
            result = current->data;
            break;
        }
        current = current->next;
    }

    if (!result) llist_errno = LLIST_NODE_NOT_FOUND;
    unlock_list(list);
    return result;
}

LLIST_API llist_node_t llist_get_node_at(llist_t *list, unsigned int index) {
    if (!list) {
        llist_errno = LLIST_NULL_ARGUMENT;
        return NULL;
    }

    if (lock_list(list)) {
        llist_errno = LLIST_MULTITHREAD_ISSUE;
        return NULL;
    }

    if (index >= list->size) {
        unlock_list(list);
        llist_errno = LLIST_OUT_OF_RANGE;
        return NULL;
    }

    list_node_t *current = list->head;
    for (unsigned int i = 0; i < index && current; i++) {
        current = current->next;
    }

    unlock_list(list);
    return current ? current->data : NULL;
}

LLIST_API int llist_for_each(llist_t *list, node_func_t action, void *arg) {
    if (!list || !action) {
        llist_errno = LLIST_NULL_ARGUMENT;
        return -1;
    }

    if (lock_list(list)) {
        llist_errno = LLIST_MULTITHREAD_ISSUE;
        return -1;
    }

    int ret = 0;
    list_node_t *current = list->head;
    unsigned int idx = 0;

    while (current && !ret) {
        ret = action(current->data, idx++, arg);
        current = current->next;
    }

    unlock_list(list);
    return ret;
}

LLIST_API llist_node_t llist_get_head(llist_t *list) {
    if (!list) {
        llist_errno = LLIST_NULL_ARGUMENT;
        return NULL;
    }

    if (lock_list(list)) {
        llist_errno = LLIST_MULTITHREAD_ISSUE;
        return NULL;
    }

    llist_node_t result = list->head ? list->head->data : NULL;
    if (!result) llist_errno = LLIST_NODE_NOT_FOUND;
    unlock_list(list);
    return result;
}

LLIST_API llist_node_t llist_get_tail(llist_t *list) {
    if (!list) {
        llist_errno = LLIST_NULL_ARGUMENT;
        return NULL;
    }

    if (lock_list(list)) {
        llist_errno = LLIST_MULTITHREAD_ISSUE;
        return NULL;
    }

    if (!list->head) {
        unlock_list(list);
        llist_errno = LLIST_NODE_NOT_FOUND;
        return NULL;
    }

    list_node_t *current = list->head;
    while (current->next) current = current->next;

    unlock_list(list);
    return current->data;
}

LLIST_API llist_node_t llist_pop(llist_t *list) {
    if (!list) {
        llist_errno = LLIST_NULL_ARGUMENT;
        return NULL;
    }

    if (lock_list(list)) {
        llist_errno = LLIST_MULTITHREAD_ISSUE;
        return NULL;
    }

    if (!list->head) {
        unlock_list(list);
        llist_errno = LLIST_NODE_NOT_FOUND;
        return NULL;
    }

    list_node_t *old_head = list->head;
    llist_node_t data = old_head->data;
    list->head = old_head->next;
    free(old_head);
    list->size--;
    unlock_list(list);
    return data;
}

LLIST_API int llist_size(llist_t *list) {
    if (!list) {
        llist_errno = LLIST_NULL_ARGUMENT;
        return -1;
    }

    if (lock_list(list)) {
        llist_errno = LLIST_MULTITHREAD_ISSUE;
        return -1;
    }

    int size = list->size;
    unlock_list(list);
    return size;
}

LLIST_API int llist_is_empty(llist_t *list) {
    if (!list) return 1;
    
    if (lock_list(list)) return 1;
    
    int empty = (list->size == 0);
    unlock_list(list);
    return empty;
}

LLIST_API int llist_append(llist_t *first, llist_t *second) {
    if (!first || !second) {
        llist_errno = LLIST_NULL_ARGUMENT;
        return -1;
    }

    if (lock_list(first) || lock_list(second)) {
        llist_errno = LLIST_MULTITHREAD_ISSUE;
        return -1;
    }

    if (!second->head) {
        unlock_list(first);
        unlock_list(second);
        return 0;
    }

    if (!first->head) {
        first->head = second->head;
    } else {
        list_node_t *current = first->head;
        while (current->next) current = current->next;
        current->next = second->head;
    }

    first->size += second->size;
    second->head = NULL;
    second->size = 0;

    unlock_list(first);
    unlock_list(second);
    return 0;
}

LLIST_API int llist_reverse(llist_t *list) {
    if (!list) {
        llist_errno = LLIST_NULL_ARGUMENT;
        return -1;
    }

    if (lock_list(list)) {
        llist_errno = LLIST_MULTITHREAD_ISSUE;
        return -1;
    }

    if (list->size <= 1) {
        unlock_list(list);
        return 0;
    }

    list_node_t *prev = NULL;
    list_node_t *current = list->head;
    list_node_t *next = NULL;

    while (current) {
        next = current->next;
        current->next = prev;
        prev = current;
        current = next;
    }

    list->head = prev;
    unlock_list(list);
    return 0;
}

/* Merge sort helpers */
static list_node_t *merge_sorted_lists(list_node_t *a, list_node_t *b, 
                                     node_cmp_t cmp_func, void *arg, int flags) {
    list_node_t dummy = {0};
    list_node_t *tail = &dummy;

    while (a && b) {
        int cmp_result = cmp_func(a->data, b->data, arg);
        int should_swap = (flags & SORT_LIST_ASC) ? (cmp_result > 0) : (cmp_result < 0);
        
        if (should_swap) {
            tail->next = b;
            b = b->next;
        } else {
            tail->next = a;
            a = a->next;
        }
        tail = tail->next;
    }

    tail->next = a ? a : b;
    return dummy.next;
}

static list_node_t *merge_sort(list_node_t *head, node_cmp_t cmp_func, 
                             void *arg, int flags) {
    if (!head || !head->next) return head;

    /* Find middle */
    list_node_t *slow = head;
    list_node_t *fast = head->next;

    while (fast && fast->next) {
        slow = slow->next;
        fast = fast->next->next;
    }

    list_node_t *mid = slow->next;
    slow->next = NULL;

    /* Sort halves */
    list_node_t *left = merge_sort(head, cmp_func, arg, flags);
    list_node_t *right = merge_sort(mid, cmp_func, arg, flags);

    /* Merge */
    return merge_sorted_lists(left, right, cmp_func, arg, flags);
}

LLIST_API int llist_sort(llist_t *list, node_cmp_t cmp_func, void *arg, int flags) {
    if (!list || !cmp_func) {
        llist_errno = LLIST_NULL_ARGUMENT;
        return -1;
    }

    if (lock_list(list)) {
        llist_errno = LLIST_MULTITHREAD_ISSUE;
        return -1;
    }

    if (list->size <= 1) {
        unlock_list(list);
        return 0;
    }

    list->head = merge_sort(list->head, cmp_func, arg, flags);
    unlock_list(list);
    return 0;
}
