/**
 * Unit tests for the find() method
 */

var xQ = require('../index');

/**
 * Test simple selector
 */
module.exports.testSimpleSelector = function(test) {
  var empty = new xQ();
  test.strictEqual(empty.find('hello').length, 0);
  
  var hello = new xQ("<doc><hello /></doc>");
  test.strictEqual(hello.find('hello').length, 1);
  
  var items = new xQ("<doc><items><item>1</item><item>2</item><item>3</item></items></doc>");
  test.strictEqual(items.find('item').length, 3);

  test.done();
}

/**
 * Test multiple selectors
 */
module.exports.testMultipleSelector = function(test) {
  var pets = new xQ("<doc><pets><cat>Fluffy</cat><dog>Fido</dog></pets><pets><cat>Mr. Whiskers</cat></pets><cat>Scratchy</cat></doc>");

  test.strictEqual(pets.find('pets cat').length, 2);

  test.done();
}

/**
 * Test the attribute selector
 */
module.exports.testAttrSelector = function(test) {
  var attrEquals =
    new xQ('<doc><attrs><attr name="fruit"><value>Apple</value></attr><attr name="color"><value>Red</value></attr></attrs></doc>');
  
  var result = attrEquals.find('attr[name="color"] value');
  
  test.strictEqual(result.length, 1);
  test.strictEqual(result.text(), 'Red');

  test.done();
}

/**
 * Test the > combinator
 */
module.exports.testImmediateCombinator = function(test) {
  var immediate = new xQ('<doc><item value="Apple" /><list><item value="Orange" /><item value="Banana" /></list></doc>');
  
  var all = immediate.find('item');
  var child = immediate.find('doc > item');
  
  test.deepEqual(all.map(function(n) { return n._attr('value').value(); }), ['Apple', 'Orange', 'Banana']);

  test.deepEqual(child.map(function(n) { return n._attr('value').value(); }), ['Apple']);

  test.done();
}

/**
 * Test the + combinator
 */
module.exports.testSiblingCombinator = function(test) {
  var siblings = new xQ('<doc><item value="Kiwi" /><item value="Orange" /><item value="Grape" /></doc>');

  var orange = siblings.find('item[value="Kiwi"] + item');
  
  test.deepEqual(orange.map(function(n) { return n._attr('value').value(); }), ['Orange']);
  
  test.done();
}

/**
 * Test *
 */
module.exports.testUniversalSelector = function(test) {
  var hello = new xQ("<doc><hello /></doc>");

  test.deepEqual(hello.find('*').map(function(n) { return n.name(); }), ['doc','hello']);
  
  var items = new xQ("<doc><items><item>1</item><item>2</item><item>3</item></items></doc>");
  
  test.deepEqual(items.find('doc *').map(function(n) { return n.name() + ' (' + n.text() + ')'; }),
    ['items (123)', 'item (1)', 'item (2)', 'item (3)']);
  
  test.done();
}

/**
 * Test unmatched selectors
 */
module.exports.testUnmatchedSelector = function(test) {
  var hello = new xQ("<doc><hello /></doc>");

  test.strictEqual(hello.find('hi').length, 0);
  test.strictEqual(hello.find('doc hi').length, 0);
  test.strictEqual(hello.find('doc > hi').length, 0);
  test.strictEqual(hello.find('> hi').length, 0);
  test.strictEqual(hello.find('doc + hi').length, 0);
  test.strictEqual(hello.find('+ hi').length, 0);
  test.strictEqual(hello.find('doc hello *').length, 0);
  
  test.done();
}
