/**
 * xQ implementation
 */

#include "libxq.h"

#include <stdlib.h>
#include <string.h>

// local (private) routines
static xmlChar* xQ_textOfDescendants(xmlDocPtr doc, xmlNodePtr node, int inLine);


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
    ok = (xQNodeList_push(&(self->context), node) == XQ_OK);
  
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
    ok = (xQNodeList_push(&(self->context), node) == XQ_OK);
  
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
    ok = (xQNodeList_push(&(self->context), node) == XQ_OK);
  
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
  
  return xQ_textOfDescendants(self->document, self->context.list[0]->children, 1);
}

/**
 * Like xmlNodeListGetString, but captures text of all descendants as
 * well, stripping out element nodes.
 *
 * Due to the reliance on xmlNodeListGetString, this doesn't work exactly
 * as I would like. Calling this function on a fragment like
 * "<p>The <i>quick</i> brown...</p>" will return "The  brown..." instead
 * of "The quick brown...", but it's sufficient for the most common use
 * case at the moment.
 *
 * Returns a pointer to the string copy on success, or 0 on failure. The
 * caller is responsible for freeing any returned string by calling
 * xmlFree().
 */
static xmlChar* xQ_textOfDescendants(xmlDocPtr doc, xmlNodePtr node, int inLine) {
  xmlChar* ret = 0;
  xmlChar* tmp = 0;
  
  if (!node)
    return ret;
  
  // types handled by xmlNodeListGetString
  if (node->type == XML_TEXT_NODE || node->type == XML_CDATA_SECTION_NODE || node->type == XML_ENTITY_REF_NODE) {

    tmp = xmlNodeListGetString(doc, node, inLine);
    ret = xmlStrcat(ret, tmp);
    if (tmp) xmlFree(tmp);

  // types not handled by xmlNodeListGetString; display their children if the have any
  } else if (node->children) {

    tmp = xQ_textOfDescendants(doc, node->children, inLine);
    ret = xmlStrcat(ret, tmp);
    if (tmp) xmlFree(tmp);

  }
  
  return ret;
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
  
  *result = 0;
  
  expr = xQSearchExpr_alloc_init(selector);
  if (!expr)
    return XQ_OUT_OF_MEMORY;
  
  if (self->document)
    *result = xQ_alloc_initDoc(self->document);
  else
    *result = xQ_alloc_init();

  if (!*result)
    retcode = XQ_OUT_OF_MEMORY;
  else
    xQ_clear(*result);
  
  for (i = 0; retcode == XQ_OK && i < self->context.size; i++)
    retcode = xQSearchExpr_eval(expr, self, self->context.list[i], &((*result)->context));
  
  if (retcode != XQ_OK) {
    *result = 0;
    xQ_free(*result, 1);
  }
  
  xQSearchExpr_free(expr);

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
