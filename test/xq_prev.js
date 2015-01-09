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
 * Test prev/prevAll/prevUntil functions
 */

var xQ = require('../index')

function allXml(q) { return q.map(function(n) { return xQ(n).xml(); }); }

/**
 * Test empty set
 */
module.exports.testEmpty = function(test) {
  var empty = new xQ();

  test.strictEqual(empty.prev().length, 0);
  test.strictEqual(empty.prev('empty').length, 0);
  test.strictEqual(empty.prevAll().length, 0);
  test.strictEqual(empty.prevAll('empty').length, 0);
  test.strictEqual(empty.prevUntil('empty').length, 0);
  
  test.done();
}

/**
 * Test element selector
 */
module.exports.testElementSelector = function(test) {
  var q = new xQ("<doc><items><number>1</number><number>2</number><string>foo</string><number>3</number></items></doc>");

  test.deepEqual(allXml(q.find('number').prev()), ['<number>1</number>','<string>foo</string>']);
  test.deepEqual(allXml(q.find('number').prev('string')), ['<string>foo</string>']);
  test.deepEqual(allXml(q.find('number').prev('foo')), []);
  test.deepEqual(allXml(q.find('number').prevAll()),
    ['<number>1</number>','<string>foo</string>','<number>2</number>','<number>1</number>']);
  test.deepEqual(allXml(q.find('number').prevAll('string')), ['<string>foo</string>']);
  test.deepEqual(allXml(q.find('number').prevAll('foo')), []);

  test.done();
}

/**
 * Test attribute selector
 */
module.exports.testAttributeSelector = function(test) {
  var q = new xQ('<doc><attrs><attr name="fruit"><value>Apple</value></attr><attr name="color"><value>Red</value></attr><attr name="number"><value>Seven</value></attr></attrs></doc>');

  test.deepEqual(allXml(q.find('attr[name="number"]').prev('attr[name="color"]')), ['<attr name="color"><value>Red</value></attr>']);
  test.deepEqual(allXml(q.find('attr[name="number"]').prev('attr[name="number"]')), []);
  test.deepEqual(allXml(q.find('attr[name="number"]').prev('attr[name="fruit"]')), []);
  test.deepEqual(allXml(q.find('attr[name="number"]').prevAll('attr[name="color"]')), ['<attr name="color"><value>Red</value></attr>']);
  test.deepEqual(allXml(q.find('attr[name="number"]').prevAll('attr[name="number"]')), []);
  test.deepEqual(allXml(q.find('attr[name="number"]').prevAll('attr[name="fruit"]')), ['<attr name="fruit"><value>Apple</value></attr>']);
  test.deepEqual(allXml(q.find('attr[name="number"]').prevUntil('attr[name="color"]')), []);
  test.deepEqual(allXml(q.find('attr[name="number"]').prevUntil('attr[name="number"]')),
    ['<attr name="color"><value>Red</value></attr>','<attr name="fruit"><value>Apple</value></attr>']);
  test.deepEqual(allXml(q.find('attr[name="number"]').prevUntil('attr[name="fruit"]')), ['<attr name="color"><value>Red</value></attr>']);

  test.done();
}
