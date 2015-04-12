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
 * Unit tests for the find() method
 */

var xQ = require('../index');
var $$ = xQ;

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
  
  test.deepEqual(all.map(function(n) { return n.getAttribute('value'); }), ['Apple', 'Orange', 'Banana']);

  test.deepEqual(child.map(function(n) { return n.getAttribute('value'); }), ['Apple']);

  test.done();
}

/**
 * Test the + combinator
 */
module.exports.testSiblingCombinator = function(test) {
  var siblings = new xQ('<doc><item value="Kiwi" /><item value="Orange" /><item value="Grape" /></doc>');

  var orange = siblings.find('item[value="Kiwi"] + item');
  
  test.deepEqual(orange.map(function(n) { return n.getAttribute('value'); }), ['Orange']);
  
  test.done();
}

/**
 * Test *
 */
module.exports.testUniversalSelector = function(test) {
  var hello = new xQ("<doc><hello /></doc>");

  test.deepEqual(hello.find('*').map(function(n) { return n.nodeName; }), ['doc','hello']);
  
  var items = new xQ("<doc><items><item>1</item><item>2</item><item>3</item></items></doc>");
  
  test.deepEqual(items.find('doc *').map(function(n) { return n.nodeName + ' (' + $$(n).text() + ')'; }),
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

/**
 * Test higher-order match
 */
module.exports.testHigherOrderEmpty = function(test) {

  test.strictEqual(
    $$().find(function(n) { return n.getAttribute('value') == 'Apple'; }),
    undefined
  );

  test.done();
}

/**
 * Test higher-order match
 */
module.exports.testHigherOrderMatch = function(test) {
  var $set = $$('<doc>' +
    '<item value="Apple">Manzana</item>' +
    '<item value="Orange">Naranja</item>' +
    '<item value="Banana">Plátano</item>' +
  '</doc>').find('item');
  
  test.strictEqual(
    $$($set.find(function(n) { return n.getAttribute('value') == 'Orange'; })).text(),
    'Naranja'
  );

  test.done();
}

/**
 * Test higher-order match
 */
module.exports.testHigherOrderNoMatch = function(test) {
  var $set = $$('<doc>' +
    '<item value="Apple">Manzana</item>' +
    '<item value="Orange">Naranja</item>' +
    '<item value="Banana">Plátano</item>' +
  '</doc>').find('item');
  
  test.strictEqual(
    $set.find(function(n) { return n.getAttribute('value') == 'Kumquat'; }),
    undefined
  );

  test.done();
}

/**
 * Test higher-order match
 */
module.exports.testHigherOrderContext = function(test) {
  var out = [];
  var ctx = {"context": 1};
  
  var $set = $$('<doc>' +
    '<item value="Apple">Manzana</item>' +
    '<item value="Orange">Naranja</item>' +
    '<item value="Banana">Plátano</item>' +
  '</doc>').find('item');
  
  test.strictEqual(
    $set.find(function(n) { out.push(this); return false; }, ctx),
    undefined
  );
  
  test.deepEqual(out, [ctx, ctx, ctx]);

  test.done();
}
