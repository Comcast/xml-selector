/**
 * Test last function
 */

var xQ = require('../index')

/**
 * Test empty document
 */
module.exports.testEmpty = function(test) {
  var empty = new xQ();

  test.strictEqual(empty.last().xml(), "");
  
  test.done();
}

/**
 * Test single node in set
 */
module.exports.testSingle = function(test) {
  var q = new xQ("<doc>Hello world!</doc>");

  test.strictEqual(q.last().xml(), "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n<doc>Hello world!</doc>\n");
  
  test.done();
}

/**
 * Test multiple nodes in set
 */
module.exports.testMultiple = function(test) {
  var q = new xQ("<p>The <i>quick</i> <b>brown <i>fox</i></b> jumps...</p>");

  test.strictEqual(q.find('i').last().xml(), "<i>fox</i>");
  
  test.done();
}