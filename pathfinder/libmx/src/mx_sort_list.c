#include "libmx.h"

static void swap_data(t_list *list) {
    void *temp_data = list->data;
    list->data = list->next->data;
    list->next->data = temp_data;
}

t_list *mx_sort_list(t_list *lst, bool(*cmp)(void *, void *)) {
    if (lst == NULL || cmp == NULL)
        return lst;

    bool sort_flag = false;
    while (!sort_flag) {
        sort_flag = true;
        for (t_list *i = lst; i->next != NULL; i = i->next) {
            if (cmp(i->data, i->next->data)) {
                swap_data(i);
                sort_flag = false;
            }
        }
    }
    return lst;
}
