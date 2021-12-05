# 排序算法的实现

## 选择排序

选择排序的算法思想是，每一次找到当前序列的最小值，然后将其交换到前面。

```c
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
```

## 冒泡排序

选择排序的算法思想是，每一次将相邻的两个元素排序交换，就相当于每次都把比较小的值往前放，直到排好序。因此每一次都能把当前序列的最大值放在后面。

```c
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
```

可以看到，上述的两个排序算法无论数据的初始状况如何，执行的常数操作都相同，即每次都会遍历相同的次数，其时间复杂度都是O(n^2)。

## 插入排序

插入排序的算法思想是，从前往后依次将数组遍历的值插入（交换）到前面序列合适的位置上。与上述两种排序算法不同的是，插入排序的时间复杂度与数据的初始状态有关，当一个已经排好序的的数据执行插入排序时，其时间复杂度可以达到O(n)；在最差情况下，即数据为倒序进行排序时，其时间复杂度为O(n^2)。

但是在评估算法复杂度时，我们需要按照最差数据情况来计算，因此该算法的时间复杂度为O(n^2)。

```c
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
```

