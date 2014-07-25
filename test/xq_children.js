/**
 * Test children function
 */

var xQ = require('../index')

function allXml(q) { return q.map(function(n) { return xQ(n).xml(); }); }

/**
 * Test empty set
 */
module.exports.testEmpty = function(test) {
  var empty = new xQ();

  test.strictEqual(empty.children().length, 0);
  test.strictEqual(empty.children('empty').length, 0);
  
  test.done();
}

/**
 * Test single child
 */
module.exports.testSingleChild = function(test) {
  var q = new xQ("<doc><hello /></doc>");
  
  test.deepEqual(allXml(q.children()), ["<doc><hello/></doc>"]);
  test.deepEqual(allXml(q.children('doc')), ["<doc><hello/></doc>"]);
  test.deepEqual(allXml(q.children('*')), ["<doc><hello/></doc>"]);
  test.deepEqual(allXml(q.children('')), ["<doc><hello/></doc>"]);
  test.deepEqual(allXml(q.children('empty')), []);

  test.done();
}

/**
 * Test multiple children with element selectors
 */
module.exports.testElementSelectors = function(test) {
  var q = new xQ("<doc><items><number>1</number><number>2</number><string>foo</string><number>3</number></items></doc>");
  
  test.deepEqual(allXml(q.find('items').children()), ["<number>1</number>","<number>2</number>","<string>foo</string>","<number>3</number>"]);
  test.deepEqual(allXml(q.find('items').children('number')), ["<number>1</number>","<number>2</number>","<number>3</number>"]);
  test.deepEqual(allXml(q.find('items').children('items number')), []);

  test.done();
}

/**
 * Test multiple children with attribute selectors
 */
module.exports.testAttributeSelectors = function(test) {
  var q = new xQ('<doc><attrs><attr name="fruit"><value>Apple</value></attr><attr name="color"><value>Red</value></attr></attrs></doc>');
  
  test.deepEqual(allXml(q.find('attrs').children('attr[name="color"]')), ["<attr name=\"color\"><value>Red</value></attr>"]);
  test.deepEqual(allXml(q.find('attrs').children('attr[name="fruit"]')), ["<attr name=\"fruit\"><value>Apple</value></attr>"]);

  test.done();
}
