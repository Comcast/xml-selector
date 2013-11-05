/**
 * Search expression routines
 */

#include "libxq.h"

#include <string.h>

// local (private) routines and data types
typedef enum {
  XQ_TT_TOKEN,
  XQ_TT_IDENT,
  XQ_TT_STRING,
  XQ_TT_NONE
} xQTokenType;

typedef struct _xQToken {
  xQTokenType type;
  xmlChar* content;
  const xmlChar* strPtr;
} xQToken;

static xQStatusCode xQSearchExpr_alloc_init_copy(xQSearchExpr** self);
static xQStatusCode xQSearchExpr_alloc_init_searchDescendants(xQSearchExpr** self, xmlChar* name);
static xQStatusCode xQSearchExpr_alloc_init_searchImmediate(xQSearchExpr** self, xmlChar* name);
static xQStatusCode xQSearchExpr_alloc_init_filterAttrEquals(xQSearchExpr** self, xmlChar* name, xmlChar* value);
static xQStatusCode xQSearchExpr_parseSelector(xQSearchExpr** expr, xQToken* tok);
static xQStatusCode xQSearchExpr_parseAttrib(xQSearchExpr** expr, xQToken* tok);
static xQStatusCode nextToken(xQToken* tokenContext);
static int xmlstrpos(const xmlChar* haystack, xmlChar needle);
#define initToken(t) memset(t, 0, sizeof(xQToken))
#define copyToken(dest, src) memcpy(dest, src, sizeof(xQToken))

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
  XQ_TYPE_TOKEN | XQ_TYPE_NQ, // >
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
static xQStatusCode nextToken(xQToken* tokenContext) {
  const xmlChar* start = tokenContext->strPtr;
  xmlChar* strStr = 0;
  xmlChar* strOut = 0;
  
  tokenContext->type = XQ_TT_NONE;
  tokenContext->content = 0;
  
  // consume any leading space
  while (*start && xqIsSpace(*start))
    ++start;
  
  // advance the pointer to the end of the token
  tokenContext->strPtr = start;
  
  // STRING
  if (*start == '"' || *start == '\'') {
    ++(tokenContext->strPtr);

    strStr = malloc(xmlStrlen(tokenContext->strPtr));
    if ((!strStr) && *(tokenContext->strPtr))
      return XQ_OUT_OF_MEMORY;
    strOut = strStr;
    
    while (*(tokenContext->strPtr)) {
      while (xqIsNotQuote(*(tokenContext->strPtr)))
        *(strOut++) = *(tokenContext->strPtr++);

      // escape
      if (*(tokenContext->strPtr) == '\\') {
        if (xqIsNotQuote(*(tokenContext->strPtr+1))) {
          *(strOut++) = *(tokenContext->strPtr++);
          *(strOut++) = *(tokenContext->strPtr++);

        } else if (*(tokenContext->strPtr+1)) {
          ++(tokenContext->strPtr);
          *(strOut++) = *(tokenContext->strPtr++);
        }

      // opposite quote
      } else if (*(tokenContext->strPtr) && (*(tokenContext->strPtr) != *start)) {
        *(strOut++) = *(tokenContext->strPtr++);

      // end of string
      } else if (*(tokenContext->strPtr)) {
        ++(tokenContext->strPtr); // move past the end of string marker
        *strOut = 0;
        tokenContext->type = XQ_TT_STRING;
        tokenContext->content = strStr;
        return XQ_OK;
        
      }
    }
    
    // unterminated string
    if (strStr) free(strStr);
    return XQ_INVALID_SEL_UNTERMINATED_STR;
    
  // TOKEN
  } else if (xqIsToken(*start)) {
    tokenContext->type = XQ_TT_TOKEN;
    ++(tokenContext->strPtr);
    
  // IDENT
  } else if (*start) {
    tokenContext->type = XQ_TT_IDENT;
    while (xqIsNotSpace(*(tokenContext->strPtr)))
      ++(tokenContext->strPtr);
  
  } else {
    
    return XQ_NO_TOKEN;
    
  }
    
  tokenContext->content = xmlStrndup(start, tokenContext->strPtr - start);
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
  xQToken tok;
  
  initToken(&tok);
  tok.strPtr = expr;
  
  status = xQSearchExpr_parseSelector(self, &tok);
  
  if (status == XQ_OK && (!*self))
    status = xQSearchExpr_alloc_init_copy(self);
  
  if (status != XQ_OK)
    xQSearchExpr_free(*self);
  
  return status;
}

/**
 * Return the tail from an xQSearchExpr list
 */
xQSearchExpr* expressionTail(xQSearchExpr** expr) {
  xQSearchExpr* lastExpr = *expr;

  while (lastExpr && lastExpr->next)
    lastExpr = lastExpr->next;
  
  return lastExpr;
}

/**
 * Parse a selector from the string
 *
 * Grammar:
 *
 * selector        <= [ simple_selector | combinator S* simple_selector ] [ S* selector ]*
 */
static xQStatusCode xQSearchExpr_parseSelector(xQSearchExpr** expr, xQToken* tok) {
  xQStatusCode status = XQ_OK;
  *expr = 0;
  
  status = nextToken(tok);
  if (status != XQ_OK)
    return status == XQ_NO_TOKEN ? XQ_OK : status;
  
  // combinator
  if (tok->type == XQ_TT_TOKEN && tok->content[0] == '>') {
    xmlFree(tok->content);
  
    status = nextToken(tok);
    if (status == XQ_OK && tok->type == XQ_TT_IDENT) {
      status = xQSearchExpr_alloc_init_searchImmediate(expr, tok->content);

      if (status == XQ_OK)
        status = xQSearchExpr_parseAttrib(&((*expr)->next), tok);
      
      if (status == XQ_OK)
        status = xQSearchExpr_parseSelector(&(expressionTail(expr)->next), tok);

    } else if (status == XQ_OK) {
      xmlFree(tok->content);
      return XQ_INVALID_SEL_UNEXPECTED_TOKEN;
    }

  // IDENT
  } else if (tok->type == XQ_TT_IDENT) {
    status = xQSearchExpr_alloc_init_searchDescendants(expr, tok->content);
    
    if (status == XQ_OK)
      status = xQSearchExpr_parseAttrib(&((*expr)->next), tok);
    
    if (status == XQ_OK)
      status = xQSearchExpr_parseSelector(&(expressionTail(expr)->next), tok);
  
  // anything else is unexpected
  } else {
    xmlFree(tok->content);
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
static xQStatusCode xQSearchExpr_parseAttrib(xQSearchExpr** expr, xQToken* tok) {
  xQToken peek;
  xQStatusCode status = XQ_OK;
  xmlChar* attrName = 0;
  xmlChar* oper = 0;
  xmlChar* attrValue = 0;
  xmlChar* endAttr = 0;
  *expr = 0;
  
  copyToken(&peek, tok);
  
  status = nextToken(&peek);
  
  // [
  if (status == XQ_OK && peek.type == XQ_TT_TOKEN && peek.content[0] == '[') {

    xmlFree(peek.content);
    
    // IDENT
    status = nextToken(&peek);
    if (status == XQ_OK) attrName = peek.content;
    
    if (status == XQ_OK && peek.type != XQ_TT_IDENT)
      status = XQ_INVALID_SEL_UNEXPECTED_TOKEN;

    // =
    if (status == XQ_OK) status = nextToken(&peek);
    if (status == XQ_OK) oper = peek.content;

    if (status == XQ_OK && (peek.type != XQ_TT_TOKEN || *oper != '='))
      status = XQ_INVALID_SEL_UNEXPECTED_TOKEN;
    
    // string | IDENT
    if (status == XQ_OK) status = nextToken(&peek);
    if (status == XQ_OK) attrValue = peek.content;
    
    if (status == XQ_OK && peek.type != XQ_TT_STRING && peek.type != XQ_TT_IDENT)
      status = XQ_INVALID_SEL_UNEXPECTED_TOKEN;

    // ]
    if (status == XQ_OK) status = nextToken(&peek);
    if (status == XQ_OK) endAttr = peek.content;
    
    if (status == XQ_OK && (peek.type != XQ_TT_TOKEN || *endAttr != ']'))
      status = XQ_INVALID_SEL_UNEXPECTED_TOKEN;
    
    if (oper) xmlFree(oper);
    if (endAttr) xmlFree(endAttr);
    
    if (status != XQ_OK) {
      if (attrName) xmlFree(attrName);
      if (attrValue) xmlFree(attrValue);
    }
    
    if (status == XQ_NO_TOKEN)
      status = XQ_INVALID_SEL_UNEXPECTED_TOKEN;

    if (status == XQ_OK)
      status = xQSearchExpr_alloc_init_filterAttrEquals(expr, attrName, attrValue);
    
    if (status == XQ_OK) {
      copyToken(tok, &peek);
      return xQSearchExpr_parseAttrib(&((*expr)->next), tok);
    }
    
  } else if (status == XQ_OK) {
    // no attributes
    xmlFree(peek.content);
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
