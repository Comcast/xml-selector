/**
 * Tree traversal routines
 */

#include "libxq.h"

#include <string.h>

/**
 * Search all decendants of node for elements and populate the output
 * list with the results.
 *
 * Returns a 0 (XQ_OK) on success, an error code otherwise
 */
xQStatusCode _xQ_findDescendants(xQ* context, xmlChar** args, xmlNodePtr node, xQNodeList* outList) {
  xQStatusCode result = XQ_OK;
  xmlNodePtr cur = node ? node->children : node;
  
  while (cur && result == XQ_OK) {
    
    if (cur->type == XML_ELEMENT_NODE) {
      result = xQNodeList_push(outList, cur);
      
      if (cur->children && result == XQ_OK)
        result = _xQ_findDescendants(context, args, cur, outList);
    }
    
    cur = cur->next;
  }
  
  return result;
}

/**
 * Search all decendants of node for elements matching name and populate
 * the output list with the results.
 *
 * Returns a 0 (XQ_OK) on success, an error code otherwise
 */
xQStatusCode _xQ_findDescendantsByName(xQ* context, xmlChar** args, xmlNodePtr node, xQNodeList* outList) {
  const xmlChar* name = args[0];
  xQStatusCode result = XQ_OK;
  xmlNodePtr cur = node ? node->children : node;
  
  while (cur && result == XQ_OK) {
    
    if (cur->type == XML_ELEMENT_NODE) {
      if (xmlStrcmp(name, cur->name) == 0)
        result = xQNodeList_push(outList, cur);
      
      if (cur->children && result == XQ_OK)
        result = _xQ_findDescendantsByName(context, args, cur, outList);
    }
    
    cur = cur->next;
  }
  
  return result;
}

/**
 * Search immediate children of node for elements matching name and populate
 * the output list with the results.
 *
 * Returns a 0 (XQ_OK) on success, an error code otherwise
 */
xQStatusCode _xQ_findChildrenByName(xQ* context, xmlChar** args, xmlNodePtr node, xQNodeList* outList) {
  const xmlChar* name = args[0];
  xQStatusCode result = XQ_OK;
  xmlNodePtr cur = node ? node->children : node;
  
  while (cur && result == XQ_OK) {
    
    if (cur->type == XML_ELEMENT_NODE) {
      if (xmlStrcmp(name, cur->name) == 0)
        result = xQNodeList_push(outList, cur);
    }
    
    cur = cur->next;
  }
  
  return result;
}

/**
 * Examine next sibling of node for a matching name and the sibling to
 * the output list if it matches.
 *
 * Returns a 0 (XQ_OK) on success, an error code otherwise
 */
xQStatusCode _xQ_findNextSiblingByName(xQ* context, xmlChar** args, xmlNodePtr node, xQNodeList* outList) {
  const xmlChar* name = args[0];
  xQStatusCode result = XQ_OK;
  xmlNodePtr sibling;
  
  if (node->type == XML_ELEMENT_NODE && (sibling = xmlNextElementSibling(node))) {
    if (xmlStrcmp(name, sibling->name) == 0)
      result = xQNodeList_push(outList, sibling);
  }
  
  return result;
}

/**
 * Copy node to the output list
 *
 * Returns a 0 (XQ_OK) on success, an error code otherwise
 */
xQStatusCode _xQ_addToOutput(xQ* context, xmlChar** args, xmlNodePtr node, xQNodeList* outList) {
  return xQNodeList_push(outList, node);
}

/**
 * Add the node to the output list only if it has an attribute with an
 * exact value.
 *
 * Returns a 0 (XQ_OK) on success, an error code otherwise
 */
xQStatusCode _xQ_filterAttributeEquals(xQ* context, xmlChar** args, xmlNodePtr node, xQNodeList* outList) {
  const xmlChar* name = args[0];
  const xmlChar* value = args[1];
  xmlChar* thisValue;
  xQStatusCode result = XQ_OK;
  
  if (node) {
    thisValue = xmlGetProp(node, name);
    if (thisValue) {
      
      if (xmlStrcmp(thisValue, value) == 0)
        result = xQNodeList_push(outList, node);
      
      xmlFree(thisValue);
    }
  }
  
  return result;
}

/**
 * Add the node to the output list only if its name matches a given value.
 *
 * Returns a 0 (XQ_OK) on success, an error code otherwise
 */
xQStatusCode _xQ_filterByName(xQ* context, xmlChar** args, xmlNodePtr node, xQNodeList* outList) {
  const xmlChar* name = args[0];
  xQStatusCode result = XQ_OK;

  if (node) {
    
    if (node->type == XML_ELEMENT_NODE) {
      if (xmlStrcmp(name, node->name) == 0)
        result = xQNodeList_push(outList, node);
      
    }
    
  }
  
  return result;
}
