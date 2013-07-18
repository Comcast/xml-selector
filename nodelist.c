/**
 * Nodelist implementation
 */

#include "libxq.h"

#include <stdlib.h>
#include <string.h>


// local (private) routines
static xQStatusCode xQNodeList_grow(xQNodeList* list, unsigned long requiredCapacity);


/**
 * Allocate and initialize a new node list
 *
 * Returns a pointer to the node list or 0 on error
 */
xQNodeList* xQNodeList_alloc_init(unsigned long size) {
  xQNodeList* list;
  
  list = (xQNodeList*) malloc(sizeof(xQNodeList));
  if (!list)
    return 0;
  
  if (XQ_OK != xQNodeList_init(list, size)) {
    free(list);
    list = 0;
  }
  
  return list;
}

/**
 * Initialize a newly allocated node list
 *
 * Returns a 0 (XQ_OK) on success, an error code otherwise
 */
xQStatusCode xQNodeList_init(xQNodeList* list, unsigned long size) {
  xmlNodePtr* buff;
  
  list->list = 0;
  list->capacity = 0;
  list->size = 0;

  buff = (xmlNodePtr*) malloc(sizeof(xmlNodePtr) * size);
  if (!buff)
    return XQ_OUT_OF_MEMORY;
  
  list->list = buff;
  list->capacity = size;
  
  return XQ_OK;
}

/**
 * Free a node list previously allocated with xQNodeList_alloc_init
 *
 * Returns a 0 (XQ_OK) on success, an error code otherwise
 */
xQStatusCode xQNodeList_free(xQNodeList* list, int freeList) {
  if (list && list->list)
    free(list->list);
  if (freeList)
    free(list);
  
  return XQ_OK;
}

/**
 * Grow the capacity of the list to accommodate a minimum number of items
 *
 * Returns a 0 (XQ_OK) on success, an error code otherwise
 */
static xQStatusCode xQNodeList_grow(xQNodeList* list, unsigned long requiredCapacity) {
  unsigned long newCapacity;
  xmlNodePtr* buff;
  
  if (list->capacity >= requiredCapacity)
    return XQ_OK;
  
  newCapacity = list->capacity < 8 ? 8 : list->capacity;
  
  while (newCapacity < requiredCapacity && newCapacity >= list->capacity)
    newCapacity << 2;
  
  if (newCapacity < requiredCapacity)
    return XQ_ARGUMENT_OUT_OF_BOUNDS;
  
  buff = (xmlNodePtr*) realloc(list->list, sizeof(xmlNodePtr) * newCapacity);
  if (!buff)
    return XQ_OUT_OF_MEMORY;
  
  list->list = buff;
  list->capacity = newCapacity;
  
  return XQ_OK;
}

/**
 * Insert an item into a list at a given index
 *
 * Returns a 0 (XQ_OK) on success, an error code otherwise
 */
xQStatusCode xQNodeList_insert(xQNodeList* list, xmlNodePtr node, unsigned long atIdx) {
  xQStatusCode result;
  
  if (atIdx > list->size)
    return XQ_ARGUMENT_OUT_OF_BOUNDS;
  
  if (list->capacity < list->size + 1)
    result = xQNodeList_grow(list, list->size + 1);
  
  if (result != XQ_OK)
    return result;
  
  if (atIdx != list->size)
    memmove(&(list->list[atIdx + 1]), &(list->list[atIdx]), sizeof(xmlNodePtr) * (list->size - atIdx));
  
  list->list[atIdx] = node;
  list->size += 1;
  
  return XQ_OK;
}

/**
 * Remove 1 or more items from the list at a given index
 *
 * Returns a 0 (XQ_OK) on success, an error code otherwise
 */
xQStatusCode xQNodeList_remove(xQNodeList* list, unsigned long fromIdx, unsigned long count) {
  unsigned long endIdx = fromIdx + count;
  
  if (fromIdx >= list->size || endIdx > list->size)
    return XQ_ARGUMENT_OUT_OF_BOUNDS;
  
  if (endIdx < list->size)
    memmove(&(list->list[fromIdx]), &(list->list[endIdx]), sizeof(xmlNodePtr) * count);
  
  list->size -= count;
  
  return XQ_OK;
}

/**
 * Assign the contents of another list to this list
 *
 * Returns a 0 (XQ_OK) on success, an error code otherwise
 */
xQStatusCode xQNodeList_assign(xQNodeList* toList, xQNodeList* fromList) {
  xQStatusCode result = XQ_OK;
  
  if (toList->capacity < fromList->size)
    result = xQNodeList_grow(toList, fromList->size);
  
  if (result == XQ_OK) {
    memmove(toList->list, fromList->list, sizeof(xmlNodePtr) * fromList->size);
    toList->size = fromList->size;
  }
  
  return result;
}
