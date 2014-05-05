#include <check.h>

#include <libxq.h>

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
 * Test suite
 */
Suite* search_suite() {
  Suite* s = suite_create("xQ");
  
  singleTestCase(s, tc_ns_prefixes, "namespace prefixes", test_ns_prefixes);

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
