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
 * Unit tests for the findIndex() method
 */

var $$ = require('../index');

/**
 * Test empty set
 */
module.exports.testEmpty = function(test) {
  var $doc = $$();
  
  test.strictEqual($doc.findIndex(function() { return true; }), -1);
  
  test.done();
}

/**
 * Test no match
 */
module.exports.testNoMatch = function(test) {
  var $doc = $$('<basket><fruit name="apple"/><fruit name="pear"/><fruit name="orange"/></basket>').find('fruit');
  
  test.strictEqual($doc.findIndex(function(elem) {
    return $$(elem).attr('name') == 'banana';
  }), -1);
  
  test.done();
}

/**
 * Test first match
 */
module.exports.testFirstMatch = function(test) {
  var $doc = $$('<basket><fruit name="apple"/><fruit name="pear"/><fruit name="orange"/></basket>').find('fruit');
  
  test.strictEqual($doc.findIndex(function(elem) {
    return $$(elem).attr('name') == 'pear';
  }), 1);
  
  test.done();
}
