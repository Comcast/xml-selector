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
var $$ = require('../../index')
;


/**
 * getAttribute - should return the attribute value
 */
module.exports['getAttribute - should return the attribute value'] = function(test) {
  var doc = $$.parseFromString("<doc foo=\"bar\" a=\"alpha\"></doc>");
  test.strictEqual(doc.documentElement.getAttribute("foo"), "bar");
  test.strictEqual(doc.documentElement.getAttribute("a"), "alpha");
  test.done();
}

/**
 * getAttribute - should return an empty string for no value
 */
module.exports['getAttribute - should return an empty string for no value'] = function(test) {
  var doc = $$.parseFromString("<doc></doc>");
  test.strictEqual(doc.documentElement.getAttribute("foo"), "");
  test.strictEqual(doc.documentElement.getAttribute(), "");
  test.done();
}

/**
 * tagName - should return the tag name for an element
 */
module.exports['tagName - should return the tag name for an element'] = function(test) {
  var doc = $$.parseFromString("<doc></doc>");
  test.strictEqual(doc.documentElement.tagName, "doc");
  test.done();
}
