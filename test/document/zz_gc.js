var $$ = require('../../index')
;


/**
 * garbage collector - should not produce errors
 */
module.exports['garbage collector - should not produce errors'] = function(test) {

  try {
    global.gc();
  } catch (e) {
    console.error("\u001B[31m" + "* skipping garbage collection; use --expose-gc to enable" + "\u001B[39m");
  }
  
  test.done();
}
