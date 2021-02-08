#include "vector.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

void VectorNew(vector *v, int elemSize, VectorFreeFunction freeFn, int initialAllocation)
{
	size_t sz = initialAllocation * elemSize;
	v->elemsize = elemSize;
	v->vec = malloc(sz);
	v->logicalLen = 0;
	v->allocLength = initialAllocation;
}

void VectorDispose(vector *v)
{
	free(v->vec);
}

int VectorLength(const vector *v)
{
	return v->logicalLen;
}

void *VectorNth(const vector *v, int position)
{ 
	assert(position >= 0 && position < v->logicalLen);
	return (char*)v->vec + position * v->elemsize;
}

void VectorReplace(vector *v, const void *elemAddr, int position)
{
	memcpy( VectorNth(v, position), elemAddr, v->elemsize);
}

void VectorInsert(vector *v, const void *elemAddr, int position)
{
	assert(position >= 0 && position <= v->logicalLen);
	VectorAppend(v, elemAddr);
	memmove((char * )v->vec + (position + 1) * v->elemsize, (char*)v->vec + position * v->elemsize, (v->logicalLen - position)* v->elemsize);
	memcpy((char*)v->vec + position * v->elemsize, elemAddr, v->elemsize);
}

void VectorAppend(vector *v, const void *elemAddr)
{
	if (v->logicalLen == v->allocLength) {
		v->allocLength *= 2;
		v->vec = realloc(v->vec, v->allocLength * v->elemsize);
		assert(v->vec != NULL);
	}
	//((char*)v->vec + (v->logicalLen) * v->elemsize) = elemAddr; 
	//指针之间的赋值，要用memcpy.
	memcpy((char*)v->vec + (v->logicalLen) * v->elemsize, elemAddr, v->elemsize);
	v->logicalLen++;
}

void VectorDelete(vector *v, int position)
{
	v->logicalLen--;
	memmove((char *)v->vec + position * v->elemsize, (char*)v->vec + (position+1) * v->elemsize, (v->logicalLen - position) * v->elemsize);
}

void VectorSort(vector *v, VectorCompareFunction compare)
{
	//插入排序，参考《算法》
	int N = v->logicalLen;
	char* temp = (char*)malloc(v->elemsize);
	assert(temp != NULL);
	for (int i = 1; i < N; ++i) {
		for (int j = i;j > 0 && compare((char *)v->vec + j * v->elemsize, (char*)v->vec + (j - 1) * v->elemsize) < 0;--j) {
			//交换
			memcpy(temp, (char*)v->vec + j * v->elemsize, v->elemsize);
			memcpy((char*)v->vec + j * v->elemsize, (char*)v->vec + (j - 1) * v->elemsize, v->elemsize);
			memcpy((char*)v->vec + (j-1) * v->elemsize, temp, v->elemsize);
		}
	}
	free(temp);
}

void VectorMap(vector *v, VectorMapFunction mapFn, void *auxData)
{
	assert(mapFn != NULL);
	for (int i = 0; i < v->logicalLen; ++i) {
		mapFn((char*)v->vec + i * v->elemsize, auxData);
	}
	
}

static const int kNotFound = -1;
int VectorSearch(const vector *v, const void *key, VectorCompareFunction searchFn, int startIndex, bool isSorted)
{ 
	if (isSorted) {
		void* left = v->vec, * right = (char*)v->vec + v->elemsize * v->logicalLen;
		while (left <= right) //这里比较的是left，right的地址
		{
			void* mid = (char*)left + ((char*)right - (char*)left) / 2;
			if (!searchFn(mid, key)) {
				return ((char*)mid - (char*)v->vec) / v->elemsize;
			}
			else if (searchFn(mid, key) > 0) {
				right = (char*)mid - v->elemsize;
			}
			else {
				left = (char*)mid + v->elemsize;
			}
		}
	}
	else {
		for (int i = 0; i < v->logicalLen; ++i) {
			if (!searchFn((char*)v->vec + i * v->elemsize, key)) {
				return i;
			}
		}
	}
	
	return -1; 
} 
