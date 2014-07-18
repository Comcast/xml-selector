/**
 * Test the attr() function
 */

var xQ = require('../index')

/**
 * Test attr call on an empty set
 */
module.exports.testEmpty = function(test) {
  var empty = new xQ();

  test.strictEqual(empty.attr("name"), undefined);
  
  test.done();
}

/**
 * Test attr call on a document
 */
module.exports.testDocument = function(test) {
  var q = new xQ('<doc><people><person name="Fred" /></people></doc>');
  
  test.strictEqual(q.attr('name'), undefined);
  
  test.done();
}

/**
 * Test attr call on a single element
 */
module.exports.testSingle = function(test) {
  var q = new xQ('<doc><people><person name="Fred" /></people></doc>');
  
  test.strictEqual(q.find('person').attr('name'), 'Fred');
  test.strictEqual(q.find('person').attr('age'), undefined);
  
  test.done();
}

/**
 * Test attr call on multiple elements
 */
module.exports.testMultiple = function(test) {
  var q = new xQ('<doc><people><person name="Sally" /><person name="Susan" /></people></doc>');
  
  test.strictEqual(q.find('person').attr('name'), 'Sally');
  
  test.done();
}
