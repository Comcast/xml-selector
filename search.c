/**
 * Search expression routines
 */

#include "libxq.h"

// local (private) routines
static xQSearchExpr* xQSearchExpr_alloc_init_copy();
static xQSearchExpr* xQSearchExpr_alloc_init_searchDescendants(xmlChar* name);
static xQSearchExpr* xQSearchExpr_alloc_init_filterAttrEquals(xmlChar* name, xmlChar* value);
static xQSearchExpr* xQSearchExpr_parseExpr(const xmlChar** strPtr);
static xQSearchExpr* xQSearchExpr_parseAttrFilter(const xmlChar** strPtr);

static xmlChar* nextToken(const xmlChar** strPtr);
static int xmlstrpos(const xmlChar* haystack, xmlChar needle);
static int isStrToken(const xmlChar* test);

#define xqIsSpace(c) (xmlstrpos(spaceChars, c) != -1)
#define xqIsNotSpace(c) (xmlstrpos(spaceChars, c) == -1)
#define xqIsToken(c) (xmlstrpos(tokenChars, c) != -1)
#define xqIsNotToken(c) (xmlstrpos(tokenChars, c) == -1)


static const xmlChar spaceChars[] = {
  ' ', '\t', '\r', '\n', '\0'
};

static const xmlChar tokenChars[] = {
  '"', '=', '[', ']', '\0'
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
 * Return true if the entire contents of string is equal to a single token
 */
static int isStrToken(const xmlChar* test) {
  int len = 0;
  
  if (!test)
    return 0;
  
  len = xmlStrlen(test);
  if (len != 1)
    return 0;
  
  return (xmlstrpos(tokenChars, test[0]) != -1);
}

/**
 * Return the next token from the string
 */
static xmlChar* nextToken(const xmlChar** strPtr) {
  const xmlChar* start = *strPtr;
  
  // consume any leading space
  while (*start && xqIsSpace(*start))
    ++start;
  
  // advance the pointer to a space or the end of the string
  *strPtr = start;
  
  // for a string delimiter, return the full string, minus the quotes
  if (*start == '"') {

    ++(*strPtr);
    while (**strPtr && **strPtr != '"')
      ++(*strPtr);

    if (**strPtr == '"') {
      ++(*strPtr);
        return xmlStrndup(start+1, (*strPtr) - start - 2);
    } else {
      // TODO: can't indicate unterminated string
      if (*(start+1))
        return xmlStrndup(start+1, (*strPtr) - start - 1);
      else
        return 0;
    }
      
  // for any other token, return the token
  } else if (xqIsToken(*start)) {
      ++(*strPtr);

  // in all other cases, return the string up to a space or token
  } else {
    while (**strPtr && xqIsNotSpace(**strPtr) && xqIsNotToken(**strPtr))
      ++(*strPtr);
  }
  
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
  
  if (isStrToken(tok)) {
    // filter
    if ('[' == tok[0]) {
      expr = xQSearchExpr_parseAttrFilter(strPtr);

    // unexpected token
    } else {
      // TODO: need a way to indicate syntax errors
      // fall through for now (will return 0)
    }
    xmlFree(tok);

  // expression
  } else {

    expr = xQSearchExpr_alloc_init_searchDescendants(tok);
    if (expr)
      expr->next = xQSearchExpr_parseExpr(strPtr);

  }
  
  return expr;
}

/**
 * Parse an attribute filter from the string
 */
static xQSearchExpr* xQSearchExpr_parseAttrFilter(const xmlChar** strPtr) {
  xmlChar* name;
  xmlChar* op;
  xmlChar* value;
  xmlChar* end;
  xQSearchExpr* expr = 0;
  
  // syntax: NAME '=' VALUE ']'

  name = nextToken(strPtr);
  op = nextToken(strPtr);
  value = nextToken(strPtr);
  end = nextToken(strPtr);

  // validate
  if ( (!name) || (!op) || (!value) || (!end) || (isStrToken(name)) || (xmlStrcmp(op, "=") != 0) || (xmlStrcmp(end, "]") != 0) ) {
      // TODO: need a way to indicate syntax errors
    if (name)  xmlFree(name);
    if (op)    xmlFree(op);
    if (value) xmlFree(value);
    if (end)   xmlFree(end);
    return 0;
  }
  
  // assemble it
  expr = xQSearchExpr_alloc_init_filterAttrEquals(name, value);
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
 * Allocate and initialize a new xQSearchExpr object that copies the
 * input node only if it has a named attribute with a given value.
 *
 * Returns a pointer to the new instance or 0 on error
 */
static xQSearchExpr* xQSearchExpr_alloc_init_filterAttrEquals(xmlChar* name, xmlChar* value) {
  xQSearchExpr* self;
  
  self = (xQSearchExpr*) malloc(sizeof(xQSearchExpr));
  if (!self) {
    xmlFree(name);
    xmlFree(value);
    return self;
  }
  
  self->argv = (xmlChar**) malloc(sizeof(xmlChar*) * 2);
  if (self->argv) {
    self->argc = 2;
    self->argv[0] = name;
    self->argv[1] = value;
    self->operation = _xQ_filterAttributeEquals;
    self->next = 0;
  } else {
    xmlFree(name);
    xmlFree(value);
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
