var libxmljs = require('libxmljs')
  , xqjs = require('./build/Release/xqjs')
;

// Add JavaScript-based utility functions

/**
 * map function
 */
xqjs.xQ.prototype.map = function(iterator, context) {
  var results = [];
  
  this.forEach(function(value, index, q) {
    results.push(iterator.call(context, value, index, q));
  });
  
  return results;
}

module.exports = xqjs.xQ;
