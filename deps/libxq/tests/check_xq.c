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
#include <check.h>

#include <libxq.h>

#include <string.h>

#define singleTestCase(suite, var, name, test) do { \
    TCase* var = tcase_create(name); \
    tcase_add_test(var, test); \
    suite_add_tcase(s, var); \
  } while(0)

/**
 * Test addNamespace/namespaceForPrefix
 */
START_TEST (test_ns_prefixes)
{
  xQ* x;
  xQStatusCode status;
  
  status = xQ_alloc_init(&x);
  ck_assert(status == XQ_OK);

  ck_assert(xQ_namespaceForPrefix(x, (xmlChar*)"nsA") == 0);
  
  status = xQ_addNamespace(x, (xmlChar*)"nsA", (xmlChar*)"http://csv.comcast.com/A");
  ck_assert(status == XQ_OK);
  
  status = xQ_addNamespace(x, (xmlChar*)"nsB", (xmlChar*)"http://csv.comcast.com/B");
  ck_assert(status == XQ_OK);

  ck_assert(xmlStrcmp(xQ_namespaceForPrefix(x, (xmlChar*)"nsA"), (xmlChar*)"http://csv.comcast.com/A") == 0);
  ck_assert(xmlStrcmp(xQ_namespaceForPrefix(x, (xmlChar*)"nsB"), (xmlChar*)"http://csv.comcast.com/B") == 0);
  ck_assert(xQ_namespaceForPrefix(x, (xmlChar*)"nsC") == 0);
  
  xQ_free(x, 1);
}
END_TEST

/**
 * Test find with namespace
 */
START_TEST (test_ns_find)
{
  xQ* x;
  xQ* x2;
  xQStatusCode status;
  const char* xml = "<doc xmlns=\"http://csv.comcast.com/A\" xmlns:nsb=\"http://csv.comcast.com/B\"><item>A</item><nsb:item>B</nsb:item></doc>";
  int xmlLen = strlen(xml);
  xmlDocPtr doc;
  xmlChar* txt;
  
  status = xQ_alloc_initMemory(&x, xml, xmlLen, &doc);
  ck_assert(status == XQ_OK);
  
  status = xQ_addNamespace(x, (xmlChar*)"nsA", (xmlChar*)"http://csv.comcast.com/A");
  ck_assert(status == XQ_OK);
  
  status = xQ_addNamespace(x, (xmlChar*)"nsB", (xmlChar*)"http://csv.comcast.com/B");
  ck_assert(status == XQ_OK);


  status = xQ_find(x, (xmlChar*)"nsA:item", &x2);

  ck_assert(status == XQ_OK);
  ck_assert(xQ_length(x2) == 1);
  ck_assert(xmlStrcmp((txt = xQ_getText(x2)), (xmlChar*)"A") == 0);
  
  xmlFree(txt);
  xQ_free(x2, 1);


  status = xQ_find(x, (xmlChar*)"nsB:item", &x2);

  ck_assert(status == XQ_OK);
  ck_assert(xQ_length(x2) == 1);
  ck_assert(xmlStrcmp((txt = xQ_getText(x2)), (xmlChar*)"B") == 0);
  
  xmlFree(txt);
  xQ_free(x2, 1);


  status = xQ_find(x, (xmlChar*)"item", &x2);

  ck_assert(status == XQ_OK);
  ck_assert(xQ_length(x2) == 2);
  ck_assert(xmlStrcmp((txt = xQ_getText(x2)), (xmlChar*)"A") == 0);
  
  xmlFree(txt);
  xQ_free(x2, 1);


  status = xQ_find(x, (xmlChar*)"nsC:item", &x2);
  ck_assert(status == XQ_UNKNOWN_NS_PREFIX);
  
  xQ_free(x, 1);

  xmlFreeDoc(doc);
}
END_TEST

/**
 * Test xml
 */
START_TEST (test_xml_no_children)
{
  xQ* x;
  xQ* x2;
  xQStatusCode status;
  const char* xml = "<doc><hello /></doc>";
  int xmlLen = strlen(xml);
  xmlDocPtr doc;
  xmlChar* txt;
  
  status = xQ_alloc_initMemory(&x, xml, xmlLen, &doc);
  ck_assert(status == XQ_OK);
  
  status = xQ_find(x, (xmlChar*)"hello", &x2);

  ck_assert(status == XQ_OK);
  ck_assert(xQ_length(x2) == 1);
  ck_assert(xmlStrcmp((txt = xQ_getXml(x2)), (xmlChar*)"<hello/>") == 0);
  
  xmlFree(txt);
  xQ_free(x2, 1);
  
  xQ_free(x, 1);

  xmlFreeDoc(doc);
}
END_TEST



/**
 * Test suite
 */
Suite* search_suite() {
  Suite* s = suite_create("xQ");
  
  singleTestCase(s, tc_ns_prefixes, "namespace prefixes", test_ns_prefixes);

  singleTestCase(s, tc_ns_find, "find with namespace", test_ns_find);

  singleTestCase(s, tc_xml_no_children, "xml without children", test_xml_no_children);

  return s;
}


int main() {
  int numFailed;
  Suite* s = search_suite();
  SRunner *sr = srunner_create(s);
  
  srunner_run_all(sr, CK_NORMAL);
  
  numFailed = srunner_ntests_failed(sr);
  
  srunner_free(sr);
  
  return (numFailed == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}
