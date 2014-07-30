/**
 * Test closest function
 */

var xQ = require('../index')

function allXml(q) { return q.map(function(n) { return xQ(n).xml(); }); }

/**
 * Test empty set
 */
module.exports.testEmpty = function(test) {
  var empty = new xQ();

  test.strictEqual(empty.closest('empty').length, 0);
  
  test.done();
}

/**
 * Test element selector
 */
module.exports.testElementSelector = function(test) {
  var q = new xQ("<doc><hello /></doc>");
  
  test.deepEqual(allXml(q.find('hello')), ["<hello/>"]);
  test.deepEqual(allXml(q.find('hello').closest('hello')), ["<hello/>"]);
  test.deepEqual(allXml(q.find('hello').closest('*')), ["<hello/>"]);
  test.deepEqual(allXml(q.find('hello').closest('empty')), []);
  test.deepEqual(allXml(q.find('hello').closest('doc hello')), []);
  test.deepEqual(allXml(q.find('hello').closest('doc')), ["<doc><hello/></doc>"]);

  test.done();
}

/**
 * Test attribute selector
 */
module.exports.testAttrSelector = function(test) {
  var q = new xQ('<doc><attrs><attr name="fruit"><value>Apple</value></attr><attr name="color"><value>Red</value></attr></attrs></doc>');
  
  test.deepEqual(allXml(q.find('value').closest('attr[name="color"]')), ['<attr name="color"><value>Red</value></attr>']);
  test.deepEqual(allXml(q.find('value').closest('attr[name="fruit"]')), ['<attr name="fruit"><value>Apple</value></attr>']);

  test.done();
}
