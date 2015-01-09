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
 * Test the attr() function
 */

var xQ = require('../index')

/**
 * Test attr call on an empty set
 */
module.exports.testEmpty = function(test) {
  var empty = new xQ();

  test.strictEqual(empty.attr("name"), undefined);
  
  test.done();
}

/**
 * Test attr call on a document
 */
module.exports.testDocument = function(test) {
  var q = new xQ('<doc><people><person name="Fred" /></people></doc>');
  
  test.strictEqual(q.attr('name'), undefined);
  
  test.done();
}

/**
 * Test attr call on a single element
 */
module.exports.testSingle = function(test) {
  var q = new xQ('<doc><people><person name="Fred" /></people></doc>');
  
  test.strictEqual(q.find('person').attr('name'), 'Fred');
  test.strictEqual(q.find('person').attr('age'), undefined);
  
  test.done();
}

/**
 * Test attr call on multiple elements
 */
module.exports.testMultiple = function(test) {
  var q = new xQ('<doc><people><person name="Sally" /><person name="Susan" /></people></doc>');
  
  test.strictEqual(q.find('person').attr('name'), 'Sally');
  
  test.done();
}
