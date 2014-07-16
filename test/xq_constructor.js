/**
 * Constructor tests
 */

var xQ = require('../index');

module.exports.testEmptyConstructor = function(test) {
  var empty = new xQ();
  
  test.ok(empty);
  
  // TODO: add length property
  // test.strictEqual(empty.length, 0);
  
  test.done();
}
