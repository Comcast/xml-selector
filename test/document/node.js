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
