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

/** Constructor accepting 1 XML string or 0 or more nodes */
var xQ = function(xmlOrNode /*, ... */) {
}

_.extend(xQ.prototype, {
  
  /** number of nodes in this instance's list */
  length: 0
  
});

module.exports = xQ;
```
