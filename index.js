var libxmljs = require('libxmljs')
  , xqjs = require('./build/Release/xqjs')
  , util = require('util')
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

/**
 * Wrap the native constructor with routines that normalize how it is
 * called.
 */
function xQ() {
  
  var nodes = [];
  
  function addArgs(args) {
    var len = args.length;
    for (var i = 0; i < len; ++i) {
      var a = args[i];
      
      if (Array.isArray(a))
        addArgs(a);
      else if ("object" === typeof a)
        nodes.push(a);
      else
        nodes.push(libxmljs.parseXml(String(a)));
    }
  }
  
  addArgs(arguments);
  
  return new xqjs.xQ(nodes);
}

util.inherits(xQ, xqjs.xQ);

module.exports = xQ;
