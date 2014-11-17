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
function find(nodeList, testFunction) {
  for (var i = 0; i < nodeList.length; i++)
    if (testFunction(nodeList[i], i, nodeList))
      return nodeList[i];
}

var items = document.getElementsByName('item');

var greeting = find(items, function(i) { return i.getAttribute('type') === 'greeting'; });
var object = find(items, function(i) { return i.getAttribute('type') === 'object'; });

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
easily working with XML, but there are three big advantages that xQ
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

## Getting Started

To start using the library, require the module, and create a new instance
with a string of XML or by giving it any number of nodes from
[libxmljs](https://github.com/polotek/libxmljs):

```javascript
var xQ = require('xq');

var xmlStr = '<items>' +
               '<item><value>Zero</value></item>' +
               '<item><value>One</value></item>' +
               '<item><value>Two</value></item>' +
             '</items>';

var q = new xQ(xmlStr);

// or

var libxmljs = require('libxmljs');
var doc = libxmljs.parseXml(xmlStr);
var children = doc.root().childNodes();

var q2 = new xQ(children[0], children[1], children[2]);
```

If you're already familiar with jQuery, that may be all you need to get
started. Feel free to jump ahead to the [Interface](#section_interface)
to see exactly what's supported.

### Working With Selectors

xQ currently supports a subset of CSS selectors. The list of currently
supported selectors is shown below.

 * __\*__
   matches any element
 * **E**
   matches any *E* element
 * **E F**
   matches any *F* element that is a descendant of an *E* element
 * **E > F**
   matches any *F* element that is a child of an *E* element
 * **E + F**
   matches any *F* element whose previous element sibling is an *E* element
 * **E[foo="warning"]**
   matches any *E* element whose *foo* attribute is exactly equal to *"warning"*.

If you're not already familiar with CSS selectors, the rules are pretty
simple. A selector with just a name: `"item"` will search the document
(or current context) for all elements with that name. So, that selector
would return all of the `item` elements from the document in our example
at the start of this page.

You can combine multiple names for more complex searches. Separating the
names with only a space means any number of elements may come between
those, but the elements to the right must be descendants of those to the
left. So, a selector like `"document item"` would also match all of the
`item` elements in our example at top since all of those elements are
descendants of the `document` element. The `items` element that comes
between them doesn't have to appear in the selector because the space
separator doesn't require the elements to be direct descendants.

The `>` combinator, however, does require elements to be direct
descendants. So a selector of `"document > item"` wouldn't match anything
in our example. A selector of either `"items > item"` or
`"document > items > item"` would match those same `item` elements.

You can further limit elements that match by specifying attribute values
in square brackets. As shown in the example code in the introduction, you
can limit the item elements to specific types, for example, only type
attributes with a value of `"greeting"`, like so:
`'item[type="greeting"]'`.

A second combinator, the `+` sign, can also be used to specify a different
relationship between elements. That symbol requires the matching element
on the right to be the next sibling of the element on the left of the `+`
sign. So, a selector like `'item[type="space"] + item'` would match the
`<item type="object">world</item>` element, since it is the next sibling
of the space type item.

Lastly, `*` matches any element. By itself it can be used to return every
element in the document. It's most useful, though, for expressions where
you may want to match multiple element names, such any element of type
punctuation: `'*[type="punctuation"]'` or any next sibling of an item
element: `"item + *"`.

### Namespaces

Namespaces are frequently a source of problems. The flexibility of
declaring namespaces in different ways in XML combines with varying
support in available XML tools to create a whole lot of headache for
developers.

Consider the following real-life XML response from the CIMA REST API:

```xml
<cima:AuthnResponse xmlns:cima="urn:comcast:login:api:v1.0" version="1.0">
 <cima:Status Code="urn:comcast:login:api:rest:1.0:status:Incorrect" Message="" />
</cima:AuthnResponse>
```

That's what an invalid response looks like. Now consider the
corresponding valid response:

```xml
<AuthnResponse xmlns="urn:comcast:login:api:v1.0" version="1.0">
  <LoginToken ExpiresOn="2013-01-08T03:54:21.488Z"><!-- very long token omitted --></LoginToken>
  <Status Code="urn:comcast:login:api:rest:1.0:status:Success"/>
</AuthnResponse>
```

Both of these documents have all elements in the same namespace
(urn:comcast:login:api:v1.0), but they specify it in completely different
ways that mean exactly the same thing. If you're using a library with
good namespace support to parse the document, this is hopefully made
transparent to you, otherwise you're going to have to struggle with the
fact that in some cases the status code can be found in an element named
"cima:Status" and in other cases it's in an element named just "Status".

xQ is a bit opinionated about this. It assumes that most of the time, you
don't care about the namespace at all, you just want the Status element.
In the cases where you do care about the namespace, you don't care
whether the document author declared a namespace prefix or set the
default namespace. You just want to access elements from the right one.

Using xQ you can access the `Code` attribute of the `Status` element from
both of the above documents either by not specifying a namespace:

```javascript
q.find('Status').attr('Code');
```

or by explicitly ensuring you match elements with the correct namespace URI:

```javascript
q.addNamespace('login', 'urn:comcast:login:api:v1.0');
q.find('login:Status').attr('Code');
```

### Chaining Operations

Most of the methods on xQ return a new xQ instance with their results.
This allows you to chain together operations and easily build more complex
searches from the basic ones provided by xQ. For an example of how this is
useful, let's consider this XML:

```xml
<catalog>
  <books>
    <book>
      <title>Bean Trees, The</title>
      <author>Kingsolver, Barbara</author>
      <published>1988</published>
      <languages><english/></languages>
      <isbn-10>0061097314</isbn-10>
      <isbn-13>978-0061097317</isbn-13>
      <publisher>HarperTorch</publisher>
    </book>
    <book>
      <title>Cien años de soledad</title>
      <author>García Márquez, Gabriel</author>
      <published>1967</published>
      <languages><spanish/></languages>
      <isbn-10>0307474720</isbn-10>
      <isbn-13>978-0307474728</isbn-13>
      <publisher>Vintage Espanol</publisher>
    </book>
    <book>
      <title>San Manuel Bueno, mártir</title>
      <author>de Unamuno, Miguel</author>
      <published>1931</published>
      <languages><spanish/></languages>
      <isbn-10>8437601851</isbn-10>
      <isbn-13>978-8437601854</isbn-13>
      <publisher>Catedra</publisher>
    </book>
    <book>
      <title>To Kill a Mockingbird</title>
      <author>Lee, Harper</author>
      <published>1960</published>
      <languages><english/></languages>
      <isbn-10>0446310786</isbn-10>
      <isbn-13>978-0446310789</isbn-13>
      <publisher>Grand Central Publishing</publisher>
    </book>
  </books>
</catalog>
```

Now let's say we'd like to list the titles of all the books in Spanish. We
don't have selectors sophisticated enough to match that pattern, but we can
do something like this:

```javascript
q.find('spanish').closest('book').find('title').map(function(t) { return t.text(); });

// produces: ['Cien años de soledad', 'San Manuel Bueno, mártir']
```

One of the advantages we gain from building operations around sets is that
we don't have to introduce a lot of checks and special cases for
operations that don't produce a match. Because the result of an operation
that doesn't match anything is an empty set, and performing operations on
an empty set simply produces another empty set, a set of chained
operations can remain very concise and still work correctly in the case of
non-matching operations.

Let's say, for example, we want to find the titles of books in French instead:

```javascript
q.find('french').closest('book').find('title').map(function(t) { return t.text(); });

// produces: []
```

This works as desired, even though we have three different operations that
follow a failed match. Again, that works because a non-match produces an
empty set and not a value like `null`, `false`, or `undefined`.

<a name="section_interface"></a>
## Interface

Conceptually, the function exported by the module implements this
interface:

```javascript
/** Constructor accepting 0 or more nodes, strings of XML, or combinations thereof */
var xQ = function(xmlOrNode /*, ... */) {
}

xQ.prototype = {
  
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
  
};

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
