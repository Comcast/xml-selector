/**
 * Test the text() function
 */

var xQ = require('../index');

/**
 * Test an empty set
 */
module.exports.testEmpty = function(test) {
  var empty = new xQ();
  
  test.strictEqual(empty.text(), "");
  
  test.done();
}

/**
 * Test a single element
 */
module.exports.testSingle = function(test) {
  var q = new xQ("<doc>Hello world!</doc>");
  
  test.strictEqual(q.text(), "Hello world!");
  
  test.done();
}

/**
 * Test multiple desdendants
 */
module.exports.testMultipleDescendants = function(test) {
  var q = new xQ("<p>The <i>quick</i> <b>brown <i>fox</i></b> jumps...</p>");
  
  test.strictEqual(q.text(), "The quick brown fox jumps...");
  
  test.done();
}
