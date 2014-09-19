/**
 * Test not function
 */

var xQ = require('../index')

function allXml(q) { return q.map(function(n) { return xQ(n).xml(); }); }

/**
 * Test empty set
 */
module.exports.testEmpty = function(test) {
  var empty = new xQ();

  test.strictEqual(empty.not('empty').length, 0);
  
  test.done();
}

/**
 * Test element selector
 */
module.exports.testElementSelector = function(test) {
  var q = new xQ("<doc><items><number>1</number><number>2</number><string>foo</string><number>3</number></items></doc>");
  
  test.deepEqual(allXml(q.find('number').not('string')), ["<number>1</number>","<number>2</number>","<number>3</number>"]);
  test.deepEqual(allXml(q.find('number').not('number')), []);
  test.deepEqual(allXml(q.find('items *').not('number')), ["<string>foo</string>"]);
  
  test.done();
}

/**
 * Test attribute selector
 */
module.exports.testAttrSelector = function(test) {
  var q = new xQ('<doc><attrs><attr name="fruit"><value>Apple</value></attr><attr name="color"><value>Red</value></attr><attr name="number"><value>Seven</value></attr></attrs></doc>');
  
  test.deepEqual(allXml(q.find('attr').not('attr[name="color"]')),
    ['<attr name="fruit"><value>Apple</value></attr>', '<attr name="number"><value>Seven</value></attr>']);
    
  test.deepEqual(allXml(q.find('attr').not('attr[name="number"]')),
    ['<attr name="fruit"><value>Apple</value></attr>', '<attr name="color"><value>Red</value></attr>']);
  
  test.deepEqual(allXml(q.find('attr').not('attr[name="shape"]')), [
    '<attr name="fruit"><value>Apple</value></attr>',
    '<attr name="color"><value>Red</value></attr>',
    '<attr name="number"><value>Seven</value></attr>'
  ]);
  
  test.done();
}
