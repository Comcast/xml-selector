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
 * Test xml function
 */

var xQ = require('../index')

/**
 * Test empty document
 */
module.exports.testEmpty = function(test) {
  var empty = new xQ();

  test.strictEqual(empty.xml(), "");
  
  test.done();
}

/**
 * Test single element document
 */
module.exports.testSingleElem = function(test) {
  var q = new xQ("<doc>Hello world!</doc>");

  test.strictEqual(q.xml(), "<?xml version=\"1.0\"?>\n<doc>Hello world!</doc>\n");
  
  test.done();
}

/**
 * Test mulitple descendants
 */
module.exports.testMultiDescendants = function(test) {
  var q = new xQ("<p>The <i>quick</i> <b>brown <i>fox</i></b> jumps...</p>");

  test.strictEqual(q.xml(), "<?xml version=\"1.0\"?>\n<p>The <i>quick</i> <b>brown <i>fox</i></b> jumps...</p>\n");
  
  test.done();
}

/**
 * Test child node
 */
module.exports.testChildNode = function(test) {
  var q = new xQ("<p>The <i>quick</i> <b>brown <i>fox</i></b> jumps...</p>");

  test.strictEqual(q.find('b').xml(), "<b>brown <i>fox</i></b>");
  
  test.strictEqual(q.find('i').xml(), "<i>quick</i>");

  test.done();
}

/**
 * Test node without children
 */
module.exports.testChildlessNode = function(test) {
  var q = new xQ("<doc><hello /></doc>");

  test.strictEqual(q.find('hello').xml(), "<hello/>");
  
  test.done();
}

