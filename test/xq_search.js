/**
 * Copyright 2013-2015 Comcast Cable Communications Management, LLC
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

/**
 * Unit tests for the search() method
 */

var $$ = require('../index');

/**
 * Test simple selector
 */
module.exports.testSimpleSelector = function(test) {
  var empty = $$();
  test.strictEqual(empty.search('hello').length, 0);
  
  var hello = $$("<doc><hello /></doc>");
  test.strictEqual(hello.search('hello').length, 1);
  
  var items = $$("<doc><items><item>1</item><item>2</item><item>3</item></items></doc>");
  test.strictEqual(items.search('item').length, 3);

  test.done();
}

/**
 * Test multiple selectors
 */
module.exports.testMultipleSelector = function(test) {
  var pets = $$("<doc><pets><cat>Fluffy</cat><dog>Fido</dog></pets><pets><cat>Mr. Whiskers</cat></pets><cat>Scratchy</cat></doc>");

  test.strictEqual(pets.search('pets cat').length, 2);

  test.done();
}

/**
 * Test the attribute selector
 */
module.exports.testAttrSelector = function(test) {
  var attrEquals =
    $$('<doc><attrs><attr name="fruit"><value>Apple</value></attr><attr name="color"><value>Red</value></attr></attrs></doc>');
  
  var result = attrEquals.search('attr[name="color"] value');
  
  test.strictEqual(result.length, 1);
  test.strictEqual(result.text(), 'Red');

  test.done();
}

/**
 * Test the > combinator
 */
module.exports.testImmediateCombinator = function(test) {
  var immediate = $$('<doc><item value="Apple" /><list><item value="Orange" /><item value="Banana" /></list></doc>');
  
  var all = immediate.search('item');
  var child = immediate.search('doc > item');
  
  test.deepEqual(all.map(function(n) { return n._attr('value').value(); }), ['Apple', 'Orange', 'Banana']);

  test.deepEqual(child.map(function(n) { return n._attr('value').value(); }), ['Apple']);

  test.done();
}

/**
 * Test the + combinator
 */
module.exports.testSiblingCombinator = function(test) {
  var siblings = $$('<doc><item value="Kiwi" /><item value="Orange" /><item value="Grape" /></doc>');

  var orange = siblings.search('item[value="Kiwi"] + item');
  
  test.deepEqual(orange.map(function(n) { return n._attr('value').value(); }), ['Orange']);
  
  test.done();
}

/**
 * Test *
 */
module.exports.testUniversalSelector = function(test) {
  var hello = $$("<doc><hello /></doc>");

  test.deepEqual(hello.search('*').map(function(n) { return n.name(); }), ['doc','hello']);
  
  var items = $$("<doc><items><item>1</item><item>2</item><item>3</item></items></doc>");
  
  test.deepEqual(items.search('doc *').map(function(n) { return n.name() + ' (' + n.text() + ')'; }),
    ['items (123)', 'item (1)', 'item (2)', 'item (3)']);
  
  test.done();
}

/**
 * Test unmatched selectors
 */
module.exports.testUnmatchedSelector = function(test) {
  var hello = $$("<doc><hello /></doc>");

  test.strictEqual(hello.search('hi').length, 0);
  test.strictEqual(hello.search('doc hi').length, 0);
  test.strictEqual(hello.search('doc > hi').length, 0);
  test.strictEqual(hello.search('> hi').length, 0);
  test.strictEqual(hello.search('doc + hi').length, 0);
  test.strictEqual(hello.search('+ hi').length, 0);
  test.strictEqual(hello.search('doc hello *').length, 0);
  
  test.done();
}
