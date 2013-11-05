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
