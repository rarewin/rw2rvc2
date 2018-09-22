#include <stdlib.h>
#include <string.h>

#include "rw2rvc2.h"

/**
 * @brief ベクタにメモリを割り当てる
 */
static struct vector_t *allocate_vector(void)
{
	const size_t ALLOCATE_SIZE = 256;

	static struct vector_t *vector_array = NULL;
	static size_t index = 0;
	static size_t size = 0;

	/* initial allocation */
	if (vector_array == NULL) {
		size += ALLOCATE_SIZE;
		vector_array = (struct vector_t*)malloc(sizeof(struct vector_t) * size);
	}

	/* リサイズ */
	if (index >= size) {
		size *= 2;
		vector_array = (struct vector_t*)realloc(vector_array, sizeof(struct vector_t) * size);
	}

	return &vector_array[index++];
}

const size_t VECTOR_DATA_DEFAULT_CAPACITY = 16;

/**
 * @brief create a new vector
 */
struct vector_t *new_vector(void)
{
	const size_t ALLOCATE_SIZE = 256;

	static void *vector_data_array = NULL;
	static size_t index = 0;
	static size_t size = 0;

	struct vector_t *v = allocate_vector();

	/* initial allocation */
	if (vector_data_array == NULL) {
		size += ALLOCATE_SIZE;
		vector_data_array = (void*)malloc(sizeof(void*) * VECTOR_DATA_DEFAULT_CAPACITY * size);
	}

	/* リサイズ */
	if (index >= size) {
		size *= 2;
		vector_data_array = (void*)realloc(vector_data_array, sizeof(void*) * VECTOR_DATA_DEFAULT_CAPACITY * size);
	}

	v->capacity = VECTOR_DATA_DEFAULT_CAPACITY;
	v->len = 0;
	v->data = ((void*)vector_data_array + sizeof(void*) * VECTOR_DATA_DEFAULT_CAPACITY * index);
	index++;

	return v;
}

/**
 * @brief push an element to a vector
 */
void vector_push(struct vector_t *v, void *element)
{
	if (v->len >= v->capacity) {
		if (v->capacity == VECTOR_DATA_DEFAULT_CAPACITY) {
			void *old = v->data;
			v->capacity *= 2;
			v->data = malloc(sizeof(void*) * VECTOR_DATA_DEFAULT_CAPACITY * v->capacity);
			memcpy(v->data, old, sizeof(void*) * VECTOR_DATA_DEFAULT_CAPACITY);
		} else {
			v->capacity *= 2;
			v->data = realloc(v->data, sizeof(void*) * VECTOR_DATA_DEFAULT_CAPACITY * v->capacity);
		}
	}

	v->data[v->len++] = element;
}
