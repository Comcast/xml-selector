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
    ok = ( node = xmlDocGetRootElement(self->document) ) != 0;
  
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
    ok = ( node = xmlDocGetRootElement(self->document) ) != 0;
  
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
    ok = ( node = xmlDocGetRootElement(self->document) ) != 0;
  
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

//TODO
//xQStatusCode xQ_find(xQ* self, const char* selector);
