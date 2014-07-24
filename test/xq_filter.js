/**
 * Test filter function
 */

var xQ = require('../index')

/**
 * Test empty document
 */
module.exports.testEmpty = function(test) {
  var empty = new xQ();

  test.strictEqual(empty.filter().length, 0);
  
  test.done();
}

/**
 * Test filtering from the document
 */
module.exports.testDocument = function(test) {
  var q = new xQ("<doc><hello /></doc>");
  
  test.strictEqual(q.filter('').length, 1);
  test.strictEqual(q.filter('*').length, 1);
  
  test.done();
}

/**
 * Test filtering from an element
 */
module.exports.testElement = function(test) {
  var q = new xQ("<doc><hello /></doc>");
  
  test.strictEqual(q.find('hello').filter('hello').length, 1);
  test.strictEqual(q.find('hello').filter('hi').length, 0);
  
  test.done();
}

/**
 * Test name filter
 */
module.exports.testName = function(test) {
  var q = new xQ("<doc><items><number>1</number><number>2</number><string>foo</string><number>3</number></items></doc>");
  
  test.deepEqual(q.find('items *').filter('number').map(function(n) { return n.text(); }), ['1', '2', '3']);
  
  test.deepEqual(q.find('items').filter('items number').length, 0);

  test.done();
}

/**
 * Test attribute filtering
 */
module.exports.testAttribute = function(test) {
  var q = new xQ('<doc><attrs><attr name="fruit"><value>Apple</value></attr><attr name="color"><value>Red</value></attr></attrs></doc>');
  
  test.deepEqual(q.find('attr').filter('attr[name="color"]').map(function(n) { return n.text(); }), ['Red']);
  
  test.deepEqual(q.find('attr').filter('attr[name="fruit"]').map(function(n) { return n.text(); }), ['Apple']);

  test.done();
}
