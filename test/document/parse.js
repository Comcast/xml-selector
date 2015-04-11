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
 * parseFromString - should create a document
 */
module.exports['parseFromString - should create a document'] = function(test) {
  var doc = $$.parseFromString("<doc></doc>");
  test.strictEqual(doc.constructor.name, 'Document');
  test.done();
}

/**
 * parseFromString - should thrown an exception for invalid XML
 */
module.exports['parseFromString - should thrown an exception for invalid XML'] = function(test) {
  test.throws(function() { var doc = $$.parseFromString("<doc"); });
  test.done();
}

/**
 * parseFromString exception - should contain the error details
 */
module.exports['parseFromString exception - should contain the error details'] = function(test) {
  try {
    $$.parseFromString("<doc");
  } catch(e) {
    test.ok(/parser error/.test(e.message));
    test.ok(/line 1/.test(e.message));
    test.ok(/Couldn't find end of Start Tag/.test(e.message));
    test.ok(/<doc/.test(e.message));
    test.done();
  }
}
