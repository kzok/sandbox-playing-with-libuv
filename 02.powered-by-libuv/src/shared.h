#ifndef SHARED_H_INCLUDED
#define SHARED_H_INCLUDED

#include <assert.h>
#include <stdlib.h>

/**
 * container_of - cast a member of a structure out to the containing structure
 * @ptr: the pointer to the member.
 * @type: the type of the container struct this is embedded in.
 * @member: the name of the member within the struct.
 * @see https://github.com/torvalds/linux/blob/e1edc277e6f6dfb372216522dfc57f9381c39e35/include/linux/container_of.h#L10-L22
 */
#define container_of(ptr, type, member) ({ \
  const typeof( ((type *)0)->member ) *__mptr = (ptr); \
  (type *)( (char *)__mptr - offsetof(type,member) );})

/**
 * @brief print one line.
 * 
 */
#define log(...) do{printf(__VA_ARGS__);printf("\n");} while(0);

/**
 * @brief malloc with assertion that the pointer is NOT NULL.
 * 
 * @param size size to allocate
 * @return void* allocated pointer
 */
static void *xmalloc(size_t size) {
  void *ret = malloc(size);
  assert(ret != NULL);
  return ret;
}

/**
 * @brief free and assign NULL if pointer is not NULL
 * 
 * @param p The allocated pointer
 */
static void xfree(void* p) {
  if(p != NULL) {
    free(p);
  }
}

#endif /** include guard */
