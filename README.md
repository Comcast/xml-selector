# XML Selector

XML Selector is a utility for working with XML. It provides partial DOM
support and a jQuery-like interface for traversing XML documents using
CSS-style selectors.

Please note that as of version 0.3 XML Selector uses its own DOM
implementation based on [libxml2](http://xmlsoft.org). This is different
than the previous non-standard document interface. XML Selector currently
implements a read-only subset of DOM Level 1. This will be extended in
the future to meet, at minimum, full Level 1 support.

## Why Should I Use This Thing?

### The TL;DR Version

Because you want a fast, mature XML parser, and you want to write less
code without hassling with a lot of complexity.

Plus, we've thought through things like namespaces, so it will be
possible to do what you need to, no matter how crazy the XML you're
dealing with is, and the most common cases should just work.

### The Longer Version

XML Selector provides an interface for efficiently and accurately working
with XML documents. To get a sense of how it helps save you coding time,
consider the following XML document:

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

Using XML Selector, this becomes:

```javascript
console.log("greeting=%s object=%s",
  $doc.find('item[type="greeting"]').text(),
  $doc.find('item[type="object"]').text());
```
where `$doc` is the document in an XMLSelector instance.

#### But Isn't *This Alternate Solution* Just As Good?

To be sure, there are a number of other viable solutions out there for
easily working with XML, but there are three big advantages that XML
Selector offers:

1. Selectors are simple but powerful

   The selector syntax is very simple and quick to pick up. If you know
   CSS, you already know the syntax. If you don't, you can pick it up in
   its entirety in a few minutes. Other technologies like XPath involve a
   steeper learning curve.

1. Simplicity doesn't come at the cost of information loss

   Some utilities simplify working with XML by converting the document
   into a native data structure (e.g. mapping element names to properties
   on an object). Unfortunately, this tends to involve information loss
   since native data structures generally don't have corollaries for both
   named attributes and named child elements. In the case of namespaces,
   ambiguities tend to arise. XML Selector has the advantage of preserving
   all of the underlying XML document, but making it simpler to find the
   portion you're looking for.

1. No stopping to check for null in the middle

   XML Selector follows jQuery's pattern of providing a collection of
   operations for sets that can be chained together. Since you're working
   with sets, there isn't a need to check for nulls or no results in the
   middle of the chain (no results just yield an empty set instead of a
   null). That frees you up to build a concise set of operations and, if
   needed, check for success or failure at the end.

## Getting Started

To start using the library, require the module, and create a new instance
with a string of XML or by giving it any number of nodes:

```javascript
var $$ = require('xml-selector');

var xmlStr = '<items>' +
               '<item><value>Zero</value></item>' +
               '<item><value>One</value></item>' +
               '<item><value>Two</value></item>' +
             '</items>';

var $doc = $$(xmlStr);

// or

var doc = $$.parseFromString(xmlStr);
var item = doc.documentElement.firstChild;

var $doc2 = $$(item, item.nextSibling, item.nextSibling.nextSibling);
```

If you're already familiar with jQuery, that may be all you need to get
started. Feel free to jump ahead to the [API](#section_api)
to see exactly what's supported.

### Working With Selectors

XML Selector currently supports a subset of CSS selectors. The list of
currently supported selectors is shown below.

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

Consider the following XML. This is a simplified document similar to a
real-life API response I once encountered:

```xml
<foo:ApiResponse xmlns:foo="urn:acmecompany:foo:api:v1.0" version="1.0">
 <foo:Status Code="urn:acmecompany:foo:api:rest:1.0:status:Incorrect" Message="" />
</foo:ApiResponse>
```

That's what an invalid response looks like. Now consider the
corresponding valid response:

```xml
<ApiResponse xmlns="urn:acmecompany:foo:api:v1.0" version="1.0">
  <Value><!-- some result here --></Value>
  <Status Code="urn:acmecompany:foo:api:rest:1.0:status:Success"/>
</ApiResponse>
```

Both of these documents have all elements in the same namespace
(urn:acmecompany:foo:api:v1.0), but they specify it in completely different
ways that mean exactly the same thing. If you're using a library with
good namespace support to parse the document, this is hopefully made
transparent to you, otherwise you're going to have to struggle with the
fact that in some cases the status code can be found in an element named
"foo:Status" and in other cases it's in an element named just "Status".

XML Selector is a bit opinionated about this. It assumes that most of the
time, you don't care about the namespace at all, you just want the Status
element. In the cases where you do care about the namespace, you don't
care whether the document author declared a namespace prefix or set the
default namespace. You just want to access elements from the right one.

Using XML Selector you can access the `Code` attribute of the `Status`
element from both of the above documents either by not specifying a
namespace:

```javascript
q.find('Status').attr('Code');
```

or by explicitly ensuring you match elements with the correct namespace URI:

```javascript
$doc.addNamespace('acme', 'urn:acmecompany:foo:api:v1.0');
$doc.find('acme:Status').attr('Code');
```

### Chaining Operations

Most of the methods on XMLSelector return a new XMLSelector instance with
their results. This allows you to chain together operations and easily
build more complex searches from the basic ones provided by XML Selector.
For an example of how this is useful, let's consider this XML:

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
$doc.find('spanish').closest('book').find('title').map(function(t) { return $$(t).text(); });

// produces: ['Cien años de soledad', 'San Manuel Bueno, mártir']
```

### Non-Matching Operations

One of the advantages we gain from building operations around sets is that
we don't have to introduce a lot of checks and special cases for
operations that don't produce a match. Because the result of an operation
that doesn't match anything is an empty set, and performing operations on
an empty set simply produces another empty set, a set of chained
operations can remain very concise and still work correctly in the case of
non-matching operations.

Let's say, for example, we want to find the titles of books in French instead:

```javascript
$doc.find('french').closest('book').find('title').map(function(t) { return $$(t).text(); });

// produces: []
```

This works as desired, even though we have three different operations that
follow a failed match. Again, that works because a non-match produces an
empty set and not a value like `null`, `false`, or `undefined`.

### Higher-Order Functions

It's possible to produce even more customized search operations by using
some of the higher-order functions which allow you to pass in callbacks
for filtering or finding elements. Continuing with the book example,
let's say we wanted the titles of the books by Unamuno:

```javascript
$doc.find('author')
    .filter(function(elem) {
      return /^de Unamuno,/.test($$(elem).text());
    })
    .closest('book')
    .find('title')
    .map(function(elem) {
      return $$(elem).text();
    });

// produces: ['San Manuel Bueno, mártir']
```

Using the same approach you could just as easily create a function to
return the publication year for an ISBN:

```javascript
function isbnPubYear(isbn) {
  return $doc.find(String(isbn).length == 10 ? 'isbn-10' : 'isbn-13')
             .filter(function(elem) { return $$(elem).text() === isbn; })
             .closest('book')
             .find('published')
             .map(function(elem) { return $$(elem).text(); })[0];
}
```

The functions `filter()` and `find()` both accept callbacks for this
purpose, and several other functions allow you to iterate over, test, or
modify the set using callbacks. See the API section, below, for details.

<a name="section_api"></a>
# API

 * [$$ (XML Selector)](#api_xml_selector)
 * [CharacterData](#api_character_data)
 * [Document](#api_document)
 * [Element](#api_element)
 * [Node](#api_node)

<a name="api_xml_selector"></a>
## $$

This is the function exported by the `xml-selector` module. It may be
invoked directly to obtain a selector instance. It also provides a
utility function for direct DOM usage.

### [new] $$(xmlString)
### [new] $$(node1[, ... nodeN])
### [new] $$(nodeArray)

Accepts an XML String, one or more nodes, an array of nodes, or any
combination thereof. It returns a new selector instance whose context is
the provided node list. It may be optionally be called with `new`, but
that is not required.

### Instance Properties

#### $selector.length

The number of nodes in this instance's list.

### Instance Methods

#### $selector[index]

 * `index`: **Number** Zero-based index

Instances of a selector can be accessed by numerical index like an
array, so you can do things like this:

```javascript
var $$ = require('xml-selector');

var $doc = $$('<items>' +
                '<item content="Zero" />' +
                '<item content="One" />' +
                '<item content="Two" />' +
              '</items>');

var $items = $doc.find('item');

console.log($items[1].getAttribute('content'));
// outputs 'One'
```

Array access returns a Node or `undefined`.

#### $selector.addNamespace(prefix, uri)

 * `prefix`: **String** Namespace prefix to use in selector expressions
 * `uri`: **String** Corresponding namespace URI

Associates a prefix to use in your selectors with a namespace URI.
Returns this selector instance.

#### $selector.attr(name)

 * `name`: **String** Attribute name

Access the value of the named attribute from the first node in the list.
Returns a String.

#### $selector.children([selector])

 * `selector`: **String** Optional selector expression

Return a new XML Selector instance containing the children of the nodes
in this set, optionally filtered by `selector`.

#### $selector.closest(selector)

 * `selector`: **String** Selector expression to match

Return a new XML Selector instance with the closest ancestor of each node
in the list that matches `selector`.

#### $selector.every(predicate[, thisArg])

 * `predicate`: **Function** Callback function for testing items, takes three arguments:
   * `item`: **Node** The item to test
   * `index`: **Number** The index of the item in the list
   * `$selector`: **XMLSelector** This selector instance
 * `thisArg`: **Mixed** Optional value to use as `this` when executing callback

Returns `true` if all items in this set pass the given predicate (a
user-supplied callback which should return a boolean for each item
supplied).

#### $selector.filter(selector)

 * `selector`: **String** Selector expression

Return a new XML Selector instance containing the nodes from this set
that match the given selector.

#### $selector.filter(filterFunction[, thisArg])

 * `filterFunction`: **Function** Callback function for filtering items, takes three arguments:
   * `item`: **Node** The item to test
   * `index`: **Number** The index of the item in the list
   * `$selector`: **XMLSelector** This selector instance
 * `thisArg`: **Mixed** Optional value to use as `this` when executing callback

Return a new XML Selector instance containing only the nodes from this set for which the supplied callback returns `true`.

#### $selector.find(selector)

 * `selector`: **String** Selector expression to search for

Alias of `search`. Searches this set for descendants matching `selector`
and returns a new XML Selector instance with the result.

#### $selector.find(predicate[, thisArg])

 * `predicate`: **Function** Callback function for testing items, takes three arguments:
   * `item`: **Node** The item to test
   * `index`: **Number** The index of the item in the list
   * `$selector`: **XMLSelector** This selector instance
 * `thisArg`: **Mixed** Optional value to use as `this` when executing callback

Find the first Node in this selector instance for which the user-supplied
callback returns `true`. Returns a Node or `undefined`.

#### $selector.findIndex(predicate[, thisArg])

 * `predicate`: **Function** Callback function for testing items, takes three arguments:
   * `item`: **Node** The item to test
   * `index`: **Number** The index of the item in the list
   * `$selector`: **XMLSelector** This selector instance
 * `thisArg`: **Mixed** Optional value to use as `this` when executing callback

Iterate over the nodes in this selector instance and return the index of
the first node for which the user-supplied callback returns `true`.
Returns the numerical index of the matching node, or -1 in the case of no
match.

#### $selector.first()

Returns a new XML Selector instance containing the first node from this
set.

#### $selector.forEach(iterator[, thisArg])

 * `iterator`: **Function** Callback function, takes three arguments:
   * `item`: **Node** The item
   * `index`: **Number** The index of the item in the list
   * `$selector`: **XMLSelector** This selector instance
 * `thisArg`: **Mixed** Optional value to use as `this` when executing callback

Iterate over the nodes in this selector instance (behaves like
Array.forEach). Returns this selector instance.

#### $selector.last()

Returns a new XML Selector instance containing the last node from this
set.

#### $selector.map(iterator[, thisArg])

 * `iterator`: **Function** Callback function, takes three arguments:
   * `item`: **Node** The item to map
   * `index`: **Number** The index of the item in the list
   * `$selector`: **XMLSelector** This selector instance
 * `thisArg`: **Mixed** Optional value to use as `this` when executing callback

Iterates over the nodes in this selector instance and returns a new Array
containing the values returned by each invocation of `iterator`.

#### $selector.next([selector])

 * `selector`: **String** Optional selector expression

Returns a new XML Selector instance containing the next siblings of the
nodes in this set, optionally filtered by `selector`.

#### $selector.nextAll([selector])

 * `selector`: **String** Optional selector expression

Returns a new XML Selector instance containing all the next siblings of
the nodes in this set, optionally filtered by `selector`.

#### $selector.nextUntil(selector)

 * `selector`: **String** Selector expression

Returns a new XML Selector instance containing all the next siblings of
the nodes in this set up to siblings matching `selector`.

#### $selector.not(selector)

 * `selector`: **String** Selector expression

Returns a new XML Selector instance containing the nodes in this set not
matching `selector`.

#### $selector.parent([selector])

 * `selector`: **String** Optional selector expression

Returns a new XML Selector instance containing the parent of the nodes in
this set, optionally filtered by `selector`.

#### $selector.parents([selector])

 * `selector`: **String** Optional selector expression

Returns a new XML Selector instance containing all the ancestors of the
nodes in this set, optionally filtered by `selector`.

#### $selector.parentsUntil(selector)

 * `selector`: **String** Selector expression

Returns a new XML Selector instance containing all the ancestors of the
nodes in this set up to the ancestor matching `selector`.

#### $selector.prev([selector])

 * `selector`: **String** Optional selector expression

Returns a new XML Selector instance containing the previous siblings of
the nodes in this set, optionally filtered by `selector`

#### $selector.prevAll([selector])

 * `selector`: **String** Optional selector expression

Returns a new XML Selector instance containing all the previous siblings
of the nodes in this set, optionally filtered by `selector`.

#### $selector.prevUntil(selector)

 * `selector`: **String** Selector expression

Returns a new XML Selector instance containing all the previous siblings
of the nodes in this set up to siblings matching `selector`.

#### $selector.reduce(iterator, initialValue[, thisArg])

 * `iterator`: **Function** Callback function, takes four arguments:
   * `accumulator`: **Mixed** The value returned by the previous invocation of the callback
   * `item`: **Node** The current item to process
   * `index`: **Number** The index of the item in the list
   * `$selector`: **XMLSelector** This selector instance
 * `initialValue`: **Mixed** Initial value for `accumulator`
 * `thisArg`: **Mixed** Optional value to use as `this` when executing callback

Calls a function for each value in the collection (left-to-right) and
passes the result to the next iteration. Returns the result of the final
call to `iterator`.

#### $selector.reduceRight(iterator, initialValue[, thisArg])

 * `iterator`: **Function** Callback function, takes four arguments:
   * `accumulator`: **Mixed** The value returned by the previous invocation of the callback
   * `item`: **Node** The current item to process
   * `index`: **Number** The index of the item in the list
   * `$selector`: **XMLSelector** This selector instance
 * `initialValue`: **Mixed** Initial value for `accumulator`
 * `thisArg`: **Mixed** Optional value to use as `this` when executing callback

Calls a function for each value in the collection (right-to-left) and
passes the result to the next iteration. Returns the result of the final
call to `iterator`.

#### $selector.search(selector)

 * `selector`: **String** Selector expression

Searches this set for descendants matching `selector` and returns a new
XML Selector instance with the result.

#### $selector.some(predicate[, thisArg])

 * `predicate`: **Function** Callback function for testing items, takes three arguments:
   * `item`: **Node** The item to test
   * `index`: **Number** The index of the item in the list
   * `$selector`: **XMLSelector** This selector instance
 * `thisArg`: **Mixed** Optional value to use as `this` when executing callback

Iterates over the Nodes in this selector instance and returns `true` if
`predicate` returns `true` for at least one of the Nodes. Returns `false`
otherwise.

#### $selector.text()

Returns a String containing the text content of the first element in the
list. In the case of an empty set, an empty String is returned.

#### $selector.xml()

Returns a String containing the XML representation of the first element
in the list. In the case of an empty set, an empty String is returned.

### Utility Functions

#### $$.parseFromString(xmlString)

 * `xmlString`: **String** A string of XML to parse

Parses a string of XML and returns a Document.

<a name="api_character_data">
## CharacterData

CharacterData extends Node. It does not represent an actual node type,
but serves as the base class for Text and Comment node types. This
interface is defined in DOM Level 1. Currently a subset of that interface
is implemented.

### Instance Properties

#### characterData.data

A String containing the character data content of this Node.

#### characterData.length

A Number indicating the length of the `data` property.

<a name="api_document">
## Document

Document extends Node. It is the top-level node in a document tree. This
interface is defined in DOM Level 1. Currently a subset of that interface
is implemented.

### Instance Properties

#### document.documentElement

The top-level Element from the document.

<a name="api_element">
## Element

Element extends Node. It represents an XML element in the document. This
interface is defined in DOM Level 1. Currently a subset of that interface
is implemented.

### Instance Properties

#### element.tagName

A String containing the name of the element.

### Instance Methods

#### element.getAttribute(name)

 * `name`: **String** Attribute name

Return the value of the named attribute. Returns an empty string if the
attribute is not set.

<a name="api_node">
## Node

Node is the base type for all objects in the document tree. This
interface is defined in DOM Level 1. Currently a subset of that interface
is implemented.

### Instance Properties

#### node.firstChild

The first child Node of this Node or `null` for none.

#### node.lastChild

The last child Node of this Node or `null` for none.

#### node.nextSibling

The sibling Node immediate following this Node in the document tree. This
property is `null` in the case of no next sibling.

#### node.nodeName

A String containing the node name as defined in the DOM specification.
For Elements this is the same as the tag name. For other classes, the
property value varies according to the node type.

#### node.nodeType

An integer indicating the node type. Values follow the DOM specification:

 * 1 - **Element**
 * 2 - **Attr**
 * 3 - **Text**
 * 4 - **CDATASection**
 * 5 - **EntityReference**
 * 6 - **Entity**
 * 7 - **ProcessingInstruction**
 * 8 - **Comment**
 * 9 - **Document**
 * 10 - **DocumentType**
 * 11 - **DocumentFragment**
 * 12 - **Notation**

#### node.ownerDocument

The Document that contains this Node. This property is `null` for the
Document itself.

#### node.parentNode

The parent Node of this Node. If this Node has no parent, this property
is `null`. The property will only be `null` for top-level nodes such as
Documents or DocumentFragments, attribute (Attr) nodes, or detached nodes.

#### node.previousSibling

The sibling Node immediate preceding this Node in the document tree. This
property is `null` in the case of no previous sibling.

### Instance Methods

#### node.hasChildNodes()

Returns a boolean. Returns `true` if this node has children, `false` otherwise.
