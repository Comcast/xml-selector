# xq

This is a module for Node.js which implements JavaScript language bindings
for [libxq](http://github.csv.comcast.com/jhunte202/xQ).

It is a utility for working with XML. It provides a jQuery-like interface
for traversing XML documents using CSS-style selectors.

## Why Should I Use This Thing?

### The TL;DR Version

Because you want to write less code and you don't want to hassle with a
lot of complexity. Plus, we've thought through things like namespaces, so
it will be possible to do what you need to, no matter how crazy the XML
you're dealing with is, and the most common cases should just work.

### The Longer Version

xQ provides an interface for efficiently and accurately working with XML
documents. To get a sense of how it helps save you coding time, consider
the following XML document:

```xml
<document>
  <list>
    <item type="greeting">Hello</item>
    <item type="punctuation">,</item>
    <item type="space" />
    <item type="object">world</item>
    <item type="punctuation">!</item>
  </list>
</document>
```

Let's say you want to grab the greeting item text and the object item
text out of this document. If you're using the DOM, it's not exactly a
trivial exercise:

```javascript
function findFirstItemByType(elem, type) {
  if (elem.nodeType === 1 && elem.nodeName === 'item' && elem.getAttribute('type') === type)
    return elem;
  
  if (elem.firstChild) {
    var result = findFirstItemByType(elem.firstChild, type);
    if (result) return result;
  }
  
  if (elem.nextSibling)
    return findFirstItemByType(elem.nextSibling, type);
  
  return type;
}

var greeting = findFirstItemByType(document.documentElement, 'greeting');
var object = findFirstItemByType(document.documentElement, 'object');

var greetingText = greeting ? greeting.textContent : undefined;
var objectText = object ? object.textContent : undefined;

console.log("greeting=%s object=%s", greetingText, objectText);
```

Using xQ, this becomes:

```javascript
console.log("greeting=%s object=%s",
  xQDoc.find('item[type="greeting"]').text(),
  xQDoc.find('item[type="object"]').text());
```

#### But Isn't *\<Alternate Solution\>* Just As Good?

To be sure, there are a number of other viable solutions out there for
more easily working with XML, but there are three big advantages that xQ
offers:

1. Selectors are simple but powerful

   The selector syntax is very simple and quick to pick up. If you know
   CSS, you already know the syntax. If you don't, you can pick it up in
   its entirety in a few minutes. Other technologies like XPath involve a
   steeper learning curve.

1. Simplicity doesn't come at the cost of information loss

   Some utilities simplify working with XML by converting the document
   into a native data structure (e.g. mapping element names to properties
   on an object). Unfortunately this tends to involve information loss
   since native data structures generally don't have corollaries for both
   named attributes and named child elements. In the case of namespaces,
   ambiguities tend to arise. xQ has the advantage of preserving all of
   the underlying XML document, but making it simpler to find the portion
   you're looking for.

1. No stopping to check for null in the middle

   xQ follows jQuery's pattern of providing a set of operations for sets
   that can be chained together. Since you're working with sets, there
   isn't a need to check for nulls or no results in the middle of the
   chain (no results just yield an empty set instead of a null). That
   frees you up to build a concise set of operations and, if needed, check
   for success or failure at the end.

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
  
  /** Associate a prefix to use in your selectors with a namespace URI */
  addNamespace: function(prefix, uri) { /* ... */ },
  
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
