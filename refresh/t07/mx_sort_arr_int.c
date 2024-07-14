void mx_sort_arr_int(int *arr, int size) {
    int temp;
    int flag = 1;
    while (flag) {
        flag = 0;
        for (int i = 0; i < size - 1; i++) {
            if (arr[i] > arr[i + 1]) {
                temp = arr[i];
                arr[i] = arr[i + 1];
                arr[i + 1] = temp;
                flag = 1;
            }
        }
    }
}
