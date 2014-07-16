/**
 * Test that the xq extension loads
 */
exports.testAvailable = function(test) {
  var xq = require('../index');
  
  test.ok(xq);
  test.strictEqual(typeof xq, 'function');
  
  test.done();
}
