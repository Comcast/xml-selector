# xq

This is a module for Node.js which implements JavaScript language bindings
for [libxq](http://github.csv.comcast.com/jhunte202/xQ).

It is a utility for working with XML. It provides a jQuery-like interface
for traversing XML documents using CSS-style selectors.

## Interface

Conceptually, the function exported by the module implements this
interface:

```javascript
var _ = require('underscore');

/** Constructor accepting 0 or more XML strings or nodes */
var xQ = function() {
}

_.extend(xQ.prototype, {
  
  /* number of nodes in this instance's list */
  length: 0
  
});

module.exports = xQ;
```
