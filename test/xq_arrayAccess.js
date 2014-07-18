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
  test.strictEqual(people[0]._attr('name').value(), 'Fred');
  test.strictEqual(people[1]._attr('name').value(), 'Sally');
  test.strictEqual(people[2]._attr('name').value(), 'Jane');
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
  
  empty.forEach(function(n, idx) { out.push([n._attr('name').value(), idx]); });
  
  test.ok(empty.forEach());
  
  var person = new xQ('<doc><people><person name="Fred" /><person name="Sally" /><person name="Jane" /></people></doc>');
  
  var people = person.find('person');
  people.forEach(function(n, idx) { out.push([n._attr('name').value(), idx]); });
  
  test.throws(function() { people.forEach(function() { throw new Error("oops!"); }) });

  var specialThis = {"special":true};
  people.forEach(function(n, idx) { test.strictEqual(this, specialThis); }, specialThis);
  
  var missing = person.find('missing');
  missing.forEach(function(n, idx) { out.push([n._attr('name').value(), idx]); });
  
  test.deepEqual(out, [['Fred', 0], ['Sally', 1], ['Jane', 2]]);
  
  test.done();
}
