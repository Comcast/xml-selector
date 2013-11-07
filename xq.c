/**
 * xQ implementation
 */

#include "libxq.h"

#include <stdlib.h>
#include <string.h>

// local (private) routines

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
  if (self)
    xQNodeList_free(&(self->context), 0);
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
  
  retcode = xQSearchExpr_alloc_init(&expr, selector);
  if (retcode != XQ_OK)
    return retcode;
  
  retcode = xQ_alloc_init(result);
  if (retcode == XQ_OK)
    (*result)->document = self->document;

  if (!*result)
    retcode = XQ_OUT_OF_MEMORY;
  else
    xQ_clear(*result);
  
  for (i = 0; retcode == XQ_OK && i < self->context.size; i++)
    retcode = xQSearchExpr_eval(expr, self, self->context.list[i], &((*result)->context));
  
  if (retcode != XQ_OK) {
    xQ_free(*result, 1);
    *result = 0;
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
  
  if ( (!self->context.size) || (!self->context.list[0]) || (!self->context.list[0]->children) )
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
