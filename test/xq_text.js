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
 * Test the text() function
 */

var xQ = require('../index');

/**
 * Test an empty set
 */
module.exports.testEmpty = function(test) {
  var empty = new xQ();
  
  test.strictEqual(empty.text(), "");
  
  test.done();
}

/**
 * Test a single element
 */
module.exports.testSingle = function(test) {
  var q = new xQ("<doc>Hello world!</doc>");
  
  test.strictEqual(q.text(), "Hello world!");
  
  test.done();
}

/**
 * Test multiple desdendants
 */
module.exports.testMultipleDescendants = function(test) {
  var q = new xQ("<p>The <i>quick</i> <b>brown <i>fox</i></b> jumps...</p>");
  
  test.strictEqual(q.text(), "The quick brown fox jumps...");
  
  test.done();
}
