/**
 * xQ implementation
 */

#include "libxq.h"

#include <stdlib.h>
#include <string.h>

// local (private) routines
static void* nsItemCopy(void* payload, xmlChar* name);
static void nsItemDestroy(void* payload, xmlChar* name);
static xQStatusCode xQ_alloc_initResult(xQ** self, xQ* other);

/**
 * Allocate and initialize a new empty xQ
 *
 * The parameter self is set to the pointer to the new instance on success
 * or 0 on error.
 *
 * Returns a 0 (XQ_OK) on success, an error code otherwise
 */
xQStatusCode xQ_alloc_init(xQ** self) {
  xQStatusCode status = XQ_OK;
  
  *self = (xQ*) malloc(sizeof(xQ));
  if (!*self)
    return XQ_OUT_OF_MEMORY;
  
  if (XQ_OK != (status = xQ_init(*self))) {
    free(*self);
    *self = 0;
  }
  
  return status;
}

/**
 * Allocate and initialize a new xQ using a an existing XML document as
 * the context.
 *
 * The parameter self is set to the pointer to the new instance on success
 * or 0 on error.
 *
 * Returns a 0 (XQ_OK) on success, an error code otherwise
 */
xQStatusCode xQ_alloc_initDoc(xQ** self, xmlDocPtr doc) {
  xQStatusCode status = XQ_OK;
  
  status = xQ_alloc_init(self);
  if (status != XQ_OK)
    return status;
  
  (*self)->document = doc;
  
  status = xQNodeList_push(&((*self)->context), (xmlNodePtr) doc);
  
  if (status != XQ_OK) {
    xQ_free(*self, 1);
    *self = 0;
  }
  
  return status;
}

/**
 * Allocate and initialize a new xQ using a an XML document from a file
 * as the context.
 *
 * The parameter self is set to the pointer to the new instance on success
 * or 0 on error.
 *
 * The parameter doc is set to the pointer to the created document on
 * success. The caller is responsible for freeing the allocated document
 * when appropriate. This is to allow refererences to the document to
 * live beyond the life of the xQ object, particularly when implementing
 * other languange bindings which may allow direct access to the node
 * list and its contents.
 *
 * Returns a 0 (XQ_OK) on success, an error code otherwise
 */
xQStatusCode xQ_alloc_initFile(xQ** self, const char* filename, xmlDocPtr* doc) {
  xQStatusCode status = XQ_OK;
  
  *doc = 0;
  
  status = xQ_alloc_init(self);
  if (status != XQ_OK)
    return status;
  
  if ( ((*self)->document = xmlParseFile(filename)) == 0 )
    status = XQ_XML_PARSER_ERROR;
    
  *doc = (*self)->document;
  
  if (status == XQ_OK)
    status = xQNodeList_push(&((*self)->context), (xmlNodePtr) *doc);
  
  if (status != XQ_OK) {
    xQ_free(*self, 1);
    *self = 0;
  }
  
  return status;
}

/**
 * Allocate and initialize a new xQ using a an XML document from a memory
 * buffer as the context.
 *
 * The parameter self is set to the pointer to the new instance on success
 * or 0 on error.
 *
 * The parameter doc is set to the pointer to the created document on
 * success. The caller is responsible for freeing the allocated document
 * when appropriate. This is to allow refererences to the document to
 * live beyond the life of the xQ object, particularly when implementing
 * other languange bindings which may allow direct access to the node
 * list and its contents.
 *
 * Returns a 0 (XQ_OK) on success, an error code otherwise
 */
xQStatusCode xQ_alloc_initMemory(xQ** self, const char* buffer, int size, xmlDocPtr* doc) {
  xQStatusCode status = XQ_OK;
  
  *doc = 0;
  
  status = xQ_alloc_init(self);
  if (status != XQ_OK)
    return status;
  
  if ( ((*self)->document = xmlParseMemory(buffer, size) ) == 0 )
    status = XQ_XML_PARSER_ERROR;
  
  *doc = (*self)->document;
  
  if (status == XQ_OK)
    status = xQNodeList_push(&((*self)->context), (xmlNodePtr) *doc);
  
  if (status != XQ_OK) {
    xQ_free(*self, 1);
    *self = 0;
  }
  
  return status;
}

/**
 * Allocate and initialize a new xQ using a list of nodes from an
 * existing XML document.
 *
 * The parameter self is set to the pointer to the new instance on success
 * or 0 on error.
 *
 * Returns a 0 (XQ_OK) on success, an error code otherwise
 */
xQStatusCode xQ_alloc_initNodeList(xQ** self, xQNodeList* list) {
  xQStatusCode status = XQ_OK;

  *self = (xQ*) malloc(sizeof(xQ));
  if (!*self)
    return XQ_OUT_OF_MEMORY;
  
  (*self)->document = list->size > 0 ? list->list[0]->doc : 0;
  (*self)->nsPrefixes = 0;
  
  status = xQNodeList_init(&((*self)->context), list->size);
  
  if (status == XQ_OK)
    status = xQNodeList_assign(&((*self)->context), list);

  if (XQ_OK != status) {
    xQ_free(*self, 1);
    *self = 0;
  }
  
  return status;
}

/**
 * Allocate and initialize a new xQ that can be used for a search result
 * based on `other`.
 *
 * Returns a 0 (XQ_OK) on success, an error code otherwise
 */
static xQStatusCode xQ_alloc_initResult(xQ** self, xQ* other) {
  xQStatusCode status = XQ_OK;

  status = xQ_alloc_init(self);

  if (status == XQ_OK)
    (*self)->document = other->document;
  
  if (status == XQ_OK && other->nsPrefixes)
    status = (((*self)->nsPrefixes = xmlHashCopy(other->nsPrefixes, nsItemCopy)) != 0) ? XQ_OK : XQ_OUT_OF_MEMORY;
  
  if (status != XQ_OK) {
    xQ_free(*self, 1);
    *self = 0;
  }
  
  return status;
}


/**
 * Initialize a newly allocated xQ
 *
 * Returns a 0 (XQ_OK) on success, an error code otherwise
 */
xQStatusCode xQ_init(xQ* self) {
  self->document = 0;
  self->nsPrefixes = 0;
  return xQNodeList_init(&(self->context), 8);
}

/**
 * Free the internal resources used by an xQ object. If freeXQ is
 * non-zero, the xQ object itself is also freed.
 *
 * Returns a 0 (XQ_OK) on success, an error code otherwise
 */
xQStatusCode xQ_free(xQ* self, int freeXQ) {
  if (self)
    xQNodeList_free(&(self->context), 0);
  if (self && self->nsPrefixes)
    xmlHashFree(self->nsPrefixes, nsItemDestroy);
  if (freeXQ)
    free(self);
  
  return XQ_OK;
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
  
  return xmlNodeGetContent(self->context.list[0]);
}

/**
 * Create a new xQ object containing the children of the current context,
 * optionally filtered by a selector. The result parameter is assigned
 * the newly allocated xQ object and the caller is responsible for
 * freeing it. On failure, the result parameter is set to null. Pass NULL
 * as the selector parameter to indicate no filter should be applied.
 *
 * Returns a 0 (XQ_OK) on success, an error code otherwise
 */
xQStatusCode xQ_children(xQ* self, const xmlChar* selector, xQ** result) {
  xQStatusCode retcode = XQ_OK;
  xQSearchExpr* expr;
  xQNodeList tmpList;
  xmlNodePtr cur, match;
  unsigned int i;
  
  expr = 0;
  *result = 0;
  tmpList.list = 0;
  
  retcode = selector ? xQSearchExpr_alloc_initFilter(&expr, selector) : XQ_OK;
  if (retcode != XQ_OK)
    return retcode;
  
  retcode = xQ_alloc_initResult(result, self);

  if (retcode == XQ_OK)
    retcode = xQNodeList_init(&tmpList, self->context.size);

  for (i = 0; retcode == XQ_OK && i < self->context.size; i++) {
    
    xmlNodePtr cur = self->context.list[i]->children;
  
    while (cur && retcode == XQ_OK) {
      
      match = cur;
      
      if (expr) {
        xQNodeList_clear(&tmpList);
    
        retcode = xQSearchExpr_eval(expr, self, cur, &tmpList);
    
        match = (retcode == XQ_OK && tmpList.size == 1 && tmpList.list[0] == cur) ? cur : 0;
      }
      
      if (match)
        xQNodeList_push(&((*result)->context), match);
    
      cur = cur->next;
    }
  }
  
  if (retcode != XQ_OK) {
    xQ_free(*result, 1);
    *result = 0;
  }
  
  xQNodeList_free(&tmpList, 0);
  xQSearchExpr_free(expr);

  return retcode;
}

// below are macros of code used in the slight variations of traversal routines

/**
 * Initialize a search expression and an xQ result
 */
#define setupSearch(self, expr, selector, result, retcode) \
  *result = 0; \
  \
  retcode = xQSearchExpr_alloc_init(&expr, selector); \
  if (retcode != XQ_OK) \
    return retcode; \
  \
  retcode = xQ_alloc_initResult(result, self);

/**
 * Initialize a filter search expression and an xQ result for filter
 * operations
 */
#define setupFilter(self, expr, selector, result, retcode) \
  *result = 0; \
  \
  retcode = xQSearchExpr_alloc_initFilter(&expr, selector); \
  if (retcode != XQ_OK) \
    return retcode; \
  \
  retcode = xQ_alloc_initResult(result, self);

/**
 * Initialize an optional filter search expression and an xQ result
 */
#define setupOptionalFilter(self, expr, selector, result, retcode) \
  expr = 0; \
  *result = 0; \
  \
  retcode = selector ? xQSearchExpr_alloc_initFilter(&expr, selector) : XQ_OK; \
  if (retcode != XQ_OK) \
    return retcode; \
  \
  retcode = xQ_alloc_initResult(result, self);

/**
 * Initialize a temporary node list
 */
#define setupTempList(self, tmpList, retcode) \
  tmpList.list = 0; \
  \
  if (retcode == XQ_OK) \
    retcode = xQNodeList_init(&tmpList, self->context.size);

/**
 * Cleanup after a search operation
 */
#define completeSearch(result, expr, retcode) \
  if (retcode != XQ_OK) { \
    xQ_free(*result, 1); \
    *result = 0; \
  } \
  \
  xQSearchExpr_free(expr);

/**
 * Cleanup after a search operation that includes a temporary node list
 */
#define completeSearchTempList(result, expr, tmpList, retcode) \
  completeSearch(result, expr, retcode) \
  xQNodeList_free(&tmpList, 0);


/**
 * Complete traversal implementation for functions that traverse a single
 * step in one direction and apply an optional filter
 */
#define stepAxisOptionallyFilter(self, selector, result, axis, retcode) \
  xQSearchExpr* expr; \
  xQNodeList tmpList; \
  xmlNodePtr match; \
  unsigned int i; \
  \
  setupOptionalFilter(self, expr, selector, result, retcode); \
  \
  setupTempList(self, tmpList, retcode); \
  \
  for (i = 0; retcode == XQ_OK && i < self->context.size; i++) { \
  \
    match = self->context.list[i]->axis; \
  \
    if (match) { \
  \
      if (expr) { \
        xQNodeList_clear(&tmpList); \
  \
        retcode = xQSearchExpr_eval(expr, self, match, &tmpList); \
  \
        match = (retcode == XQ_OK && tmpList.size == 1 && tmpList.list[0] == match) ? match : 0; \
      } \
  \
      if (match) \
        xQNodeList_push(&((*result)->context), match); \
  \
    } \
  } \
  \
  completeSearchTempList(result, expr, tmpList, retcode);

/**
 * Complete traversal implementation for functions that traverse along a
 * single axis collecting all elements and optionally applying a filter
 */
#define traverseAxisOptionallyFilter(self, selector, result, axis, retcode) \
  xQSearchExpr* expr; \
  xQNodeList tmpList; \
  xmlNodePtr cur, match; \
  unsigned int i; \
  \
  setupOptionalFilter(self, expr, selector, result, retcode); \
  \
  setupTempList(self, tmpList, retcode); \
  \
  for (i = 0; retcode == XQ_OK && i < self->context.size; i++) { \
  \
    cur = self->context.list[i]->axis && XML_ELEMENT_NODE == self->context.list[i]->axis->type ? self->context.list[i]->axis : 0; \
  \
    while (cur && retcode == XQ_OK) { \
  \
      match = cur; \
  \
      if (expr) { \
        xQNodeList_clear(&tmpList); \
  \
        retcode = xQSearchExpr_eval(expr, self, cur, &tmpList); \
  \
        match = (retcode == XQ_OK && tmpList.size == 1 && tmpList.list[0] == cur) ? cur : 0; \
      } \
  \
      if (match) \
        xQNodeList_push(&((*result)->context), match); \
  \
      cur = cur->axis && XML_ELEMENT_NODE == cur->axis->type ? cur->axis : 0 ; \
    } \
  } \
  \
  completeSearchTempList(result, expr, tmpList, retcode);

/**
 * Complete traversal implementation for functions that traverse along a
 * single axis until an element matching the supplied filter is found.
 */
#define traverseAxisUntil(self, selector, result, axis, retcode) \
  xQSearchExpr* expr; \
  xQNodeList tmpList; \
  xmlNodePtr cur; \
  unsigned int i, failed; \
  \
  setupFilter(self, expr, selector, result, retcode); \
  \
  setupTempList(self, tmpList, retcode); \
  \
  for (i = 0; retcode == XQ_OK && i < self->context.size; i++) { \
    xQNodeList_clear(&tmpList); \
  \
    cur = self->context.list[i]->axis && XML_ELEMENT_NODE == self->context.list[i]->axis->type ? self->context.list[i]->axis : 0; \
    failed = 0; \
  \
    while (cur && retcode == XQ_OK && (!failed)) { \
      retcode = xQSearchExpr_eval(expr, self, cur, &tmpList); \
  \
      if (retcode == XQ_OK && (tmpList.size != 1 || tmpList.list[0] != cur)) \
        xQNodeList_push(&((*result)->context), cur); \
      else if (retcode == XQ_OK) \
        failed = 1; \
  \
      cur = cur->axis && XML_ELEMENT_NODE == cur->axis->type ? cur->axis : 0; \
    } \
  } \
  \
  completeSearchTempList(result, expr, tmpList, retcode);


// traversal routines follow

/**
 * For each item in the current context, travel up the dom until an
 * element matching the supplied selector is found. The found elements,
 * if any, are stored in a new xQ object. The result parameter is assigned
 * the newly allocated xQ object and the caller is responsible for
 * freeing it. On failure, the result parameter is set to null.
 *
 * Returns a 0 (XQ_OK) on success, an error code otherwise
 */
xQStatusCode xQ_closest(xQ* self, const xmlChar* selector, xQ** result) {
  xQStatusCode retcode = XQ_OK;
  xQSearchExpr* expr;
  xQNodeList tmpList;
  xmlNodePtr cur, match;
  unsigned int i;
  
  setupFilter(self, expr, selector, result, retcode);
  
  setupTempList(self, tmpList, retcode);

  for (i = 0; retcode == XQ_OK && i < self->context.size; i++) {
    
    xmlNodePtr cur = self->context.list[i];
    match = 0;
  
    while (cur && retcode == XQ_OK && (!match)) {
      
      xQNodeList_clear(&tmpList);
      
      retcode = xQSearchExpr_eval(expr, self, cur, &tmpList);
    
      match = (retcode == XQ_OK && tmpList.size == 1 && tmpList.list[0] == cur) ? cur : 0;
      
      if (match)
        xQNodeList_push(&((*result)->context), match);
    
      cur = cur->parent;
    }
  }
  
  completeSearchTempList(result, expr, tmpList, retcode);

  return retcode;
}

/**
 * Search the current context for selector and return the result as a new
 * xQ object. The result parameter is assigned the newly allocated xQ
 * object and the caller is responsible for freeing it. On failure, the
 * result parameter is set to null.
 *
 * Returns a 0 (XQ_OK) on success, an error code otherwise
 */
xQStatusCode xQ_find(xQ* self, const xmlChar* selector, xQ** result) {
  xQStatusCode retcode = XQ_OK;
  xQSearchExpr* expr;
  unsigned int i;
  
  setupSearch(self, expr, selector, result, retcode);
  
  for (i = 0; retcode == XQ_OK && i < self->context.size; i++)
    retcode = xQSearchExpr_eval(expr, self, self->context.list[i], &((*result)->context));
  
  completeSearch(result, expr, retcode);

  return retcode;
}

/**
 * Filter the current context for items matching a selector and return
 * the result as a new xQ object. The result parameter is assigned the
 * newly allocated xQ object and the caller is responsible for freeing
 * it. On failure, the result parameter is set to null.
 *
 * Returns a 0 (XQ_OK) on success, an error code otherwise
 */
xQStatusCode xQ_filter(xQ* self, const xmlChar* selector, xQ** result) {
  xQStatusCode retcode = XQ_OK;
  xQSearchExpr* expr;
  xQNodeList tmpList;
  unsigned int i;
  
  setupFilter(self, expr, selector, result, retcode);
  
  setupTempList(self, tmpList, retcode);
  
  for (i = 0; retcode == XQ_OK && i < self->context.size; i++) {
    xQNodeList_clear(&tmpList);
    
    retcode = xQSearchExpr_eval(expr, self, self->context.list[i], &tmpList);
    
    if (retcode == XQ_OK && tmpList.size == 1 && tmpList.list[0] == self->context.list[i])
      xQNodeList_push(&((*result)->context), self->context.list[i]);
  }
  
  completeSearchTempList(result, expr, tmpList, retcode);

  return retcode;
}

/**
 * Create a new xQ object containing the next sibling of each node in the
 * current context, optionally filtered by a selector. The result
 * parameter is assigned the newly allocated xQ object and the caller is
 * responsible for freeing it. On failure, the result parameter is set to
 * null. Pass NULL as the selector parameter to indicate no filter should
 * be applied.
 *
 * Returns a 0 (XQ_OK) on success, an error code otherwise
 */
xQStatusCode xQ_next(xQ* self, const xmlChar* selector, xQ** result) {
  xQStatusCode retcode = XQ_OK;

  stepAxisOptionallyFilter(self, selector, result, next, retcode);

  return retcode;
}

/**
 * Create a new xQ object containing all the next siblings of each node
 * in the current context, optionally filtered by a selector. The result
 * parameter is assigned the newly allocated xQ object and the caller is
 * responsible for freeing it. On failure, the result parameter is set to
 * null. Pass NULL as the selector parameter to indicate no filter should
 * be applied.
 *
 * Returns a 0 (XQ_OK) on success, an error code otherwise
 */
xQStatusCode xQ_nextAll(xQ* self, const xmlChar* selector, xQ** result) {
  xQStatusCode retcode = XQ_OK;

  traverseAxisOptionallyFilter(self, selector, result, next, retcode);

  return retcode;
}

/**
 * Create a new xQ object containing all the next siblings of each node
 * in the current context, up to but not including the first sibling
 * matched by a selector. The result parameter is assigned the newly
 * allocated xQ object and the caller is responsible for freeing it. On
 * failure, the result parameter is set to null.
 *
 * Returns a 0 (XQ_OK) on success, an error code otherwise
 */
xQStatusCode xQ_nextUntil(xQ* self, const xmlChar* selector, xQ** result) {
  xQStatusCode retcode = XQ_OK;

  traverseAxisUntil(self, selector, result, next, retcode);

  return retcode;
}

/**
 * Create a new xQ object containing all the nodes in the current context
 * which do not match the provided selector. The result parameter is
 * assigned the newly allocated xQ object and the caller is responsible
 * for freeing it. On failure, the result parameter is set to null.
 *
 * Returns a 0 (XQ_OK) on success, an error code otherwise
 */
xQStatusCode xQ_not(xQ* self, const xmlChar* selector, xQ** result) {
  xQStatusCode retcode = XQ_OK;
  xQSearchExpr* expr;
  xQNodeList tmpList;
  xmlNodePtr cur;
  unsigned int i;
  
  setupFilter(self, expr, selector, result, retcode);
  
  setupTempList(self, tmpList, retcode);
  
  for (i = 0; retcode == XQ_OK && i < self->context.size; i++) {
    xQNodeList_clear(&tmpList);
    
    cur = self->context.list[i];
    
    retcode = xQSearchExpr_eval(expr, self, cur, &tmpList);
    
    if (retcode == XQ_OK && (tmpList.size != 1 || tmpList.list[0] != cur))
      xQNodeList_push(&((*result)->context), cur);
  }
  
  completeSearchTempList(result, expr, tmpList, retcode);

  return retcode;
}

/**
 * Create a new xQ object containing the parent node of each node in
 * the current context, optionally filtered by a selector. The result
 * parameter is assigned the newly allocated xQ object and the caller is
 * responsible for freeing it. On failure, the result parameter is set to
 * null. Pass NULL as the selector parameter to indicate no filter should
 * be applied.
 *
 * Returns a 0 (XQ_OK) on success, an error code otherwise
 */
xQStatusCode xQ_parent(xQ* self, const xmlChar* selector, xQ** result) {
  xQStatusCode retcode = XQ_OK;

  stepAxisOptionallyFilter(self, selector, result, parent, retcode);

  return retcode;
}

/**
 * Create a new xQ object containing all the ancestors of each
 * node in the current context, optionally filtered by a selector. The
 * result parameter is assigned the newly allocated xQ object and the
 * caller is responsible for freeing it. On failure, the result parameter
 * is set to null. Pass NULL as the selector parameter to indicate no
 * filter should be applied.
 *
 * Returns a 0 (XQ_OK) on success, an error code otherwise
 */
xQStatusCode xQ_parents(xQ* self, const xmlChar* selector, xQ** result) {
  xQStatusCode retcode = XQ_OK;

  traverseAxisOptionallyFilter(self, selector, result, parent, retcode);

  return retcode;
}

/**
 * Create a new xQ object containing all the ancestors of each
 * node in the current context, up to but not including the first ancestor
 * matched by a selector. The result parameter is assigned the newly
 * allocated xQ object and the caller is responsible for freeing it. On
 * failure, the result parameter is set to null.
 *
 * Returns a 0 (XQ_OK) on success, an error code otherwise
 */
xQStatusCode xQ_parentsUntil(xQ* self, const xmlChar* selector, xQ** result) {
  xQStatusCode retcode = XQ_OK;

  traverseAxisUntil(self, selector, result, parent, retcode);

  return retcode;
}

/**
 * Create a new xQ object containing the previous sibling of each node in
 * the current context, optionally filtered by a selector. The result
 * parameter is assigned the newly allocated xQ object and the caller is
 * responsible for freeing it. On failure, the result parameter is set to
 * null. Pass NULL as the selector parameter to indicate no filter should
 * be applied.
 *
 * Returns a 0 (XQ_OK) on success, an error code otherwise
 */
xQStatusCode xQ_prev(xQ* self, const xmlChar* selector, xQ** result) {
  xQStatusCode retcode = XQ_OK;

  stepAxisOptionallyFilter(self, selector, result, prev, retcode);

  return retcode;
}

/**
 * Create a new xQ object containing all the previous siblings of each
 * node in the current context, optionally filtered by a selector. The
 * result parameter is assigned the newly allocated xQ object and the
 * caller is responsible for freeing it. On failure, the result parameter
 * is set to null. Pass NULL as the selector parameter to indicate no
 * filter should be applied.
 *
 * Returns a 0 (XQ_OK) on success, an error code otherwise
 */
xQStatusCode xQ_prevAll(xQ* self, const xmlChar* selector, xQ** result) {
  xQStatusCode retcode = XQ_OK;

  traverseAxisOptionallyFilter(self, selector, result, prev, retcode);

  return retcode;
}

/**
 * Create a new xQ object containing all the previous siblings of each
 * node in the current context, up to but not including the first sibling
 * matched by a selector. The result parameter is assigned the newly
 * allocated xQ object and the caller is responsible for freeing it. On
 * failure, the result parameter is set to null.
 *
 * Returns a 0 (XQ_OK) on success, an error code otherwise
 */
xQStatusCode xQ_prevUntil(xQ* self, const xmlChar* selector, xQ** result) {
  xQStatusCode retcode = XQ_OK;

  traverseAxisUntil(self, selector, result, prev, retcode);

  return retcode;
}

/**
 * Clear the context of the the xQ object.
 *
 * Returns a 0 (XQ_OK) on success, an error code otherwise
 */
xQStatusCode xQ_clear(xQ* self) {
  return xQNodeList_clear(&(self->context));
}

/**
 * Return the value of the named attribute from the first item in an xQ's
 * collection.
 *
 * Returns a pointer to the string copy on success, or 0 on failure or in
 * the event of no such attribute. The caller is responsible for freeing
 * any returned string by calling xmlFree().
 */
xmlChar* xQ_getAttr(xQ* self, const char* name) {
  if ( (!self->context.size) || (!self->context.list[0]) )
    return 0;
  
  return xmlGetProp(self->context.list[0], (xmlChar*)name);
}

/**
 * Return the XML of the first item in an xQ's collection.
 *
 * Returns a pointer to the string copy on success, or 0 on failure. The
 * caller is responsible for freeing any returned string by calling
 * xmlFree().
 */
xmlChar* xQ_getXml(xQ* self) {
  xmlNodePtr node = 0;
  xmlBufferPtr buff = 0;
  xmlChar* str = 0;
  int strSize = 0;
  
  if ( (!self->context.size) || (!self->context.list[0]) ||
       (!self->context.list[0]->children && XML_ELEMENT_NODE != self->context.list[0]->type) )
    return xmlCharStrdup("");
  
  node = self->context.list[0];
  
  // dump an entire document
  if (XML_DOCUMENT_NODE == node->type) {

    xmlDocDumpMemory((xmlDoc*)node, &str, &strSize);
    if (!strSize)
      return 0;

  // dump a node from within the document
  } else {
    
    buff = xmlBufferCreate();
    if (!buff)
      return 0;
      
    if (!xmlNodeDump(buff, node->doc, node, 0, 0)) {
      xmlBufferFree(buff);
      return 0;
    }
    
    str = (xmlChar*) xmlBufferContent(buff);
    if (str) str = xmlStrdup(str);
    xmlBufferFree(buff);

  }
  
  return str;
}

/**
 * Create a new xQ object containing only the first item from the current
 * context (if any). The result parameter is assigned the newly allocated
 * xQ object and the caller is responsible for freeing it. On failure,
 * the result parameter is set to null.
 *
 * Returns a 0 (XQ_OK) on success, an error code otherwise
 */
xQStatusCode xQ_first(xQ* self, xQ** result) {
  xQStatusCode retcode = XQ_OK;
  
  *result = 0;
  
  retcode = xQ_alloc_initResult(result, self);

  if (retcode == XQ_OK && self->context.size)
    xQNodeList_push(&((*result)->context), self->context.list[0]);
  
  if (retcode != XQ_OK && (*result)) {
    xQ_free(*result, 1);
    *result = 0;
  }
  
  return retcode;
}

/**
 * Create a new xQ object containing only the last item from the current
 * context (if any). The result parameter is assigned the newly allocated
 * xQ object and the caller is responsible for freeing it. On failure,
 * the result parameter is set to null.
 *
 * Returns a 0 (XQ_OK) on success, an error code otherwise
 */
xQStatusCode xQ_last(xQ* self, xQ** result) {
  xQStatusCode retcode = XQ_OK;
  
  *result = 0;
  
  retcode = xQ_alloc_initResult(result, self);

  if (retcode == XQ_OK && self->context.size)
    xQNodeList_push(&((*result)->context), self->context.list[(self->context.size)-1]);
  
  if (retcode != XQ_OK && (*result)) {
    xQ_free(*result, 1);
    *result = 0;
  }
  
  return retcode;
}

/**
 * Associates a new namespace prefix with the given URI. Any existing
 * association for the same prefix is overwritten.
 *
 * Returns 0 (XQ_OK) on success, an error code otherwise
 */
xQStatusCode xQ_addNamespace(xQ* self, const xmlChar* prefix, const xmlChar* uri) {
  xmlChar* uriCopy = 0;
  
  if (!self->nsPrefixes)
    self->nsPrefixes = xmlHashCreate(8);
  
  if (!self->nsPrefixes)
    return XQ_OUT_OF_MEMORY;
  
  uriCopy = xmlStrdup(uri);
  if (!uriCopy)
    return XQ_OUT_OF_MEMORY;
  
  if (xmlHashUpdateEntry(self->nsPrefixes, prefix, (void*)uriCopy, nsItemDestroy) != 0) {
    xmlFree(uriCopy);
    return XQ_OUT_OF_MEMORY;
  }
  
  return XQ_OK;
}

/**
 * Looks up the namespace URI associated with the given prefix.
 *
 * Returns the namespace URI string if found, otherwise NULL.
 */
const xmlChar* xQ_namespaceForPrefix(xQ* self, const xmlChar* prefix) {
  if (!self->nsPrefixes)
    return 0;
  
  return (const xmlChar*)xmlHashLookup(self->nsPrefixes, prefix);
}

/**
 * Performs an item copy operation for the ns prefix table
 */
static void* nsItemCopy(void* payload, xmlChar* name) {
  return (void*)xmlStrdup((xmlChar*)payload);
}

/**
 * Performs an item destroy operation for the ns prefix table
 */
static void nsItemDestroy(void* payload, xmlChar* name) {
  xmlFree(payload);
}
