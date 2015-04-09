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
