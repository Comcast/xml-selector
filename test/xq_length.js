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
 * Test the length property
 */

var xQ = require('../index')

/**
 * Test length an empty set
 */
module.exports.testEmpty = function(test) {
  var empty = new xQ();

  test.strictEqual(empty.length, 0);
  
  test.done();
}

/**
 * Test length on a single node
 */
module.exports.testSingle = function(test) {
  var q = new xQ('<doc><people><person name="Fred" /><person name="Sally" /><person name="Jane" /></people></doc>');

  test.strictEqual(q.length, 1);
  
  test.done();
}

/**
 * Test length on a set of multiple nodes
 */
module.exports.testMultiple = function(test) {
  var q = new xQ('<doc><people><person name="Fred" /><person name="Sally" /><person name="Jane" /></people></doc>');

  test.strictEqual(q.find('person').length, 3);
  
  test.done();
}

/**
 * Test length on an empty set created from a search result
 */
module.exports.testEmptyResult = function(test) {
  var q = new xQ('<doc><people><person name="Fred" /><person name="Sally" /><person name="Jane" /></people></doc>');

  test.strictEqual(q.find('missing').length, 0);
  
  test.done();
}

/**
 * Test setting length (not allowed)
 */
module.exports.testSet = function(test) {
  var q = new xQ('<doc><people><person name="Fred" /><person name="Sally" /><person name="Jane" /></people></doc>');

  test.strictEqual(q.length, 1);
  
  q.length = 5;
  
  test.strictEqual(q.length, 1);

  test.done();
}
