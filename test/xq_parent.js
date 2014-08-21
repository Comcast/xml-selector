/**
 * Test parent/parents/parentsUntil functions
 */

var xQ = require('../index')

function allXml(q) { return q.map(function(n) { return xQ(n).xml(); }); }

/**
 * Test empty set
 */
module.exports.testEmpty = function(test) {
  var empty = new xQ();

  test.strictEqual(empty.parent().length, 0);
  test.strictEqual(empty.parent('empty').length, 0);
  test.strictEqual(empty.parents().length, 0);
  test.strictEqual(empty.parents('empty').length, 0);
  test.strictEqual(empty.parentsUntil('empty').length, 0);
  
  test.done();
}

/**
 * Test parent()
 */
module.exports.testParent = function(test) {
  var q = new xQ("<doc><container><items><number>1</number><number>2</number></items></container>" +
                 "<items><number>3</number></items>" +
                 "<items><wrapper><number>4</number></wrapper></items></doc>");

  test.deepEqual(allXml(q.find('number').parent()),
    ["<items><number>1</number><number>2</number></items>",
     "<items><number>1</number><number>2</number></items>",
     "<items><number>3</number></items>",
     "<wrapper><number>4</number></wrapper>"]);
     
  test.deepEqual(allXml(q.find('number').parent('items')),
    ["<items><number>1</number><number>2</number></items>",
     "<items><number>1</number><number>2</number></items>",
     "<items><number>3</number></items>"]);
  
  test.deepEqual(allXml(q.find('number').parent('wrapper')), ["<wrapper><number>4</number></wrapper>"]);

  test.deepEqual(allXml(q.find('number').parent('nomatch')), []);
  
  test.done();
}

/**
 * Test parents()
 */
module.exports.testParents = function(test) {
  var q = new xQ("<doc><container><items><number>1</number><number>2</number></items></container>" +
                 "<items><number>3</number></items>" +
                 "<items><wrapper><number>4</number></wrapper></items></doc>");
  
  test.deepEqual(allXml(q.find('number').parents()),
    ["<items><number>1</number><number>2</number></items>",
     "<container><items><number>1</number><number>2</number></items></container>",
     "<doc><container><items><number>1</number><number>2</number></items></container><items><number>3</number></items><items><wrapper><number>4</number></wrapper></items></doc>",
     "<items><number>1</number><number>2</number></items>",
     "<container><items><number>1</number><number>2</number></items></container>",
     "<doc><container><items><number>1</number><number>2</number></items></container><items><number>3</number></items><items><wrapper><number>4</number></wrapper></items></doc>",
     "<items><number>3</number></items>",
     "<doc><container><items><number>1</number><number>2</number></items></container><items><number>3</number></items><items><wrapper><number>4</number></wrapper></items></doc>",
     "<wrapper><number>4</number></wrapper>",
     "<items><wrapper><number>4</number></wrapper></items>",
     "<doc><container><items><number>1</number><number>2</number></items></container><items><number>3</number></items><items><wrapper><number>4</number></wrapper></items></doc>"]);
     
  test.deepEqual(allXml(q.find('number').parents('items')),
    ["<items><number>1</number><number>2</number></items>",
     "<items><number>1</number><number>2</number></items>",
     "<items><number>3</number></items>",
     "<items><wrapper><number>4</number></wrapper></items>"]);
  
  test.deepEqual(allXml(q.find('number').parents('nomatch')), []);

  test.done();
}

/**
 * Test parentsUntil()
 */
module.exports.testParentsUntil = function(test) {
  var q = new xQ("<doc><container><items><number>1</number><number>2</number></items></container>" +
                 "<items><number>3</number></items>" +
                 "<items><wrapper><number>4</number></wrapper></items></doc>");

  test.deepEqual(allXml(q.find('number').parentsUntil('items')), ["<wrapper><number>4</number></wrapper>"]);
  
  test.deepEqual(allXml(q.find('number').parentsUntil('nomatch')),
    ["<items><number>1</number><number>2</number></items>",
     "<container><items><number>1</number><number>2</number></items></container>",
     "<doc><container><items><number>1</number><number>2</number></items></container><items><number>3</number></items><items><wrapper><number>4</number></wrapper></items></doc>",
     "<items><number>1</number><number>2</number></items>",
     "<container><items><number>1</number><number>2</number></items></container>",
     "<doc><container><items><number>1</number><number>2</number></items></container><items><number>3</number></items><items><wrapper><number>4</number></wrapper></items></doc>",
     "<items><number>3</number></items>",
     "<doc><container><items><number>1</number><number>2</number></items></container><items><number>3</number></items><items><wrapper><number>4</number></wrapper></items></doc>",
     "<wrapper><number>4</number></wrapper>",
     "<items><wrapper><number>4</number></wrapper></items>",
     "<doc><container><items><number>1</number><number>2</number></items></container><items><number>3</number></items><items><wrapper><number>4</number></wrapper></items></doc>"]);

  test.done();
}
