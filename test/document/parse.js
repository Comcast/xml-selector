var $$ = require('../../index')
;


/**
 * parseFromString - should create a document
 */
module.exports['parseFromString - should create a document'] = function(test) {
  var doc = $$.parseFromString("<doc></doc>");
  test.strictEqual(doc.constructor.name, 'Document');
  test.done();
}
