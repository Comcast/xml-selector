/**
 * Test invalid selectors
 */

var xQ = require('../index')

/**
 * Utility to catch an exception and return its message
 */
function exceptionMessage(block) {
  try {
    block();
  } catch (e) {
    return e.message;
  }
}

/**
 * Test unterminated quotes
 */
module.exports.testUnterminatedQuote = function(test) {
  var empty = new xQ();

  test.strictEqual(exceptionMessage(function() { empty.find("'string value"); }), "Unterminated string in selector");
  
  test.strictEqual(exceptionMessage(function() { empty.find("\"string value"); }), "Unterminated string in selector");
  
  test.done();
}

/**
 * Test bad token sequence
 */
module.exports.testBadToken = function(test) {
  var empty = new xQ();

  test.strictEqual(exceptionMessage(function() { empty.find(">> bad child"); }), "Invalid selector");
  
  test.strictEqual(exceptionMessage(function() { empty.find("elem +"); }), "Invalid selector");
  
  test.done();
}

/**
 * Test attribute-related errors
 */
module.exports.testAttributes = function(test) {
  var empty = new xQ();

  // no selector
  test.strictEqual(exceptionMessage(function() { empty.find("[attr=\"value\"]"); }), "Invalid selector");
  
  // missing [
  test.strictEqual(exceptionMessage(function() { empty.find("elem attr=\"value\"]"); }), "Invalid selector");
  
  // missing =
  test.strictEqual(exceptionMessage(function() { empty.find("elem[attr\"value\"]"); }), "Invalid selector");
  
  // missing string
  test.strictEqual(exceptionMessage(function() { empty.find("elem[attr=]"); }), "Invalid selector");
  
  // missing ]
  test.strictEqual(exceptionMessage(function() { empty.find("elem[attr=\"value\""); }), "Invalid selector");
  
  test.done();
}
