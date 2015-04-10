var $$ = require('../../index')
;


/**
 * nodeType - should return the DOM node type
 */
module.exports['nodeType - should return the DOM node type'] = function(test) {
  var doc = $$.parseFromString("<doc></doc>");
  test.strictEqual(doc.nodeType, 9); // Node.DOCUMENT_NODE
  test.done();
}

/**
 * nodeName - should return the tag name for an element
 */
module.exports['nodeName - should return the tag name for an element'] = function(test) {
  var doc = $$.parseFromString("<doc></doc>");
  test.strictEqual(doc.documentElement.nodeName, "doc");
  test.done();
}

/**
 * nodeName - should allow UTF-8 encoded values
 */
module.exports['nodeName - should allow UTF-8 encoded values'] = function(test) {
  var doc = $$.parseFromString("<d\u00F6c></d\u00F6c>");
  test.strictEqual(doc.documentElement.nodeName, "d\u00F6c");
  test.done();
}

/**
 * firstChild - should return the first child node
 */
module.exports['firstChild - should return the first child node'] = function(test) {
  var doc = $$.parseFromString("<doc><a/><b/><c/></doc>");
  test.ok(doc.documentElement.firstChild);
  test.strictEqual(doc.documentElement.firstChild.nodeType, 1) // Node.ELEMENT_NODE
  test.strictEqual(doc.documentElement.firstChild.nodeName, "a");
  test.done();
}

/**
 * firstChild - should return null for no children
 */
module.exports['firstChild - should return null for no children'] = function(test) {
  var doc = $$.parseFromString("<doc></doc>");
  test.strictEqual(doc.documentElement.firstChild, null);
  test.done();
}

/**
 * lastChild - should return the last child node
 */
module.exports['lastChild - should return the last child node'] = function(test) {
  var doc = $$.parseFromString("<doc><a/><b/><c/></doc>");
  test.ok(doc.documentElement.lastChild);
  test.strictEqual(doc.documentElement.lastChild.nodeType, 1) // Node.ELEMENT_NODE
  test.strictEqual(doc.documentElement.lastChild.nodeName, "c");
  test.done();
}

/**
 * lastChild - should return null for no children
 */
module.exports['lastChild - should return null for no children'] = function(test) {
  var doc = $$.parseFromString("<doc></doc>");
  test.strictEqual(doc.documentElement.lastChild, null);
  test.done();
}

/**
 * parentNode - should return the parent node
 */
module.exports['parentNode - should return the parent node'] = function(test) {
  var doc = $$.parseFromString("<doc><a/><b/><c/></doc>");
  var c = doc.documentElement.lastChild;
  test.ok(c.parentNode);
  test.strictEqual(c.parentNode.nodeType, 1) // Node.ELEMENT_NODE
  test.strictEqual(c.parentNode.nodeName, "doc");
  test.done();
}

/**
 * parentNode - should return null for the document
 */
module.exports['parentNode - should return null for the document'] = function(test) {
  var doc = $$.parseFromString("<doc></doc>");
  test.strictEqual(doc.parentNode, null);
  test.done();
}
