/**
 * Search expression routines
 */

#include "libxq.h"
#include "xqutil.h"

#include <string.h>

const xmlChar* XQ_EMPTY_NAMESPACE = (xmlChar*)"";

// local (private) routines and data types
typedef enum {
  XQ_TT_NONE = 0,
  XQ_TT_TOKEN,
  XQ_TT_IDENT,
  XQ_TT_STRING,
} xQTokenType;

typedef struct _xQToken {
  xQTokenType type;
  xmlChar* content;
  int length;
  const xmlChar* strPtr;
  xQStatusCode lastStatus;
} xQToken;

typedef xQStatusCode (*xQSearchExprCtorPtr)(xQSearchExpr**, xmlChar*, xmlChar*);

static xQStatusCode xQSearchExpr_alloc_init_copy(xQSearchExpr** self);
static xQStatusCode xQSearchExpr_alloc_init_allDescendants(xQSearchExpr** self);
static xQStatusCode xQSearchExpr_alloc_init_searchDescendants(xQSearchExpr** self, xmlChar* name, xmlChar* ns);
static xQStatusCode xQSearchExpr_alloc_init_searchImmediate(xQSearchExpr** self, xmlChar* name, xmlChar* ns);
static xQStatusCode xQSearchExpr_alloc_init_searchNextSibling(xQSearchExpr** self, xmlChar* name, xmlChar* ns);
static xQStatusCode xQSearchExpr_alloc_init_filterAttrEquals(xQSearchExpr** self, xmlChar* name, xmlChar* value);
static xQStatusCode xQSearchExpr_parseSelector(xQSearchExpr** expr, xQToken* tok);
static XQINLINE xQStatusCode xQSearchExpr_parseSingleSelector(xQSearchExpr** expr, xQToken* tok);
static XQINLINE xQStatusCode xQSearchExpr_parseCombinator(xQToken* tok, xQSearchExprCtorPtr* ctor);
static XQINLINE xQStatusCode xQSearchExpr_parseSimpleSelector(xQSearchExpr** expr, xQToken* tok, xQSearchExprCtorPtr ctor);
static XQINLINE xQStatusCode xQSearchExpr_parseElementName(xQToken* tok, xmlChar** nsPrefix, xmlChar** name, int* isWildcard);
static xQStatusCode xQSearchExpr_parseAttribs(xQSearchExpr** expr, xQToken* tok);
static xQStatusCode nextToken(xQToken* tokenContext);
static int xmlstrpos(const xmlChar* haystack, xmlChar needle);
#define initToken(t) memset(t, 0, sizeof(xQToken))
#define copyToken(dest, src) memcpy(dest, src, sizeof(xQToken))
#define tokenFirstChar(t) ((t)->content ? (t)->content[0] : 0)
#define destroyToken(t) if ((t) && (t)->content) xmlFree((t)->content);

#define freeAndResetPtr(s) if (s) { xmlFree(s); s = 0; }

/*
 * Selector grammar:
 *
 * selector        ::= single_selector | single_selector selector
 * single_selector ::= combinator simple_selector | simple_selector
 * combinator      ::= '>' | '+'
 * simple_selector ::= element_name | element_name attribs
 * element_name    ::= IDENT ':' IDENT | IDENT | '*'
 * attribs         ::=  attrib | attrib attribs
 * attrib          ::= '[' IDENT  '='  ( string | IDENT ) ']'
 * string          ::= '"' ( [^'"\\] | "'" | escape )* '"' | "'" ( [^'"\\] | '"' | escape )* "'"
 * escape          ::= '\' [\\'"]
 * IDENT           ::= [^ \t\r\n"'*+:=>\[\\\]]+
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
  XQ_TYPE_TOKEN | XQ_TYPE_NQ, // *
  XQ_TYPE_TOKEN | XQ_TYPE_NQ, // +
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
  XQ_TYPE_TOKEN | XQ_TYPE_NQ, // :
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
  if (tokenContext->content) xmlFree(tokenContext->content);
  tokenContext->content = 0;
  tokenContext->length = 0;
  
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
      return tokenContext->lastStatus = XQ_OUT_OF_MEMORY;
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
        tokenContext->length = strOut - strStr;
        return tokenContext->lastStatus = XQ_OK;
        
      }
    }
    
    // unterminated string
    if (strStr) free(strStr);
    return tokenContext->lastStatus = XQ_INVALID_SEL_UNTERMINATED_STR;
    
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
    
    return tokenContext->lastStatus = XQ_NO_TOKEN;
    
  }
    
  tokenContext->content = xmlStrndup(start, tokenContext->strPtr - start);
  tokenContext->length = tokenContext->strPtr - start;
  return tokenContext->lastStatus = XQ_OK;
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
  nextToken(&tok);
  
  status = xQSearchExpr_parseSelector(self, &tok);
  
  if (status == XQ_NO_MATCH) {
    if (tok.lastStatus != XQ_OK && tok.lastStatus != XQ_NO_TOKEN)
      status = tok.lastStatus;
    else if (tok.type != XQ_TT_NONE)
      status = XQ_INVALID_SEL_UNEXPECTED_TOKEN;
    else
      status = XQ_OK;
  }

  destroyToken(&tok);
  
  if (status == XQ_OK && (!*self))
    status = xQSearchExpr_alloc_init_copy(self);
  
  if (status != XQ_OK) {
    xQSearchExpr_free(*self);
    *self = 0;
  }
  
  return status;
}

/**
 * Allocate and initialize a new xQSearchExpr object from an expression
 * string. The created expression can be used for filtering a node list,
 * as opposted to searching (searching self instead of descendants).
 *
 * Returns a pointer to the new instance or 0 on error
 */
xQStatusCode xQSearchExpr_alloc_initFilter(xQSearchExpr** self, const xmlChar* expr) {
  const xmlChar* ptr = expr;
  xQStatusCode status = XQ_OK;
  xQToken tok;
  
  initToken(&tok);

  tok.strPtr = expr;
  nextToken(&tok);
  
  status = xQSearchExpr_parseSelector(self, &tok);
  
  if (status == XQ_NO_MATCH) {
    if (tok.lastStatus != XQ_OK && tok.lastStatus != XQ_NO_TOKEN)
      status = tok.lastStatus;
    else if (tok.type != XQ_TT_NONE)
      status = XQ_INVALID_SEL_UNEXPECTED_TOKEN;
    else
      status = XQ_OK;
  }

  destroyToken(&tok);

  if (status == XQ_OK && (!*self))
    status = xQSearchExpr_alloc_init_copy(self);
  else if (status == XQ_OK) {
    // convert to self-search
    if ((*self)->operation == _xQ_findDescendants)
      (*self)->operation = _xQ_addToOutput;
    else if ((*self)->operation == _xQ_findDescendantsByName)
      (*self)->operation = _xQ_filterByName;
  }
  
  if (status != XQ_OK)
    xQSearchExpr_free(*self);
  
  return status;
}

/**
 * Return the tail from an xQSearchExpr list
 */
static XQINLINE xQSearchExpr* expressionTail(xQSearchExpr** expr) {
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
 * selector        ::= single_selector | single_selector selector
 */
static xQStatusCode xQSearchExpr_parseSelector(xQSearchExpr** expr, xQToken* tok) {
  xQStatusCode status = XQ_OK;
  *expr = 0;
  
  // single_selector
  status = xQSearchExpr_parseSingleSelector(expr, tok);

  // | single_selector selector
  if (status == XQ_OK)
    status = xQSearchExpr_parseSelector(&(expressionTail(expr)->next), tok);
  
  return status;
}

/**
 * Parse a single selector from the string
 *
 * Grammar:
 *
 * single_selector ::= combinator simple_selector | simple_selector
 */
static XQINLINE xQStatusCode xQSearchExpr_parseSingleSelector(xQSearchExpr** expr, xQToken* tok) {
  xQStatusCode status = XQ_OK;
  xQStatusCode cmbStatus = XQ_OK;
  xQSearchExprCtorPtr ctor = xQSearchExpr_alloc_init_searchDescendants;
  *expr = 0;
  
  // combinator simple_selector
  cmbStatus = xQSearchExpr_parseCombinator(tok, &ctor);
  
  // | simple_selector
  status = xQSearchExpr_parseSimpleSelector(expr, tok, ctor);
  
  if (status == XQ_NO_MATCH && cmbStatus != XQ_NO_MATCH)
    return XQ_INVALID_SEL_UNEXPECTED_TOKEN;
  
  return status;
}
  
/**
 * Parse a combinator and selector from the string
 *
 * Grammar:
 *
 * combinator      <= '>' | '+'
 */
static XQINLINE xQStatusCode xQSearchExpr_parseCombinator(xQToken* tok, xQSearchExprCtorPtr* ctorPtr) {

  if (tok->type == XQ_TT_TOKEN && tokenFirstChar(tok) == '>') {
    *ctorPtr = xQSearchExpr_alloc_init_searchImmediate;
    nextToken(tok);
    return XQ_OK;
    
  } else if (tok->type == XQ_TT_TOKEN && tokenFirstChar(tok) == '+') {
    *ctorPtr = xQSearchExpr_alloc_init_searchNextSibling;
    nextToken(tok);
    return XQ_OK;
    
  } else {
    return XQ_NO_MATCH;
  }

}

/**
 * Parse a simple selector
 *
 * Grammar:
 *
 * simple_selector ::= element_name | element_name attribs
 */
static XQINLINE xQStatusCode xQSearchExpr_parseSimpleSelector(xQSearchExpr** expr, xQToken* tok, xQSearchExprCtorPtr ctor) {
  xQStatusCode status = XQ_OK;
  xmlChar* nsPrefix = 0;
  xmlChar* name = 0;
  int isWildcard = 0;
  
  // element_name
  status = xQSearchExpr_parseElementName(tok, &nsPrefix, &name, &isWildcard);

  if (status == XQ_OK && isWildcard)
    status = xQSearchExpr_alloc_init_allDescendants(expr);
  else if (status == XQ_OK)
    status = ctor(expr, name, nsPrefix);
  
  // | element_name attribs
  if (status == XQ_OK)
    status = xQSearchExpr_parseAttribs(&((*expr)->next), tok);
  
  return status;
}

/**
 * Parse an element name
 *
 * Grammar:
 *
 * element_name    ::= IDENT ':' IDENT | IDENT | '*'
 */
static XQINLINE xQStatusCode xQSearchExpr_parseElementName(xQToken* tok, xmlChar** nsPrefix, xmlChar** name, int* isWildcard) {
  xQStatusCode status = XQ_OK;
  *isWildcard = 0;
  
  // IDENT ':' IDENT | IDENT
  if (tok->type == XQ_TT_IDENT) {
    
    *name = xmlStrndup(tok->content, tok->length);
    if (!(*name))
      status = XQ_OUT_OF_MEMORY;
    
    if (status == XQ_OK)
      nextToken(tok);
    
    // IDENT ':' IDENT
    if (status == XQ_OK && tok->type == XQ_TT_TOKEN && tokenFirstChar(tok) == ':') {
      
      nextToken(tok);
      
      if (tok->type != XQ_TT_IDENT)
        status = XQ_INVALID_SEL_UNEXPECTED_TOKEN;
      
      if (status == XQ_OK) {
        *nsPrefix = *name;
        *name = xmlStrndup(tok->content, tok->length);
      }
      
      if (!(*name))
        status = XQ_OUT_OF_MEMORY;
    }
    
  
  // | '*'
  } else if (tok->type == XQ_TT_TOKEN && tokenFirstChar(tok) == '*') {
    
    *isWildcard = 1;
    nextToken(tok);

  } else {
    return XQ_NO_MATCH;
  }
  
  if (status != XQ_OK) {
    freeAndResetPtr(*name);
    freeAndResetPtr(*nsPrefix);
  }
  
  return status;

}

/**
 * Parse any attribute specifications from the string
 *
 * Grammar:
 *
 * attribs         ::=  attrib | attrib attribs
 * attrib          ::= '[' IDENT  '='  ( string | IDENT ) ']'
 */
static xQStatusCode xQSearchExpr_parseAttribs(xQSearchExpr** expr, xQToken* tok) {
  xQStatusCode status = XQ_OK;
  xmlChar* attrName = 0;
  xmlChar* attrValue = 0;
  
  // [
  if (tok->type == XQ_TT_TOKEN && tokenFirstChar(tok) == '[') {

    nextToken(tok);
    
    // IDENT
    if (tok->type != XQ_TT_IDENT)
      status = XQ_INVALID_SEL_UNEXPECTED_TOKEN;
    
    if (status == XQ_OK)
      status = (attrName = xmlStrndup(tok->content, tok->length)) ? XQ_OK : XQ_OUT_OF_MEMORY;
    
    if (status == XQ_OK)
      nextToken(tok);

    // =
    if (status == XQ_OK && (tok->type != XQ_TT_TOKEN || tokenFirstChar(tok) != '='))
      status = XQ_INVALID_SEL_UNEXPECTED_TOKEN;

    if (status == XQ_OK)
      nextToken(tok);
    
    // string | IDENT
    if (status == XQ_OK && (tok->type != XQ_TT_STRING && tok->type != XQ_TT_IDENT))
      status = tok->lastStatus == XQ_OK ? XQ_INVALID_SEL_UNEXPECTED_TOKEN : tok->lastStatus;

    if (status == XQ_OK)
      status = (attrValue = xmlStrndup(tok->content, tok->length)) ? XQ_OK : XQ_OUT_OF_MEMORY;

    if (status == XQ_OK)
      nextToken(tok);

    // ]
    if (status == XQ_OK && (tok->type != XQ_TT_TOKEN || tokenFirstChar(tok) != ']'))
      status = XQ_INVALID_SEL_UNEXPECTED_TOKEN;

    if (status == XQ_OK)
      nextToken(tok);

    if (status == XQ_OK)
      status = xQSearchExpr_alloc_init_filterAttrEquals(expr, attrName, attrValue);
    
    if (status != XQ_OK) {
      freeAndResetPtr(attrName);
      freeAndResetPtr(attrValue);
    }
    
    if (status == XQ_OK)
      return xQSearchExpr_parseAttribs(&((*expr)->next), tok);
    
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
 * Allocate and initialize a new xQSearchExpr object that copies all
 * descendants of the input node to the output.
 *
 * Returns a pointer to the new instance or 0 on error
 */
static xQStatusCode xQSearchExpr_alloc_init_allDescendants(xQSearchExpr** self) {

  *self = (xQSearchExpr*) malloc(sizeof(xQSearchExpr));
  if (!*self)
    return XQ_OUT_OF_MEMORY;
  
  (*self)->argc = 0;
  (*self)->argv = 0;
  (*self)->operation = _xQ_findDescendants;
  (*self)->next = 0;
  
  return XQ_OK;
}

/**
 * Allocate and initialize a new xQSearchExpr object that searches the
 * input node for descendants with a given name.
 *
 * Returns a pointer to the new instance or 0 on error
 */
static xQStatusCode xQSearchExpr_alloc_init_searchDescendants(xQSearchExpr** self, xmlChar* name, xmlChar* ns) {
  
  *self = (xQSearchExpr*) malloc(sizeof(xQSearchExpr));
  if (!(*self)) {
    xmlFree(name);
    return XQ_OUT_OF_MEMORY;
  }
  
  (*self)->argv = (xmlChar**) malloc(sizeof(xmlChar*) * 2);
  if ((*self)->argv) {
    (*self)->argc = 2;
    (*self)->argv[0] = name;
    (*self)->argv[1] = ns;
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
static xQStatusCode xQSearchExpr_alloc_init_searchImmediate(xQSearchExpr** self, xmlChar* name, xmlChar* ns) {
  
  *self = (xQSearchExpr*) malloc(sizeof(xQSearchExpr));
  if (!(*self)) {
    xmlFree(name);
    return XQ_OUT_OF_MEMORY;
  }
  
  (*self)->argv = (xmlChar**) malloc(sizeof(xmlChar*) * 2);
  if ((*self)->argv) {
    (*self)->argc = 2;
    (*self)->argv[0] = name;
    (*self)->argv[1] = ns;
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
 * Allocate and initialize a new xQSearchExpr object that searches the
 * input node for next siblings with a given name.
 *
 * Returns a pointer to the new instance or 0 on error
 */
static xQStatusCode xQSearchExpr_alloc_init_searchNextSibling(xQSearchExpr** self, xmlChar* name, xmlChar* ns) {
  
  *self = (xQSearchExpr*) malloc(sizeof(xQSearchExpr));
  if (!(*self)) {
    xmlFree(name);
    return XQ_OUT_OF_MEMORY;
  }
  
  (*self)->argv = (xmlChar**) malloc(sizeof(xmlChar*) * 2);
  if ((*self)->argv) {
    (*self)->argc = 2;
    (*self)->argv[0] = name;
    (*self)->argv[1] = ns;
    (*self)->operation = _xQ_findNextSiblingByName;
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
