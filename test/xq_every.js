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
 * Unit tests for the every() method
 */

var $$ = require('../index');

/**
 * Test empty set
 */
module.exports.testEmpty = function(test) {
  var $doc = $$();
  
  test.strictEqual($doc.every(function() { return true; }), true);
  
  test.done();
}

/**
 * Test failure
 */
module.exports.testFailure = function(test) {
  var $doc = $$('<basket><fruit name="apple"/><fruit name="pear"/><fruit name="orange"/></basket>').find('fruit');
  
  test.strictEqual($doc.every(function(elem) {
    return $$(elem).attr('name') == 'apple';
  }), false);
  
  test.done();
}

/**
 * Test success
 */
module.exports.testSuccess = function(test) {
  var $doc = $$('<basket><fruit name="apple"/><fruit name="pear"/><fruit name="orange"/></basket>').find('fruit');
  
  test.strictEqual($doc.every(function(elem) {
    return $$(elem).attr('name') != 'banana';
  }), true);
  
  test.done();
}

/**
 * Test context
 */
module.exports.testContext = function(test) {
  var out = [];
  var ctx = {"context": 1};
  
  var $doc = $$('<basket><fruit name="apple"/><fruit name="pear"/><fruit name="orange"/></basket>').find('fruit');
  
  test.strictEqual($doc.every(function(elem) {
    out.push(this);
    return $$(elem).attr('name') != 'banana';
  }, ctx), true);
  
  test.deepEqual(out, [ctx, ctx, ctx]);
  
  test.done();
}

/**
 * Test args
 */
module.exports.testContext = function(test) {
  var $doc = $$('<basket><fruit name="apple"/></basket>').find('fruit');
  
  test.strictEqual($doc.every(function(elem, idx, list) {

    test.strictEqual(elem, $doc[0]);
    test.strictEqual(idx, 0);
    test.strictEqual(list, $doc);
    
    return false;
    
  }), false);
  
  test.done();
}
