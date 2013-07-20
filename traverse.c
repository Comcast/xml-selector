/**
 * Tree traversal routines
 */

#include "libxq.h"

#include <string.h>

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
