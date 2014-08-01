/**
 * Test next/nextAll/nextUntil functions
 */

var xQ = require('../index')

function allXml(q) { return q.map(function(n) { return xQ(n).xml(); }); }

/**
 * Test empty set
 */
module.exports.testEmpty = function(test) {
  var empty = new xQ();

  test.strictEqual(empty.next().length, 0);
  test.strictEqual(empty.next('empty').length, 0);
  test.strictEqual(empty.nextAll().length, 0);
  test.strictEqual(empty.nextAll('empty').length, 0);
  test.strictEqual(empty.nextUntil('empty').length, 0);
  
  test.done();
}

/**
 * Test element selector
 */
module.exports.testElementSelector = function(test) {
  var q = new xQ("<doc><items><number>1</number><number>2</number><string>foo</string><number>3</number></items></doc>");

  test.deepEqual(allXml(q.find('number').next()), ['<number>2</number>','<string>foo</string>']);
  test.deepEqual(allXml(q.find('number').next('string')), ['<string>foo</string>']);
  test.deepEqual(allXml(q.find('number').next('foo')), []);
  test.deepEqual(allXml(q.find('number').nextAll()),
    ['<number>2</number>','<string>foo</string>','<number>3</number>','<string>foo</string>','<number>3</number>']);
  test.deepEqual(allXml(q.find('number').nextAll('string')), ['<string>foo</string>','<string>foo</string>']);
  test.deepEqual(allXml(q.find('number').nextAll('foo')), []);

  test.done();
}

/**
 * Test attribute selector
 */
module.exports.testAttributeSelector = function(test) {
  var q = new xQ('<doc><attrs><attr name="fruit"><value>Apple</value></attr><attr name="color"><value>Red</value></attr><attr name="number"><value>Seven</value></attr></attrs></doc>');

  test.deepEqual(allXml(q.find('attr[name="fruit"]').next('attr[name="color"]')), ['<attr name="color"><value>Red</value></attr>']);
  test.deepEqual(allXml(q.find('attr[name="fruit"]').next('attr[name="number"]')), []);
  test.deepEqual(allXml(q.find('attr[name="fruit"]').next('attr[name="fruit"]')), []);
  test.deepEqual(allXml(q.find('attr[name="fruit"]').nextAll('attr[name="color"]')), ['<attr name="color"><value>Red</value></attr>']);
  test.deepEqual(allXml(q.find('attr[name="fruit"]').nextAll('attr[name="number"]')), ['<attr name="number"><value>Seven</value></attr>']);
  test.deepEqual(allXml(q.find('attr[name="fruit"]').nextAll('attr[name="fruit"]')), []);
  test.deepEqual(allXml(q.find('attr[name="fruit"]').nextUntil('attr[name="color"]')), []);
  test.deepEqual(allXml(q.find('attr[name="fruit"]').nextUntil('attr[name="number"]')), ['<attr name="color"><value>Red</value></attr>']);
  test.deepEqual(allXml(q.find('attr[name="fruit"]').nextUntil('attr[name="fruit"]')),
    ['<attr name="color"><value>Red</value></attr>','<attr name="number"><value>Seven</value></attr>']);

  test.done();
}
