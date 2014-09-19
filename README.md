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
  
  /** Return a new xQ containing the children of the nodes in this set, optionally filtered by a selector */
  children: function(optionalSelector) { /* ... */ },
  
  /** Return a new xQ with the closest ancestor of each node in the list that matches the supplied selector */
  closest: function(selector) { /* ... */ },
  
  /** Return a new xQ containing the nodes from this set that match the given selector */
  filter: function(selector) { /* ... */ },
  
  /** Search this set for descendants matching a selector and return a new xQ with the result */
  find: function(selector) { /* ... */ },
  
  /** Return a new xQ containing the first node from this set */
  first: function() { /* ... */ },
  
  /** Iterate over the nodes in this instance (behaves like Array.forEach) and returns this */
  forEach: function(iterator, thisArg) { /* ... */ },
  
  /** Return a new xQ containing the last node from this set */
  last: function() { /* ... */ },
  
  /** Iterate over the nodes in this instance and return a new Array containing the returned result of each step */
  map: function(iterator, thisArg) { /* ... */ },
  
  /** Return a new xQ containing the next siblings of the nodes in this set, optionally filtered by a selector */
  next: function(optionalSelector) { /* ... */ },
  
  /** Return a new xQ containing all the next siblings of the nodes in this set, optionally filtered by a selector */
  nextAll: function(optionalSelector) { /* ... */ },

  /** Return a new xQ containing all the next siblings of the nodes in this set up to siblings matching a selector */
  nextUntil: function(selector) { /* ... */ },

  /** Return a new xQ containing the nodes in this set not matching a selector */
  not: function(selector) { /* ... */ },

  /** Return a new xQ containing the parent of the nodes in this set, optionally filtered by a selector */
  parent: function(optionalSelector) { /* ... */ },
  
  /** Return a new xQ containing all the ancestors of the nodes in this set, optionally filtered by a selector */
  parents: function(optionalSelector) { /* ... */ },

  /** Return a new xQ containing all the ancestors of the nodes in this set up to the ancestor matching a selector */
  parentsUntil: function(selector) { /* ... */ },

  /** Return a new xQ containing the previous siblings of the nodes in this set, optionally filtered by a selector */
  prev: function(optionalSelector) { /* ... */ },
  
  /** Return a new xQ containing all the previous siblings of the nodes in this set, optionally filtered by a selector */
  prevAll: function(optionalSelector) { /* ... */ },

  /** Return a new xQ containing all the previous siblings of the nodes in this set up to siblings matching a selector */
  prevUntil: function(selector) { /* ... */ },

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
