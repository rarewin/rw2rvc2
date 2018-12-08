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
		if ((vector_array = (struct vector_t*)realloc(vector_array, sizeof(struct vector_t) * size)) == NULL) {
			color_printf(stderr, COL_RED, "memory allocation failed\n");
			exit(1);
		}
	}

	return &vector_array[index++];
}

const size_t VECTOR_DATA_DEFAULT_CAPACITY = 16;

/**
 * @brief 新規ベクタを生成する
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
 * @brief ベクタ要素をプッシュする
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


/**
 * @brief 辞書を新規に作成する
 */
struct dict_t *new_dict(void)
{
	const size_t ALLOCATE_SIZE = 8;
	const size_t DICT_SIZE = 32;

	static struct dict_t *dict_array = NULL;
	static size_t index = 0;
	static size_t size = ALLOCATE_SIZE;

	struct dict_t *d;

	/* 初期化 */
	if (dict_array == NULL)
		dict_array = (struct dict_t*)malloc(sizeof(struct dict_t) * size);

	/* サイズを拡大する */
	if (index >= size) {
		size *= 2;
		dict_array = (struct dict_t*)realloc(dict_array, sizeof(struct dict_t) * size);
	}

	d = &dict_array[index++];

	d->len = 0;
	d->capacity = DICT_SIZE;
	d->dict = (struct dict_element_t*)malloc(sizeof(struct dict_element_t) * d->capacity);

	return d;
}

/**
 * @brief 辞書にデータを追加する
 */
void dict_append(struct dict_t *d, char *key, void *value)
{
	struct dict_element_t *e;

	/* 既に辞書に存在していたらその値を使う */
	if ((e = dict_lookup(d, key)) != NULL) {
		e->value = value;
		return;
	}

	if (d->len >= d->capacity) {
		d->capacity *= 2;
		d->dict = (struct dict_element_t*)realloc(d->dict, sizeof(struct dict_element_t) * d->capacity);
	}

	(d->dict)[d->len].key = key;
	(d->dict)[d->len].value = value;

	d->len++;
}

/**
 * @brief 辞書からデータを参照する
 */
struct dict_element_t *dict_lookup(struct dict_t *d, char *key)
{
	size_t i;

	for (i = 0; i < d->len; i++) {
		if (strcmp((d->dict)[i].key, key) == 0)
			return &(d->dict)[i];
	}

	return NULL;
}
