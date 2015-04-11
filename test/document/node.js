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
var $$ = require('../../index')
;


/**
 * nodeType - should return the DOM node type
 */
module.exports['nodeType - should return the DOM node type'] = function(test) {
  var doc = $$.parseFromString("<doc></doc>");
  test.strictEqual(doc.nodeType, 9); // Node.DOCUMENT_NODE
  test.done();
}

/**
 * nodeName - should return the tag name for an element
 */
module.exports['nodeName - should return the tag name for an element'] = function(test) {
  var doc = $$.parseFromString("<doc></doc>");
  test.strictEqual(doc.documentElement.nodeName, "doc");
  test.done();
}

/**
 * nodeName - should allow UTF-8 encoded values
 */
module.exports['nodeName - should allow UTF-8 encoded values'] = function(test) {
  var doc = $$.parseFromString("<d\u00F6c></d\u00F6c>");
  test.strictEqual(doc.documentElement.nodeName, "d\u00F6c");
  test.done();
}

/**
 * firstChild - should return the first child node
 */
module.exports['firstChild - should return the first child node'] = function(test) {
  var doc = $$.parseFromString("<doc><a/><b/><c/></doc>");
  test.ok(doc.documentElement.firstChild);
  test.strictEqual(doc.documentElement.firstChild.nodeType, 1) // Node.ELEMENT_NODE
  test.strictEqual(doc.documentElement.firstChild.nodeName, "a");
  test.done();
}

/**
 * firstChild - should return null for no children
 */
module.exports['firstChild - should return null for no children'] = function(test) {
  var doc = $$.parseFromString("<doc></doc>");
  test.strictEqual(doc.documentElement.firstChild, null);
  test.done();
}

/**
 * lastChild - should return the last child node
 */
module.exports['lastChild - should return the last child node'] = function(test) {
  var doc = $$.parseFromString("<doc><a/><b/><c/></doc>");
  test.ok(doc.documentElement.lastChild);
  test.strictEqual(doc.documentElement.lastChild.nodeType, 1) // Node.ELEMENT_NODE
  test.strictEqual(doc.documentElement.lastChild.nodeName, "c");
  test.done();
}

/**
 * lastChild - should return null for no children
 */
module.exports['lastChild - should return null for no children'] = function(test) {
  var doc = $$.parseFromString("<doc></doc>");
  test.strictEqual(doc.documentElement.lastChild, null);
  test.done();
}

/**
 * parentNode - should return the parent node
 */
module.exports['parentNode - should return the parent node'] = function(test) {
  var doc = $$.parseFromString("<doc><a/><b/><c/></doc>");
  var c = doc.documentElement.lastChild;
  test.ok(c.parentNode);
  test.strictEqual(c.parentNode.nodeType, 1) // Node.ELEMENT_NODE
  test.strictEqual(c.parentNode.nodeName, "doc");
  test.done();
}

/**
 * parentNode - should return null for the document
 */
module.exports['parentNode - should return null for the document'] = function(test) {
  var doc = $$.parseFromString("<doc></doc>");
  test.strictEqual(doc.parentNode, null);
  test.done();
}

/**
 * nextSibling - should return the next sibling node
 */
module.exports['nextSibling - should return the next sibling node'] = function(test) {
  var doc = $$.parseFromString("<doc><a/><b/><c/></doc>");
  var a = doc.documentElement.firstChild;
  test.ok(a.nextSibling);
  test.strictEqual(a.nextSibling.nodeType, 1) // Node.ELEMENT_NODE
  test.strictEqual(a.nextSibling.nodeName, "b");
  test.done();
}

/**
 * nextSibling - should return null for no next sibling
 */
module.exports['nextSibling - should return null for no next sibling'] = function(test) {
  var doc = $$.parseFromString("<doc><a/><b/><c/></doc>");
  var c = doc.documentElement.lastChild;
  test.strictEqual(c.nextSibling, null);
  test.done();
}

/**
 * previousSibling - should return the previous sibling node
 */
module.exports['previousSibling - should return the previous sibling node'] = function(test) {
  var doc = $$.parseFromString("<doc><a/><b/><c/></doc>");
  var c = doc.documentElement.lastChild;
  test.ok(c.previousSibling);
  test.strictEqual(c.previousSibling.nodeType, 1) // Node.ELEMENT_NODE
  test.strictEqual(c.previousSibling.nodeName, "b");
  test.done();
}

/**
 * previousSibling - should return null for no previous sibling
 */
module.exports['previousSibling - should return null for no previous sibling'] = function(test) {
  var doc = $$.parseFromString("<doc><a/><b/><c/></doc>");
  var a = doc.documentElement.firstChild;
  test.strictEqual(a.previousSibling, null);
  test.done();
}

/**
 * ownerDocument - should return the document containing the node
 */
module.exports['ownerDocument - should return the document containing the node'] = function(test) {
  var doc = $$.parseFromString("<doc><a/><b/><c/></doc>");
  var a = doc.documentElement.firstChild;
  test.ok(a.ownerDocument, null);
  test.strictEqual(a.ownerDocument.nodeType, 9); // Node.DOCUMENT_NODE
  test.done();
}

/**
 * ownerDocument - should return null for the document
 */
module.exports['ownerDocument - should return null for the document'] = function(test) {
  var doc = $$.parseFromString("<doc></doc>");
  test.strictEqual(doc.ownerDocument, null);
  test.done();
}

/**
 * hasChildNodes() - should return true for a node with children
 */
module.exports['hasChildNodes - should return true for a node with children'] = function(test) {
  var doc = $$.parseFromString("<doc><a/><b/><c/></doc>");
  test.strictEqual(doc.hasChildNodes(), true);
  test.strictEqual(doc.documentElement.hasChildNodes(), true);
  test.done();
}

/**
 * hasChildNodes() - should return false for a node without children
 */
module.exports['hasChildNodes - should return false for a node without children'] = function(test) {
  var doc = $$.parseFromString("<doc></doc>");
  test.strictEqual(doc.documentElement.hasChildNodes(), false);
  test.done();
}

