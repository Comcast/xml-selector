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

/**
 * parseFromString - should thrown an exception for invalid XML
 */
module.exports['parseFromString - should thrown an exception for invalid XML'] = function(test) {
  test.throws(function() { var doc = $$.parseFromString("<doc"); });
  test.done();
}

/**
 * parseFromString exception - should contain the error details
 */
module.exports['parseFromString exception - should contain the error details'] = function(test) {
  try {
    $$.parseFromString("<doc");
  } catch(e) {
    test.ok(/parser error/.test(e.message));
    test.ok(/line 1/.test(e.message));
    test.ok(/Couldn't find end of Start Tag/.test(e.message));
    test.ok(/<doc/.test(e.message));
    test.done();
  }
}
