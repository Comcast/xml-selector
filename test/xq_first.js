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
 * Test first function
 */

var xQ = require('../index')

/**
 * Test empty document
 */
module.exports.testEmpty = function(test) {
  var empty = new xQ();

  test.strictEqual(empty.first().xml(), "");
  
  test.done();
}

/**
 * Test single node in set
 */
module.exports.testSingle = function(test) {
  var q = new xQ("<doc>Hello world!</doc>");

  test.strictEqual(q.first().xml(), "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n<doc>Hello world!</doc>\n");
  
  test.done();
}

/**
 * Test multiple nodes in set
 */
module.exports.testMultiple = function(test) {
  var q = new xQ("<p>The <i>quick</i> <b>brown <i>fox</i></b> jumps...</p>");

  test.strictEqual(q.find('i').first().xml(), "<i>quick</i>");
  
  test.done();
}
