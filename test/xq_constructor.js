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
 * Constructor tests
 */

var xQ = require('../index');

/**
 * Test the empty constructor
 */
module.exports.testEmpty = function(test) {
  var empty = new xQ();
  
  test.ok(empty);
  
  test.strictEqual(empty.length, 0);
  
  test.done();
}

/**
 * Test the empty constructor without new
 */
module.exports.testEmptySansNew = function(test) {
  var empty = xQ();
  
  test.ok(empty);
  
  test.strictEqual(empty.length, 0);
  
  test.done();
}

/**
 * Test the string constructor
 */
module.exports.testString = function(test) {
  var q = new xQ("<doc><hello /></doc>");
  
  test.ok(q);
  test.strictEqual(q.length, 1);
  
  test.done();
}

/**
 * Test the string constructor without new
 */
module.exports.testStringSansNew = function(test) {
  var q = xQ("<doc><hello /></doc>");
  
  test.ok(q);
  test.strictEqual(q.length, 1);
  
  test.done();
}

/**
 * Test invalid constructor calls
 */
module.exports.testInvalid = function(test) {
  test.throws(function() {
    new xQ("something's not right");
  });
  
  test.throws(function() {
    new xQ(5);
  });
  
  test.done();
}

/**
 * Test invalid constructor calls without new
 */
module.exports.testInvalidSansNew = function(test) {
  test.throws(function() {
    xQ("something's not right");
  });
  
  test.throws(function() {
    xQ(5);
  });
  
  test.done();
}

/**
 * Constructor from a document
 */
module.exports.testDocument = function(test) {
  var xml = "<doc><items><item>Apple</item><item>Orange</item><item>Banana</item></items><more><item>Salad</item></more></doc>";
  var doc = xQ.parseFromString(xml);

  var q = new xQ(doc);
  
  test.ok(q);
  test.strictEqual(q.find('item').length, 4);

  test.done();
}

/**
 * Constructor from a document without new
 */
module.exports.testDocumentSansNew = function(test) {
  var xml = "<doc><items><item>Apple</item><item>Orange</item><item>Banana</item></items><more><item>Salad</item></more></doc>";
  var doc = xQ.parseFromString(xml);

  var q = xQ(doc);
  
  test.ok(q);
  test.strictEqual(q.find('item').length, 4);

  test.done();
}

/**
 * Constructor for a single xml node
 */
module.exports.testSingleNode = function(test) {
  var xml = "<doc><items><item>Apple</item><item>Orange</item><item>Banana</item></items><more><item>Salad</item></more></doc>";
  var doc = xQ.parseFromString(xml);
  
  var q = new xQ(doc.documentElement.firstChild);
  
  test.ok(q);
  test.strictEqual(q.find('item').length, 3);
  test.deepEqual(q.find('item').map(function(n) { return xQ(n).text(); }), ["Apple", "Orange", "Banana"]);

  test.done();
}

/**
 * Constructor for a single xml node without new
 */
module.exports.testSingleNodeSansNew = function(test) {
  var xml = "<doc><items><item>Apple</item><item>Orange</item><item>Banana</item></items><more><item>Salad</item></more></doc>";
  var doc = xQ.parseFromString(xml);
  
  var q = xQ(doc.documentElement.firstChild);
  
  test.ok(q);
  test.strictEqual(q.find('item').length, 3);
  test.deepEqual(q.find('item').map(function(n) { return xQ(n).text(); }), ["Apple", "Orange", "Banana"]);

  test.done();
}

/**
 * Constructor for multiple xml nodes
 */
module.exports.testMultiNode = function(test) {
  var xml = "<doc><items><item>Apple</item><item>Orange</item><item>Banana</item></items><more><item>Salad</item></more></doc>";
  var doc = xQ.parseFromString(xml);

  var a = doc.documentElement.firstChild;
  var b = a.nextSibling;
  var q = new xQ(a, b);
  
  test.ok(q);
  test.strictEqual(q.length, 2);
  test.strictEqual(q.find('item').length, 4);
  test.deepEqual(q.find('item').map(function(n) { return xQ(n).text(); }), ["Apple", "Orange", "Banana", "Salad"]);

  test.done();
}

/**
 * Constructor for multiple xml nodes without new
 */
module.exports.testMultiNodeSansNew = function(test) {
  var xml = "<doc><items><item>Apple</item><item>Orange</item><item>Banana</item></items><more><item>Salad</item></more></doc>";
  var doc = xQ.parseFromString(xml);

  var a = doc.documentElement.firstChild;
  var b = a.nextSibling;
  var q = xQ(a, b);
  
  test.ok(q);
  test.strictEqual(q.length, 2);
  test.strictEqual(q.find('item').length, 4);
  test.deepEqual(q.find('item').map(function(n) { return xQ(n).text(); }), ["Apple", "Orange", "Banana", "Salad"]);

  test.done();
}

/**
 * Constructor for multiple xml nodes in an array
 */
module.exports.testArray = function(test) {
  var xml = "<doc><items><item>Apple</item><item>Orange</item><item>Banana</item></items><more><item>Salad</item></more></doc>";
  var doc = xQ.parseFromString(xml);

  var a = doc.documentElement.firstChild;
  var b = a.nextSibling;
  var q = new xQ([a, b]);
  
  test.ok(q);
  test.strictEqual(q.length, 2);
  test.strictEqual(q.find('item').length, 4);
  test.deepEqual(q.find('item').map(function(n) { return xQ(n).text(); }), ["Apple", "Orange", "Banana", "Salad"]);

  test.done();
}

/**
 * Constructor for multiple xml nodes in an array without new
 */
module.exports.testArraySansNew = function(test) {
  var xml = "<doc><items><item>Apple</item><item>Orange</item><item>Banana</item></items><more><item>Salad</item></more></doc>";
  var doc = xQ.parseFromString(xml);

  var a = doc.documentElement.firstChild;
  var b = a.nextSibling;
  var q = xQ([a, b]);
  
  test.ok(q);
  test.strictEqual(q.length, 2);
  test.strictEqual(q.find('item').length, 4);
  test.deepEqual(q.find('item').map(function(n) { return xQ(n).text(); }), ["Apple", "Orange", "Banana", "Salad"]);

  test.done();
}
