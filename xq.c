/**
 * xQ implementation
 */

#include "libxq.h"

#include <stdlib.h>
#include <string.h>


/**
 * Allocate and initialize a new empty xQ
 *
 * Returns a pointer to the new instance or 0 on error
 */
xQ* xQ_alloc_init() {
  xQ* self;
  
  self = (xQ*) malloc(sizeof(xQ));
  if (!self)
    return 0;
  
  if (XQ_OK != xQ_init(self)) {
    free(self);
    self = 0;
  }
  
  return self;
}

/**
 * Allocate and initialize a new xQ using a an existing XML document as
 * the context.
 *
 * This creates a deep copy of the document which is released when the xQ
 * object is freed.
 *
 * Returns a pointer to the new instance or 0 on error
 */
xQ* xQ_alloc_initDoc(xmlDocPtr doc) {
  xQ* self;
  xmlNodePtr node;
  int ok;
  
  self = xQ_alloc_init();
  if (!self)
    return 0;
  
  ok = ( self->document = xmlCopyDoc(doc, 1) ) != 0;
  
  if (ok)
    node = (xmlNodePtr) self->document;
  
  if (ok)
    ok = (xmlNodeList_push(&(self->context), node) == XQ_OK);
  
  if (!ok) {
    xQ_free(self, 1);
    self = 0;
  }
  
  return self;
}

/**
 * Allocate and initialize a new xQ using a an XML document from a file
 * as the context.
 *
 * Returns a pointer to the new instance or 0 on error
 */
xQ* xQ_alloc_initFile(const char* filename) {
  xQ* self;
  xmlNodePtr node;
  int ok;
  
  self = xQ_alloc_init();
  if (!self)
    return 0;
  
  ok = ( self->document = xmlParseFile(filename) ) != 0;
  
  if (ok)
    node = (xmlNodePtr) self->document;
  
  if (ok)
    ok = (xmlNodeList_push(&(self->context), node) == XQ_OK);
  
  if (!ok) {
    xQ_free(self, 1);
    self = 0;
  }
  
  return self;
}

/**
 * Allocate and initialize a new xQ using a an XML document from a memory
 * buffer as the context.
 *
 * Returns a pointer to the new instance or 0 on error
 */
xQ* xQ_alloc_initMemory(const char* buffer, int size) {
  xQ* self;
  xmlNodePtr node;
  int ok;
  
  self = xQ_alloc_init();
  if (!self)
    return 0;
  
  ok = ( self->document = xmlParseMemory(buffer, size) ) != 0;
  
  if (ok)
    node = (xmlNodePtr) self->document;
  
  if (ok)
    ok = (xmlNodeList_push(&(self->context), node) == XQ_OK);
  
  if (!ok) {
    xQ_free(self, 1);
    self = 0;
  }
  
  return self;
}


/**
 * Initialize a newly allocated xQ
 *
 * Returns a 0 (XQ_OK) on success, an error code otherwise
 */
xQStatusCode xQ_init(xQ* self) {
  self->document = 0;
  return xQNodeList_init(&(self->context), 8);
}

/**
 * Free the internal resources used by an xQ object. If freeXQ is
 * non-zero, the xQ object itself is also freed.
 *
 * Returns a 0 (XQ_OK) on success, an error code otherwise
 */
xQStatusCode xQ_free(xQ* self, int freeXQ) {
  if (self && self->document)
    xmlFreeDoc(self->document);
  if (self)
    xQNodeList_free(&(self->context), 0);
  if (freeXQ)
    free(self);
}

/**
 * Return the size of the collection currently contained in the xQ object
 */
unsigned long xQ_length(xQ* self) {
  return self->context.size;
}

/**
 * Return the text of the first item in an xQ's collection.
 *
 * Returns a pointer to the string copy on success, or 0 on failure. The
 * caller is responsible for freeing any returned string by calling
 * xmlFree().
 */
xmlChar* xQ_getText(xQ* self) {
  if ( (!self->context.size) || (!self->context.list[0]) || (!self->context.list[0]->children) )
    return xmlCharStrdup("");
  
  return xmlNodeListGetString(self->document, self->context.list[0]->children, 1);
}

/**
 * Search the current context for selector and set the context to the
 * result.
 *
 * Returns a 0 (XQ_OK) on success, an error code otherwise
 */
xQStatusCode xQ_find(xQ* self, const xmlChar* selector) {
  xQStatusCode result = XQ_OK;
  xQSearchExpr* expr;
  xQNodeList tmpList;
  unsigned int i;
  
  if (!self->context.size)
    return result;
  
  expr = xQSearchExpr_alloc_init(selector);
  if (!expr)
    return XQ_OUT_OF_MEMORY;
  
  xQNodeList_init(&tmpList, 8);
  
  for (i = 0; result == XQ_OK && i < self->context.size; i++)
    result = xQSearchExpr_eval(expr, self, self->context.list[i], &tmpList);
  
  if (result == XQ_OK)
    result = xQNodeList_assign(&(self->context), &tmpList);
  
  xQNodeList_free(&tmpList, 0);

  return result;
}
