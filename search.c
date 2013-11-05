/**
 * Search expression routines
 */

#include "libxq.h"

// local (private) routines and data types
static xQStatusCode xQSearchExpr_alloc_init_copy(xQSearchExpr** self);
static xQStatusCode xQSearchExpr_alloc_init_searchDescendants(xQSearchExpr** self, xmlChar* name);
static xQStatusCode xQSearchExpr_alloc_init_searchImmediate(xQSearchExpr** self, xmlChar* name);
static xQStatusCode xQSearchExpr_alloc_init_filterAttrEquals(xQSearchExpr** self, xmlChar* name, xmlChar* value);
static xQStatusCode xQSearchExpr_parseSelector(xQSearchExpr** expr, const xmlChar** strPtr);
static xQStatusCode xQSearchExpr_parseAttrib(xQSearchExpr** expr, const xmlChar** strPtr);

typedef enum {
  XQ_TT_TOKEN,
  XQ_TT_IDENT,
  XQ_TT_STRING,
  XQ_TT_NONE
} xQTokenType;

typedef struct _xQToken {
  xQTokenType type;
  xmlChar* content;
} xQToken;

static xQStatusCode nextToken(const xmlChar** strPtr, xQToken* tokenOut);
static int xmlstrpos(const xmlChar* haystack, xmlChar needle);

/*
 * Selector grammar:
 *
 * selector        <= [ simple_selector | combinator S* simple_selector ] [ S* selector ]*
 * combinator      <= '>'
 * simple_selector <= element_name [ attrib ]*
 * element_name    <= IDENT
 * attrib          <= '[' S* IDENT S* '=' S* [ string | IDENT ] S* ']'
 * string          <= '"' [ NQ | "'" | escape ]* '"' | "'" [ NQ | '"' | escape ]* "'"
 * escape          <= '\' [ '\' | '"' | '"' ]
 * S               <= ' ' | '\t' | '\r' | '\n'
 * IDENT           <= NS+
 *
 * Where NS is any non-token terminal not part of S
 * and NQ is any terminal other than '\', '"', or "'"
 */

// table for tokenizing
typedef xmlChar xQCharacterClass;
#define XQ_TYPE_NS 1
#define XQ_TYPE_SPACE 2
#define XQ_TYPE_NQ 4
#define XQ_TYPE_TOKEN 8

static const xQCharacterClass characterClassTable[] = {
  0, // NULL
  XQ_TYPE_NS | XQ_TYPE_NQ,
  XQ_TYPE_NS | XQ_TYPE_NQ,
  XQ_TYPE_NS | XQ_TYPE_NQ,
  XQ_TYPE_NS | XQ_TYPE_NQ,
  XQ_TYPE_NS | XQ_TYPE_NQ,
  XQ_TYPE_NS | XQ_TYPE_NQ,
  XQ_TYPE_NS | XQ_TYPE_NQ,
  XQ_TYPE_NS | XQ_TYPE_NQ,
  XQ_TYPE_SPACE | XQ_TYPE_NQ, // tab
  XQ_TYPE_SPACE | XQ_TYPE_NQ, // nl
  XQ_TYPE_NS | XQ_TYPE_NQ,
  XQ_TYPE_NS | XQ_TYPE_NQ,
  XQ_TYPE_SPACE | XQ_TYPE_NQ, // cr
  XQ_TYPE_NS | XQ_TYPE_NQ,
  XQ_TYPE_NS | XQ_TYPE_NQ,
  XQ_TYPE_NS | XQ_TYPE_NQ,
  XQ_TYPE_NS | XQ_TYPE_NQ,
  XQ_TYPE_NS | XQ_TYPE_NQ,
  XQ_TYPE_NS | XQ_TYPE_NQ,
  XQ_TYPE_NS | XQ_TYPE_NQ,
  XQ_TYPE_NS | XQ_TYPE_NQ,
  XQ_TYPE_NS | XQ_TYPE_NQ,
  XQ_TYPE_NS | XQ_TYPE_NQ,
  XQ_TYPE_NS | XQ_TYPE_NQ,
  XQ_TYPE_NS | XQ_TYPE_NQ,
  XQ_TYPE_NS | XQ_TYPE_NQ,
  XQ_TYPE_NS | XQ_TYPE_NQ,
  XQ_TYPE_NS | XQ_TYPE_NQ,
  XQ_TYPE_NS | XQ_TYPE_NQ,
  XQ_TYPE_NS | XQ_TYPE_NQ,
  XQ_TYPE_NS | XQ_TYPE_NQ,
  XQ_TYPE_SPACE | XQ_TYPE_NQ, // space
  XQ_TYPE_NS | XQ_TYPE_NQ,
  XQ_TYPE_TOKEN, // "
  XQ_TYPE_NS | XQ_TYPE_NQ,
  XQ_TYPE_NS | XQ_TYPE_NQ,
  XQ_TYPE_NS | XQ_TYPE_NQ,
  XQ_TYPE_NS | XQ_TYPE_NQ,
  XQ_TYPE_TOKEN, // '
  XQ_TYPE_NS | XQ_TYPE_NQ,
  XQ_TYPE_NS | XQ_TYPE_NQ,
  XQ_TYPE_NS | XQ_TYPE_NQ,
  XQ_TYPE_NS | XQ_TYPE_NQ,
  XQ_TYPE_NS | XQ_TYPE_NQ,
  XQ_TYPE_NS | XQ_TYPE_NQ,
  XQ_TYPE_NS | XQ_TYPE_NQ,
  XQ_TYPE_NS | XQ_TYPE_NQ,
  XQ_TYPE_NS | XQ_TYPE_NQ,
  XQ_TYPE_NS | XQ_TYPE_NQ,
  XQ_TYPE_NS | XQ_TYPE_NQ,
  XQ_TYPE_NS | XQ_TYPE_NQ,
  XQ_TYPE_NS | XQ_TYPE_NQ,
  XQ_TYPE_NS | XQ_TYPE_NQ,
  XQ_TYPE_NS | XQ_TYPE_NQ,
  XQ_TYPE_NS | XQ_TYPE_NQ,
  XQ_TYPE_NS | XQ_TYPE_NQ,
  XQ_TYPE_NS | XQ_TYPE_NQ,
  XQ_TYPE_NS | XQ_TYPE_NQ,
  XQ_TYPE_NS | XQ_TYPE_NQ,
  XQ_TYPE_NS | XQ_TYPE_NQ,
  XQ_TYPE_TOKEN | XQ_TYPE_NQ, // =
  XQ_TYPE_NS | XQ_TYPE_NQ,
  XQ_TYPE_NS | XQ_TYPE_NQ,
  XQ_TYPE_NS | XQ_TYPE_NQ,
  XQ_TYPE_NS | XQ_TYPE_NQ,
  XQ_TYPE_NS | XQ_TYPE_NQ,
  XQ_TYPE_NS | XQ_TYPE_NQ,
  XQ_TYPE_NS | XQ_TYPE_NQ,
  XQ_TYPE_NS | XQ_TYPE_NQ,
  XQ_TYPE_NS | XQ_TYPE_NQ,
  XQ_TYPE_NS | XQ_TYPE_NQ,
  XQ_TYPE_NS | XQ_TYPE_NQ,
  XQ_TYPE_NS | XQ_TYPE_NQ,
  XQ_TYPE_NS | XQ_TYPE_NQ,
  XQ_TYPE_NS | XQ_TYPE_NQ,
  XQ_TYPE_NS | XQ_TYPE_NQ,
  XQ_TYPE_NS | XQ_TYPE_NQ,
  XQ_TYPE_NS | XQ_TYPE_NQ,
  XQ_TYPE_NS | XQ_TYPE_NQ,
  XQ_TYPE_NS | XQ_TYPE_NQ,
  XQ_TYPE_NS | XQ_TYPE_NQ,
  XQ_TYPE_NS | XQ_TYPE_NQ,
  XQ_TYPE_NS | XQ_TYPE_NQ,
  XQ_TYPE_NS | XQ_TYPE_NQ,
  XQ_TYPE_NS | XQ_TYPE_NQ,
  XQ_TYPE_NS | XQ_TYPE_NQ,
  XQ_TYPE_NS | XQ_TYPE_NQ,
  XQ_TYPE_NS | XQ_TYPE_NQ,
  XQ_TYPE_NS | XQ_TYPE_NQ,
  XQ_TYPE_NS | XQ_TYPE_NQ,
  XQ_TYPE_TOKEN | XQ_TYPE_NQ, // [
  XQ_TYPE_TOKEN, // backslash
  XQ_TYPE_TOKEN | XQ_TYPE_NQ // ]
};
#define LAST_TABLE_ENTRY ']'
#define DEFAULT_CHARACTER_CLASS (XQ_TYPE_NS | XQ_TYPE_NQ)

#define xqCharacterClass(c) (c > LAST_TABLE_ENTRY ? DEFAULT_CHARACTER_CLASS : characterClassTable[c])
#define xqIsSpace(c) (xqCharacterClass(c) & XQ_TYPE_SPACE)
#define xqIsNotSpace(c) (xqCharacterClass(c) & XQ_TYPE_NS)
#define xqIsToken(c) (xqCharacterClass(c) & XQ_TYPE_TOKEN)
#define xqIsNotQuote(c) (xqCharacterClass(c) & XQ_TYPE_NQ)


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
static xQStatusCode nextToken(const xmlChar** strPtr, xQToken* tokenOut) {
  const xmlChar* start = *strPtr;
  xmlChar* strStr = 0;
  xmlChar* strOut = 0;
  
  tokenOut->type = XQ_TT_NONE;
  tokenOut->content = 0;
  
  // consume any leading space
  while (*start && xqIsSpace(*start))
    ++start;
  
  // advance the pointer to the end of the token
  *strPtr = start;
  
  // STRING
  if (*start == '"' || *start == '\'') {
    ++(*strPtr);

    strStr = malloc(xmlStrlen(*strPtr));
    if ((!strStr) && (*strPtr))
      return XQ_OUT_OF_MEMORY;
    strOut = strStr;
    
    while (**strPtr) {
      while (xqIsNotQuote(**strPtr))
        *(strOut++) = *((*strPtr)++);

      // escape
      if (**strPtr == '\\') {
        if (xqIsNotQuote(*(*strPtr+1))) {
          *(strOut++) = *((*strPtr)++);
          *(strOut++) = *((*strPtr)++);

        } else if (*(*strPtr+1)) {
          ++(*strPtr);
          *(strOut++) = *((*strPtr)++);
        }

      // opposite quote
      } else if (**strPtr && (**strPtr != *start)) {
        *(strOut++) = *((*strPtr)++);

      // end of string
      } else if (**strPtr) {
        ++(*strPtr); // move past the end of string marker
        *strOut = 0;
        tokenOut->type = XQ_TT_STRING;
        tokenOut->content = strStr;
        return XQ_OK;
        
      }
    }
    
    // unterminated string
    if (strStr) free(strStr);
    return XQ_INVALID_SEL_UNTERMINATED_STR;
    
  // TOKEN
  } else if (xqIsToken(*start)) {
    tokenOut->type = XQ_TT_TOKEN;
    ++(*strPtr);
    
  // IDENT
  } else if (*start) {
    tokenOut->type = XQ_TT_IDENT;
    while (xqIsNotSpace(**strPtr))
      ++(*strPtr);
  
  } else {
    
    return XQ_NO_TOKEN;
    
  }
    
  tokenOut->content = xmlStrndup(start, (*strPtr) - start);
  return XQ_OK;
}

/*
void dumpAST(xQSearchExpr* self, int level) {
  int i;
  
  if (level == 0)
    printf("parsed expression:\n ");
  else
    printf(" ");
  
  for (i = 0; i < level; i++) { printf(" "); }
  
  printf("0x%lx ", (long)self);
  
  if (self->operation == _xQ_findDescendantsByName) {
    printf("_xQ_findDescendantsByName %s\n", self->argv[0]);
  } else if (self->operation == _xQ_findChildrenByName) {
    printf("_xQ_findChildrenByName %s\n", self->argv[0]);
  } else if (self->operation == _xQ_filterAttributeEquals) {
    printf("_xQ_filterAttributeEquals %s %s\n", self->argv[0], self->argv[1]);
  } else if (self->operation == _xQ_addToOutput) {
    printf("_xQ_addToOutput\n");
  } else {
    printf("unknown\n");
  }
  
  if (self->next)
    dumpAST(self->next, level + 1);
}
*/

/**
 * Allocate and initialize a new xQSearchExpr object from an expression
 * string.
 *
 * Returns a pointer to the new instance or 0 on error
 */
xQStatusCode xQSearchExpr_alloc_init(xQSearchExpr** self, const xmlChar* expr) {
  const xmlChar* ptr = expr;
  xQStatusCode status = XQ_OK;
  
  status = xQSearchExpr_parseSelector(self, &ptr);
  
  if (status == XQ_OK && (!*self))
    status = xQSearchExpr_alloc_init_copy(self);
  
  if (status != XQ_OK)
    xQSearchExpr_free(*self);
  
  return status;
}

/**
 * Parse a selector from the string
 *
 * Grammar:
 *
 * selector        <= [ simple_selector | combinator S* simple_selector ] [ S* selector ]*
 */
static xQStatusCode xQSearchExpr_parseSelector(xQSearchExpr** expr, const xmlChar** strPtr) {
  xQToken tok;
  xQStatusCode status = XQ_OK;
  xQSearchExpr* lastExpr = 0;
  *expr = 0;
  
  status = nextToken(strPtr, &tok);
  if (status != XQ_OK)
    return status == XQ_NO_TOKEN ? XQ_OK : status;
  
  // combinator
  if (tok.type == XQ_TT_TOKEN && tok.content[0] == '>') {
    xmlFree(tok.content);
  
    status = nextToken(strPtr, &tok);
    if (status == XQ_OK && tok.type == XQ_TT_IDENT) {
      status = xQSearchExpr_alloc_init_searchImmediate(expr, tok.content);

      if (status == XQ_OK)
        status = xQSearchExpr_parseAttrib(&((*expr)->next), strPtr);
      
      lastExpr = *expr;
      while (status == XQ_OK && lastExpr->next)
        lastExpr = lastExpr->next;
      
      if (status == XQ_OK)
        status = xQSearchExpr_parseSelector(&(lastExpr->next), strPtr);

    } else if (status == XQ_OK) {
      xmlFree(tok.content);
      return XQ_INVALID_SEL_UNEXPECTED_TOKEN;
    }

  // IDENT
  } else if (tok.type == XQ_TT_IDENT) {
    status = xQSearchExpr_alloc_init_searchDescendants(expr, tok.content);
    
    if (status == XQ_OK)
      status = xQSearchExpr_parseAttrib(&((*expr)->next), strPtr);
    
      lastExpr = *expr;
      while (status == XQ_OK && lastExpr->next)
        lastExpr = lastExpr->next;
      
      if (status == XQ_OK)
        status = xQSearchExpr_parseSelector(&(lastExpr->next), strPtr);
  
  // anything else is unexpected
  } else {
    xmlFree(tok.content);
    return XQ_INVALID_SEL_UNEXPECTED_TOKEN;
  }
    
  return status == XQ_NO_TOKEN ? XQ_OK : status;
}

/**
 * Parse any attribute specifications from the string
 *
 * Grammar:
 *
 * attrib          <= '[' S* IDENT S* '=' S* [ string | IDENT ] S* ']'
 */
static xQStatusCode xQSearchExpr_parseAttrib(xQSearchExpr** expr, const xmlChar** strPtr) {
  xQToken tok;
  xQStatusCode status = XQ_OK;
  const xmlChar* peekPtr = *strPtr;
  xmlChar* attrName = 0;
  xmlChar operChar = 0;
  xmlChar* attrValue = 0;
  *expr = 0;
  
  status = nextToken(&peekPtr, &tok);
  
  // [
  if (status == XQ_OK && tok.type == XQ_TT_TOKEN && tok.content[0] == '[') {

    xmlFree(tok.content);
    
    // IDENT
    status = nextToken(&peekPtr, &tok);
    if (status == XQ_OK)
      attrName = tok.content;
    if (status == XQ_OK && tok.type != XQ_TT_IDENT)
      status = XQ_INVALID_SEL_UNEXPECTED_TOKEN;

    
    // =
    if (status == XQ_OK)
      status = nextToken(&peekPtr, &tok);
    if (status == XQ_OK) {
      operChar = tok.content[0];
      xmlFree(tok.content);
    }
    if (status == XQ_OK && (tok.type != XQ_TT_TOKEN || operChar != '='))
      status = XQ_INVALID_SEL_UNEXPECTED_TOKEN;
      
    // string | IDENT
    status = nextToken(&peekPtr, &tok);
    if (status == XQ_OK)
      attrValue = tok.content;
    if (status == XQ_OK && tok.type != XQ_TT_STRING && tok.type != XQ_TT_IDENT)
      status = XQ_INVALID_SEL_UNEXPECTED_TOKEN;
    
    // ]
    if (status == XQ_OK)
      status = nextToken(&peekPtr, &tok);
    if (status == XQ_OK) {
      operChar = tok.content[0];
      xmlFree(tok.content);
    }
    if (status == XQ_OK && (tok.type != XQ_TT_TOKEN || operChar != ']'))
      status = XQ_INVALID_SEL_UNEXPECTED_TOKEN;
    
    if (status != XQ_OK) {
      if (attrName) xmlFree(attrName);
      if (attrValue) xmlFree(attrValue);
      return status;
    }
    
    status = xQSearchExpr_alloc_init_filterAttrEquals(expr, attrName, attrValue);
    
    if (status == XQ_OK) {
      *strPtr = peekPtr;
      return xQSearchExpr_parseAttrib(&((*expr)->next), strPtr);
    }
    
  } else if (status == XQ_OK) {
    // no attributes
    xmlFree(tok.content);
  }
  
  return status;
}

/**
 * Allocate and initialize a new xQSearchExpr object that copies the
 * input node to the output.
 *
 * Returns a pointer to the new instance or 0 on error
 */
static xQStatusCode xQSearchExpr_alloc_init_copy(xQSearchExpr** self) {
  
  *self = (xQSearchExpr*) malloc(sizeof(xQSearchExpr));
  if (!*self)
    return XQ_OUT_OF_MEMORY;
  
  (*self)->argc = 0;
  (*self)->argv = 0;
  (*self)->operation = _xQ_addToOutput;
  (*self)->next = 0;
  
  return XQ_OK;
}

/**
 * Allocate and initialize a new xQSearchExpr object that searches the
 * input node for descendants with a given name.
 *
 * Returns a pointer to the new instance or 0 on error
 */
static xQStatusCode xQSearchExpr_alloc_init_searchDescendants(xQSearchExpr** self, xmlChar* name) {
  
  *self = (xQSearchExpr*) malloc(sizeof(xQSearchExpr));
  if (!(*self)) {
    xmlFree(name);
    return XQ_OUT_OF_MEMORY;
  }
  
  (*self)->argv = (xmlChar**) malloc(sizeof(xmlChar*));
  if ((*self)->argv) {
    (*self)->argc = 1;
    (*self)->argv[0] = name;
    (*self)->operation = _xQ_findDescendantsByName;
    (*self)->next = 0;
  } else {
    xmlFree(name);
    free(*self);
    return XQ_OUT_OF_MEMORY;
  }
  
  return XQ_OK;
}

/**
 * Allocate and initialize a new xQSearchExpr object that searches the
 * input node for immediate children with a given name.
 *
 * Returns a pointer to the new instance or 0 on error
 */
static xQStatusCode xQSearchExpr_alloc_init_searchImmediate(xQSearchExpr** self, xmlChar* name) {
  
  *self = (xQSearchExpr*) malloc(sizeof(xQSearchExpr));
  if (!(*self)) {
    xmlFree(name);
    return XQ_OUT_OF_MEMORY;
  }
  
  (*self)->argv = (xmlChar**) malloc(sizeof(xmlChar*));
  if ((*self)->argv) {
    (*self)->argc = 1;
    (*self)->argv[0] = name;
    (*self)->operation = _xQ_findChildrenByName;
    (*self)->next = 0;
  } else {
    xmlFree(name);
    free(*self);
    return XQ_OUT_OF_MEMORY;
  }
  
  return XQ_OK;
}

/**
 * Allocate and initialize a new xQSearchExpr object that copies the
 * input node only if it has a named attribute with a given value.
 *
 * Returns a pointer to the new instance or 0 on error
 */
static xQStatusCode xQSearchExpr_alloc_init_filterAttrEquals(xQSearchExpr** self, xmlChar* name, xmlChar* value) {

  (*self) = (xQSearchExpr*) malloc(sizeof(xQSearchExpr));
  if (!self) {
    xmlFree(name);
    xmlFree(value);
    return XQ_OUT_OF_MEMORY;
  }
  
  (*self)->argv = (xmlChar**) malloc(sizeof(xmlChar*) * 2);
  if ((*self)->argv) {
    (*self)->argc = 2;
    (*self)->argv[0] = name;
    (*self)->argv[1] = value;
    (*self)->operation = _xQ_filterAttributeEquals;
    (*self)->next = 0;
  } else {
    xmlFree(name);
    xmlFree(value);
    free(*self);
    return XQ_OUT_OF_MEMORY;
  }
  
  return XQ_OK;
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
