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
