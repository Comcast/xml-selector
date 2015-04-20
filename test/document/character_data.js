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
 * data - should return the character data
 */
module.exports['data - should return the character data'] = function(test) {
  var doc = $$.parseFromString("<doc>Hello</doc>");
  test.strictEqual(doc.documentElement.firstChild.data, "Hello");
  test.done();
}

/**
 * length - should return the length of the character data
 */
module.exports['length - should return the length of the character data'] = function(test) {
  var doc = $$.parseFromString("<doc>Hello</doc>");
  test.strictEqual(doc.documentElement.firstChild.length, 5);
  test.done();
}

/**
 * length - should return the character count for multi-byte encodings
 */
module.exports['length - should return the character count for multi-byte encodings'] = function(test) {
  var doc = $$.parseFromString("<doc>H\u1EBDllo</doc>");
  test.strictEqual(doc.documentElement.firstChild.length, 5);
  test.done();
}
