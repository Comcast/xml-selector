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
 * Test array access and iteration
 */

var xQ = require('../index');

/**
 * Test array index access
 */
module.exports.testIndex = function(test) {
  var empty = new xQ();
  
  test.strictEqual(empty[0], undefined);
  
  var person = new xQ('<doc><people><person name="Fred" /><person name="Sally" /><person name="Jane" /></people></doc>');
  
  test.strictEqual(person[10], undefined);
  
  var people = person.find('person');
  test.strictEqual(people[0].getAttribute('name'), 'Fred');
  test.strictEqual(people[1].getAttribute('name'), 'Sally');
  test.strictEqual(people[2].getAttribute('name'), 'Jane');
  test.strictEqual(people[3], undefined);
  
  test.strictEqual(person.find('missing')[0], undefined);
  
  test.done();
}

/**
 * Test forEach
 */
module.exports.testForEach = function(test) {
  var out = [];
  var i = 0;
  
  var empty = new xQ();
  
  empty.forEach(function(n, idx) { out.push([n.getAttribute('name'), idx]); });
  
  test.ok(empty.forEach());
  
  var person = new xQ('<doc><people><person name="Fred" /><person name="Sally" /><person name="Jane" /></people></doc>');
  
  var people = person.find('person');
  people.forEach(function(n, idx) { out.push([n.getAttribute('name'), idx]); });
  
  test.throws(function() { people.forEach(function() { throw new Error("oops!"); }) });

  var specialThis = {"special":true};
  people.forEach(function(n, idx) { test.strictEqual(this, specialThis); }, specialThis);
  
  var missing = person.find('missing');
  missing.forEach(function(n, idx) { out.push([n.getAttribute('name'), idx]); });
  
  test.deepEqual(out, [['Fred', 0], ['Sally', 1], ['Jane', 2]]);
  
  test.done();
}
