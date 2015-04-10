var $$ = require('../../index')
;


/**
 * documentElement - should return the document element
 */
module.exports['documentElement - should return the documentElement'] = function(test) {
  var doc = $$.parseFromString("<doc></doc>");
  test.ok(doc.documentElement);
  test.strictEqual(doc.documentElement.nodeType, 1); // Node.ELEMENT_NODE
  test.strictEqual(doc.documentElement.nodeName, "doc");
  test.done();
}
