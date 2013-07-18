/**
 * Search expression routines
 */

#include "libxq.h"

// local (private) routines
static xQSearchExpr* xQSearchExpr_alloc_init_copy();
static xQSearchExpr* xQSearchExpr_alloc_init_searchDescendants(xmlChar* name);
static xQSearchExpr* xQSearchExpr_parseExpr(const xmlChar** strPtr);

static xmlChar* nextToken(const xmlChar** strPtr);
static int xmlstrpos(const xmlChar* haystack, xmlChar needle);


static const xmlChar spaceChars[] = {
  ' ', '\t', '\r', '\n', '\0'
};

/**
 * Return the position of needle in haystack or -1
 */
static int xmlstrpos(const xmlChar* haystack, xmlChar needle) {
  int i;
  
  for (i = 0; haystack[i]; i++)
    if (haystack[i] == needle)
      return i;
  
  return -1;
}

/**
 * Return the next token from the string
 */
static xmlChar* nextToken(const xmlChar** strPtr) {
  const xmlChar* start = *strPtr;
  
  // consume any leading space
  while (*start && xmlstrpos(spaceChars, *start) != -1)
    ++start;
  
  // advance the pointer to a space or the end of the string
  *strPtr = start;
  while (**strPtr && xmlstrpos(spaceChars, **strPtr) == -1)
    ++(*strPtr);
  
  if (!*start)
    return 0;
  
  return xmlStrndup(start, (*strPtr) - start);
}

/**
 * Allocate and initialize a new xQSearchExpr object from an expression
 * string.
 *
 * Returns a pointer to the new instance or 0 on error
 */
xQSearchExpr* xQSearchExpr_alloc_init(const xmlChar* expr) {
  xQSearchExpr* self;
  const xmlChar* ptr = expr;
  
  self = xQSearchExpr_parseExpr(&ptr);
  
  if (!self)
    self = xQSearchExpr_alloc_init_copy();
  
  return self;
}

/**
 * Parse an expression from the string
 */
static xQSearchExpr* xQSearchExpr_parseExpr(const xmlChar** strPtr) {
  xmlChar* tok;
  xQSearchExpr* expr = 0;
  
  tok = nextToken(strPtr);
  if (!tok)
    return 0;
  
  expr = xQSearchExpr_alloc_init_searchDescendants(tok);
  if (expr)
    expr->next = xQSearchExpr_parseExpr(strPtr);
  
  return expr;
}

/**
 * Allocate and initialize a new xQSearchExpr object that copies the
 * input node to the output.
 *
 * Returns a pointer to the new instance or 0 on error
 */
static xQSearchExpr* xQSearchExpr_alloc_init_copy() {
  xQSearchExpr* self;
  
  self = (xQSearchExpr*) malloc(sizeof(xQSearchExpr));
  if (!self)
    return self;
  
  self->argc = 0;
  self->argv = 0;
  self->operation = _xQ_addToOutput;
  self->next = 0;
  
  return self;
}

/**
 * Allocate and initialize a new xQSearchExpr object that searches the
 * input node for descendants with a given name.
 *
 * Returns a pointer to the new instance or 0 on error
 */
static xQSearchExpr* xQSearchExpr_alloc_init_searchDescendants(xmlChar* name) {
  xQSearchExpr* self;
  
  self = (xQSearchExpr*) malloc(sizeof(xQSearchExpr));
  if (!self) {
    xmlFree(name);
    return self;
  }
  
  self->argv = (xmlChar**) malloc(sizeof(xmlChar*));
  if (self->argv) {
    self->argc = 1;
    self->argv[0] = name;
    self->operation = _xQ_findDescendantsByName;
    self->next = 0;
  } else {
    xmlFree(name);
    free(self);
    return 0;
  }
  
  return self;
}

/**
 * Release an xQSearchExpr object and all resources allocated by it
 *
 * Returns a 0 (XQ_OK) on success, an error code otherwise
 */
xQStatusCode xQSearchExpr_free(xQSearchExpr* self) {
  unsigned int i;
  
  if (!self)
    return XQ_OK;

  if (self->argv)
    for (i = 0; i < self->argc; i++)
      xmlFree(self->argv[i]);
  
  if (self->next)
    return xQSearchExpr_free(self->next);
  
  return XQ_OK;
}

/**
 * Evaluate a search expression and populate a node list with the results
 *
 * Returns a 0 (XQ_OK) on success, an error code otherwise
 */
xQStatusCode xQSearchExpr_eval(xQSearchExpr* self, xQ* context, xmlNodePtr node, xQNodeList* outList) {
  xQNodeList tmpList;
  xQStatusCode result;
  unsigned int i;
  
  if (!self->next)
    return self->operation(context, self->argv, node, outList);
    
  xQNodeList_init(&tmpList, 8);
  result = self->operation(context, self->argv, node, &tmpList);
  
  for (i = 0; result == XQ_OK && i < tmpList.size; i++)
    result = xQSearchExpr_eval(self->next, context, tmpList.list[i], outList);
  
  xQNodeList_free(&tmpList, 0);
  return result;
}
