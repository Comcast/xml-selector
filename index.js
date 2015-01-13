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
var libxmljs = require('libxmljs')
  , xqjs = require('./build/Release/xqjs')
  , util = require('util')
;

// Add JavaScript-based utility functions

/**
 * find function
 */
xqjs.xQ.prototype.find = function(selectorOrPredicate, context) {
  
  if ('function' == typeof selectorOrPredicate) {
    
    var idx = this.findIndex(selectorOrPredicate, context);
    
    return idx === -1 ? undefined : this[idx];
    
  } else {
    
    return this.search(selectorOrPredicate);
    
  }

}

/**
 * every function - returns true if all items pass a predicate
 */
xqjs.xQ.prototype.every = function(predicate, context) {
  return this.findIndex(function(n, idx, self) {
    return ! (context ? predicate.call(context, n, idx, self) : predicate(n, idx, self));
  }) === -1;
}

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
