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
 * Unit tests for the reduce() method
 */

var $$ = require('../index');

/**
 * Test empty set
 */
module.exports.testEmpty = function(test) {
  test.strictEqual($$().reduce(function(memo, n) { return memo+1; }, 0), 0);
  
  test.done();
}

/**
 * Test success
 */
module.exports.testSuccess = function(test) {
  var $doc = $$('<doc><item>2</item><item>4</item><item>6</item></doc>').find('item');
  
  test.strictEqual($doc.reduce(function(memo, elem) {
    return memo + parseInt(elem.text());
  }, 0), 12);
  
  var $doc = $$('<doc><item>he</item><item>llo</item><item> world</item></doc>').find('item');
  
  test.strictEqual($doc.reduce(function(memo, elem) {
    return memo + elem.text();
  }, ""), "hello world");

  test.done();
}

/**
 * Test context
 */
module.exports.testContext = function(test) {
  var out = [];
  var ctx = {"context": 1};
  
  test.strictEqual($$('<doc/>').reduce(function(memo, n) {
    out.push(this);
    return memo + 1;
  }, 0, ctx), 1);
  
  test.deepEqual(out, [ctx]);
  
  test.done();
}

/**
 * Test args
 */
module.exports.testArgs = function(test) {
  var $doc = $$('<basket><fruit name="apple"/></basket>').find('fruit');
  
  test.strictEqual($doc.reduce(function(memo, elem, idx, list) {

    test.strictEqual(memo, "memo");
    test.strictEqual(elem, $doc[0]);
    test.strictEqual(idx, 0);
    test.strictEqual(list, $doc);
    
    return false;
    
  }, "memo"), false);
  
  test.done();
}
