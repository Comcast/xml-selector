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
 * Test invalid selectors
 */

var xQ = require('../index')

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
 * Test unterminated quotes
 */
module.exports.testUnterminatedQuote = function(test) {
  var empty = new xQ();

  test.strictEqual(exceptionMessage(function() { empty.find("'string value"); }), "Unterminated string in selector");
  
  test.strictEqual(exceptionMessage(function() { empty.find("\"string value"); }), "Unterminated string in selector");
  
  test.done();
}

/**
 * Test bad token sequence
 */
module.exports.testBadToken = function(test) {
  var empty = new xQ();

  test.strictEqual(exceptionMessage(function() { empty.find(">> bad child"); }), "Invalid selector");
  
  test.strictEqual(exceptionMessage(function() { empty.find("elem +"); }), "Invalid selector");
  
  test.done();
}

/**
 * Test attribute-related errors
 */
module.exports.testAttributes = function(test) {
  var empty = new xQ();

  // no selector
  test.strictEqual(exceptionMessage(function() { empty.find("[attr=\"value\"]"); }), "Invalid selector");
  
  // missing [
  test.strictEqual(exceptionMessage(function() { empty.find("elem attr=\"value\"]"); }), "Invalid selector");
  
  // missing =
  test.strictEqual(exceptionMessage(function() { empty.find("elem[attr\"value\"]"); }), "Invalid selector");
  
  // missing string
  test.strictEqual(exceptionMessage(function() { empty.find("elem[attr=]"); }), "Invalid selector");
  
  // missing ]
  test.strictEqual(exceptionMessage(function() { empty.find("elem[attr=\"value\""); }), "Invalid selector");
  
  test.done();
}
