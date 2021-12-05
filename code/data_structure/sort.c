#include <stdio.h>

/* 交换数据 */
void swap(int *arr, int left, int right)
{
    int temp;

    if (arr == NULL || left == right)
    {
        return;
    }

    temp = arr[left];
    arr[left] = arr[right];
    arr[right] = temp;
}

/* 选择排序 */
void selectSort(int arr[], int length)
{
    if (length < 2)
    {
        return;
    }

    for (int i = 0; i < length; ++i)
    {
        int minIndex = i;
        for (int j = i + 1; j < length; ++j)
        {
            minIndex = arr[j] < arr[minIndex] ? j : minIndex;
        }
        swap(arr, i, minIndex);
    }
}

/* 冒泡排序 */
void bubbleSort(int arr[], int length)
{
    if (length < 2)
    {
        return;
    }

    for (int i = length - 1; i > 0; i--)
    {
        for (int j = 0; j < i; ++j)
        {
            if (arr[j] > arr[j+1])
            {
                swap(arr, j, j+1);
            }
        }
    }
}

/* 插入排序 */
void insertSort(int arr[], int length)
{
    if (length < 2)
    {
        return;
    }

    for (int i = 1; i < length - 1; ++i)
    {
        for (int j = i - 1; j >= 0 && arr[j] > arr[j+1]; j--)
        {
            swap(arr, j, j+1);
        }
    }
}