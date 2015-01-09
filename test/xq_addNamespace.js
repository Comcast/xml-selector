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
 * Test addNamespace function
 */

var xQ = require('../index')

function allXml(q) { return q.map(function(n) { return xQ(n).xml(); }); }

/**
 * Utility to catch an exception and return its message
 */
function exceptionMessage(block) {
  try {
    block();
  } catch (e) {
    return e.message;
  }
}

/**
 * Test addNamespace
 */
module.exports.testAddNamespace = function(test) {
  var q = new xQ("<doc xmlns=\"http://csv.comcast.com/A\" xmlns:nsb=\"http://csv.comcast.com/B\"><item>A</item><nsb:item>B</nsb:item></doc>");
  
  q.addNamespace("a", "http://csv.comcast.com/A");
  q.addNamespace("b", "http://csv.comcast.com/B");

  test.deepEqual(allXml(q.find('item')), ['<item>A</item>', '<nsb:item>B</nsb:item>']);
  test.deepEqual(allXml(q.find('a:item')), ['<item>A</item>']);
  test.deepEqual(allXml(q.find('b:item')), ['<nsb:item>B</nsb:item>']);
  test.deepEqual(allXml(q.find('item').not('a:item')), ['<nsb:item>B</nsb:item>']);

  test.done();
}

/**
 * Test invalid namespace prefix
 */
module.exports.testInvalidPrefix = function(test) {
  var q = new xQ("<doc xmlns=\"http://csv.comcast.com/A\" xmlns:nsb=\"http://csv.comcast.com/B\"><item>A</item><nsb:item>B</nsb:item></doc>");
  
  q.addNamespace("a", "http://csv.comcast.com/A");
  q.addNamespace("b", "http://csv.comcast.com/B");

  test.strictEqual(exceptionMessage(function() { q.find('c:item'); }), "Unknown namespace prefix");
  
  test.done();
}
