#ifndef __LIBXQ_H_INCLUDED__
#define __LIBXQ_H_INCLUDED__
/**
 * A library implementing a jQuery-like interface for working with XML
 */

#include <libxml/parser.h>
#include <libxml/tree.h>

#ifdef __cplusplus
extern "C" {
#endif


typedef enum {
  XQ_OK = 0,
  XQ_OUT_OF_MEMORY,
  XQ_ARGUMENT_OUT_OF_BOUNDS
} xQStatusCode;

typedef struct _xQNodeList {
  xmlNodePtr* list;
  unsigned long capacity;
  unsigned long size;
} xQNodeList;

xQNodeList* xQNodeList_alloc_init(unsigned long size);
xQStatusCode xQNodeList_init(xQNodeList* list, unsigned long size);
xQStatusCode xQNodeList_free(xQNodeList* list, int freeList);
xQStatusCode xQNodeList_insert(xQNodeList* list, xmlNodePtr node, unsigned long atIdx);
xQStatusCode xQNodeList_remove(xQNodeList* list, unsigned long fromIdx, unsigned long count);
#define xQNodeList_push(list, node) (xQNodeList_insert(list, node, list->size))



typedef struct _xQ {
  xmlDocPtr document;
  xQNodeList context;
} xQ;

xQ* xQ_alloc_init();
xQ* xQ_alloc_initDoc(xmlDocPtr doc);
xQ* xQ_alloc_initFile(const char* filename);
xQ* xQ_alloc_initMemory(const char* buffer, int size);
xQStatusCode xQ_init(xQ* self);
xQStatusCode xQ_free(xQ* self, int freeXQ);
xQStatusCode xQ_find(xQ* self, const char* selector);

#ifdef __cplusplus
}
#endif

#endif // __LIBXQ_H_INCLUDED__
