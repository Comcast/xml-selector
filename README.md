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
  
  /** Number of nodes in this instance's list */
  length: 0,
  
  /** Return the value of the named attribute from the first node in the list */
  attr: function(name) { /* ... */ },
  
  /** Return a new xQ containing the nodes that matched the given selector */
  find: function(selector) { /* ... */ },
  
  /** Return a new xQ containing the first node from this set */
  first: function() { /* ... */ },
  
  /** Iterate over the nodes in this instance (behaves like Array.forEach) and returns this */
  forEach: function(iterator, thisArg) { /* ... */ },
  
  /** Iterate over the nodes in this instance and return a new Array containing the returned result of each step */
  map: function(iterator, thisArg) { /* ... */ },
  
  /** Return the text content of the first element in the list or an empty string */
  text: function() { /* ... */ },
  
  /** Return the xml of the first element in the list or an empty string */
  xml: function() { /* ... */ }
  
});

module.exports = xQ;
```

Additionally, instances of xQ can be accessed by numerical index like an
array, so you can do things like this:

```javascript
var xQ = require('xq');

var q = new xQ('<items>' +
                  '<item>Zero</item>' +
                  '<item>One</item>' +
                  '<item>Two</item>' +
                '</items>');

var items = q.find('item');

console.log(items[1].text());
// outputs 'One'
```
