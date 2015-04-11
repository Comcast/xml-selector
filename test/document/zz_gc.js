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
