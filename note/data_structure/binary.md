## 认识二分法

以一个有序数组为例，如果想要找某个值，每次都查找最中间的值，如果查找值比中间值小，则在左侧序列继续二分查找，否则在右侧查找。由于每一次都取中间值对比，因此数组的长度 `N = 2 ^ 常数时间操作`，二分法的时间复杂度为O(logN)。

```c
int binarySearch(int nums[], int target, int length)
{
     int mid = 0;
     int left = 0;
     int right = length - 1;

     while (left <= length)
     {
          mid = left + ((right - left) >> 1);
          if (num[mid] == target)
               return;
          else if (nums[mid] < target>)
               left = mid + 1;
          else if (nums[mid] > target)
               right = mid - 1;
     }
     return 0;
}
```

> 注意：
>    1. 取中间值如果直接用 (left + right) / 2 很有可能导致溢出
>    2. 采用位运算比除运算快（右移一位除2，左移一位乘2），如 n*2+1 可以表示为 `((n << 1) | 1)`

### 二分法处理局部最小问题

对于一个无序数组 arr，任意两个相邻的数都不相等，返回任意一个局部最小值。

思路：寻找局部最小值，首先看第一个元素是否比第二个元素小，若果是，则第一个元素为局部最小值，否则之后的序列是先下降后上升的。我们取中间值，如果中间值比左侧值要小，那么整个序列还在下降，局部最小值应该在序列的右侧；如果中间值比左侧值要大，那么这个序列左侧的值一定是先下降后上升的，局部最小值必然在序列的左侧。

```c
int findLocalMinimum(int arr[], int length)
{
     int mid = 0;
     int left = 0;
     int right = length - 1;

     if (length == 1 || arr[left] < arr[left + 1])
     {
          return arr[left];
     }

     while(left <= right)
     {
          mid = left + (right - left) / 2;
          if (arr[mid] >= arr[left])
          {
               right = mid - 1;
          }
          else if (arr[mid] < arr[left])
          {
               left = mid + 1;
          }
     }
     return arr[mid];
}
```

在这个例子中我们可以看到，并不是只有有序才能使用二分法，二分法是一种思路，只要我们有一种排他性的条件即可。

### 寻找左右边界的二分查找

如果要查找的值在数组中有多个元素，而我们要找到这个值最左侧元素或是最右侧元素，那么我们就需要控制找到元素之后是向左寻找还是向右寻找了。

如果是要找左侧边界，那么就需要想做搜索，反之向右搜索。二分搜索就可以更新为：

```c
mid = left + (right - left) / 2;
if (arr[mid] > arr[left])
{
    right = mid - 1;
}
else if (arr[mid] < arr[left])
{
    left = mid + 1;
}
else if (arr[mid] == target)
{
    right = mid - 1;    // 向左侧搜索
    // left = mid + 1;  // 向右侧搜索
}
```