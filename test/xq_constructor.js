/**
 * Constructor tests
 */

var xQ = require('../index');

/**
 * Test the empty constructor
 */
module.exports.testEmpty = function(test) {
  var empty = new xQ();
  
  test.ok(empty);
  
  test.strictEqual(empty.length, 0);
  
  test.done();
}

/**
 * Test the empty constructor without new
 */
module.exports.testEmptySansNew = function(test) {
  var empty = xQ();
  
  test.ok(empty);
  
  test.strictEqual(empty.length, 0);
  
  test.done();
}

/**
 * Test the string constructor
 */
module.exports.testString = function(test) {
  var q = new xQ("<doc><hello /></doc>");
  
  test.ok(q);
  test.strictEqual(q.length, 1);
  
  test.done();
}

/**
 * Test the string constructor without new
 */
module.exports.testStringSansNew = function(test) {
  var q = xQ("<doc><hello /></doc>");
  
  test.ok(q);
  test.strictEqual(q.length, 1);
  
  test.done();
}

/**
 * Test invalid constructor calls
 */
module.exports.testInvalid = function(test) {
  test.throws(function() {
    new xQ("something's not right");
  });
  
  test.throws(function() {
    new xQ(5);
  });
  
  test.done();
}

/**
 * Test invalid constructor calls without new
 */
module.exports.testInvalidSansNew = function(test) {
  test.throws(function() {
    xQ("something's not right");
  });
  
  test.throws(function() {
    xQ(5);
  });
  
  test.done();
}
