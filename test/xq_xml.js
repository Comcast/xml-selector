/**
 * Test xml function
 */

var xQ = require('../index')

/**
 * Test empty document
 */
module.exports.testEmpty = function(test) {
  var empty = new xQ();

  test.strictEqual(empty.xml(), "");
  
  test.done();
}

/**
 * Test single element document
 */
module.exports.testSingleElem = function(test) {
  var q = new xQ("<doc>Hello world!</doc>");

  test.strictEqual(q.xml(), "<?xml version=\"1.0\"?>\n<doc>Hello world!</doc>\n");
  
  test.done();
}

/**
 * Test mulitple descendants
 */
module.exports.testMultiDescendants = function(test) {
  var q = new xQ("<p>The <i>quick</i> <b>brown <i>fox</i></b> jumps...</p>");

  test.strictEqual(q.xml(), "<?xml version=\"1.0\"?>\n<p>The <i>quick</i> <b>brown <i>fox</i></b> jumps...</p>\n");
  
  test.done();
}

/**
 * Test child node
 */
module.exports.testChildNode = function(test) {
  var q = new xQ("<p>The <i>quick</i> <b>brown <i>fox</i></b> jumps...</p>");

  test.strictEqual(q.find('b').xml(), "<b>brown <i>fox</i></b>");
  
  test.strictEqual(q.find('i').xml(), "<i>quick</i>");

  test.done();
}
