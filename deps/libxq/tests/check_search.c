#include <check.h>

#include <libxq.h>

#define singleTestCase(suite, var, name, test) do { \
    TCase* var = tcase_create(name); \
    tcase_add_test(var, test); \
    suite_add_tcase(s, var); \
  } while(0)

/**
 * Test the empty selector
 */
START_TEST (test_empty_selector)
{
  xQSearchExpr* expr;
  xQStatusCode status;
  
  status = xQSearchExpr_alloc_init(&expr, (xmlChar*)"");
  
  ck_assert(status == XQ_OK);
  ck_assert(expr->operation == _xQ_addToOutput);
  ck_assert(expr->next == 0);
  
  xQSearchExpr_free(expr);
}
END_TEST

/**
 * Test a single selector
 */
START_TEST (test_single_selector)
{
  xQSearchExpr* expr;
  xQStatusCode status;
  
  status = xQSearchExpr_alloc_init(&expr, (xmlChar*)"elem");
  
  ck_assert(status == XQ_OK);
  ck_assert(expr->operation == _xQ_findDescendantsByName);
  ck_assert(expr->argc == 2);
  ck_assert(xmlStrcmp(expr->argv[0], (xmlChar*)"elem") == 0);
  ck_assert(expr->argv[1] == 0);
  ck_assert(expr->next == 0);
  
  xQSearchExpr_free(expr);
}
END_TEST

/**
 * Test a namespace selector
 */
START_TEST (test_ns_selector)
{
  xQSearchExpr* expr;
  xQStatusCode status;
  
  status = xQSearchExpr_alloc_init(&expr, (xmlChar*)"ns:elem");
  
  ck_assert(status == XQ_OK);
  ck_assert(expr->operation == _xQ_findDescendantsByName);
  ck_assert(expr->argc == 2);
  ck_assert(xmlStrcmp(expr->argv[0], (xmlChar*)"elem") == 0);
  ck_assert(xmlStrcmp(expr->argv[1], (xmlChar*)"ns") == 0);
  ck_assert(expr->next == 0);
  
  xQSearchExpr_free(expr);
}
END_TEST

/**
 * Test a dual selector
 */
START_TEST (test_dual_selector)
{
  xQSearchExpr* expr;
  xQStatusCode status;
  
  status = xQSearchExpr_alloc_init(&expr, (xmlChar*)"elem1 elem2");
  
  ck_assert(status == XQ_OK);
  ck_assert(expr->operation == _xQ_findDescendantsByName);
  ck_assert(expr->argc == 2);
  ck_assert(xmlStrcmp(expr->argv[0], (xmlChar*)"elem1") == 0);
  ck_assert(expr->argv[1] == 0);
  ck_assert(expr->next != 0);
  
  ck_assert(expr->next->operation == _xQ_findDescendantsByName);
  ck_assert(expr->next->argc == 2);
  ck_assert(xmlStrcmp(expr->next->argv[0], (xmlChar*)"elem2") == 0);
  ck_assert(expr->next->argv[1] == 0);
  ck_assert(expr->next->next == 0);

  xQSearchExpr_free(expr);
}
END_TEST

/**
 * Test a dual selector with namespaces
 */
START_TEST (test_dual_ns_selector)
{
  xQSearchExpr* expr;
  xQStatusCode status;
  
  status = xQSearchExpr_alloc_init(&expr, (xmlChar*)"nsA:elem1 nsB:elem2");
  
  ck_assert(status == XQ_OK);
  ck_assert(expr->operation == _xQ_findDescendantsByName);
  ck_assert(expr->argc == 2);
  ck_assert(xmlStrcmp(expr->argv[0], (xmlChar*)"elem1") == 0);
  ck_assert(xmlStrcmp(expr->argv[1], (xmlChar*)"nsA") == 0);
  ck_assert(expr->next != 0);
  
  ck_assert(expr->next->operation == _xQ_findDescendantsByName);
  ck_assert(expr->next->argc == 2);
  ck_assert(xmlStrcmp(expr->next->argv[0], (xmlChar*)"elem2") == 0);
  ck_assert(xmlStrcmp(expr->next->argv[1], (xmlChar*)"nsB") == 0);
  ck_assert(expr->next->next == 0);

  xQSearchExpr_free(expr);
}
END_TEST

/**
 * Test a wildcard selector
 */
START_TEST (test_wildcard_selector)
{
  xQSearchExpr* expr;
  xQStatusCode status;
  
  status = xQSearchExpr_alloc_init(&expr, (xmlChar*)"* elem");
  
  ck_assert(status == XQ_OK);
  ck_assert(expr->operation == _xQ_findDescendants);
  ck_assert(expr->argc == 0);
  ck_assert(expr->next != 0);
  
  ck_assert(expr->next->operation == _xQ_findDescendantsByName);
  ck_assert(expr->next->argc == 2);
  ck_assert(xmlStrcmp(expr->next->argv[0], (xmlChar*)"elem") == 0);
  ck_assert(expr->next->argv[1] == 0);
  ck_assert(expr->next->next == 0);

  xQSearchExpr_free(expr);
}
END_TEST

/**
 * Test a + combinator
 */
START_TEST (test_plus_combinator)
{
  xQSearchExpr* expr;
  xQStatusCode status;
  
  status = xQSearchExpr_alloc_init(&expr, (xmlChar*)"elem1 + elem2");
  
  ck_assert(status == XQ_OK);
  ck_assert(expr->operation == _xQ_findDescendantsByName);
  ck_assert(expr->argc == 2);
  ck_assert(xmlStrcmp(expr->argv[0], (xmlChar*)"elem1") == 0);
  ck_assert(expr->argv[1] == 0);
  ck_assert(expr->next != 0);
  
  ck_assert(expr->next->operation == _xQ_findNextSiblingByName);
  ck_assert(expr->next->argc == 2);
  ck_assert(xmlStrcmp(expr->next->argv[0], (xmlChar*)"elem2") == 0);
  ck_assert(expr->next->argv[1] == 0);
  ck_assert(expr->next->next == 0);

  xQSearchExpr_free(expr);
}
END_TEST

/**
 * Test a > combinator
 */
START_TEST (test_gt_combinator)
{
  xQSearchExpr* expr;
  xQStatusCode status;
  
  status = xQSearchExpr_alloc_init(&expr, (xmlChar*)"elem1 > elem2");
  
  ck_assert(status == XQ_OK);
  ck_assert(expr->operation == _xQ_findDescendantsByName);
  ck_assert(expr->argc == 2);
  ck_assert(xmlStrcmp(expr->argv[0], (xmlChar*)"elem1") == 0);
  ck_assert(expr->argv[1] == 0);
  ck_assert(expr->next != 0);
  
  ck_assert(expr->next->operation == _xQ_findChildrenByName);
  ck_assert(expr->next->argc == 2);
  ck_assert(xmlStrcmp(expr->next->argv[0], (xmlChar*)"elem2") == 0);
  ck_assert(expr->next->argv[1] == 0);
  ck_assert(expr->next->next == 0);

  xQSearchExpr_free(expr);
}
END_TEST

/**
 * Test a single attribute condition
 */
START_TEST (test_single_attr)
{
  xQSearchExpr* expr;
  xQStatusCode status;
  
  // string value
  status = xQSearchExpr_alloc_init(&expr, (xmlChar*)"elem1[attr=\"value\"]");
  
  ck_assert(status == XQ_OK);
  ck_assert(expr->operation == _xQ_findDescendantsByName);
  ck_assert(expr->argc == 2);
  ck_assert(xmlStrcmp(expr->argv[0], (xmlChar*)"elem1") == 0);
  ck_assert(expr->argv[1] == 0);
  ck_assert(expr->next != 0);
  
  ck_assert(expr->next->operation == _xQ_filterAttributeEquals);
  ck_assert(expr->next->argc == 2);
  ck_assert(xmlStrcmp(expr->next->argv[0], (xmlChar*)"attr") == 0);
  ck_assert(xmlStrcmp(expr->next->argv[1], (xmlChar*)"value") == 0);
  ck_assert(expr->next->next == 0);

  xQSearchExpr_free(expr);

  
  // ident value
  status = xQSearchExpr_alloc_init(&expr, (xmlChar*)"elem1[attr=value]");
  
  ck_assert(status == XQ_OK);
  ck_assert(expr->operation == _xQ_findDescendantsByName);
  ck_assert(expr->argc == 2);
  ck_assert(xmlStrcmp(expr->argv[0], (xmlChar*)"elem1") == 0);
  ck_assert(expr->argv[1] == 0);
  ck_assert(expr->next != 0);
  
  ck_assert(expr->next->operation == _xQ_filterAttributeEquals);
  ck_assert(expr->next->argc == 2);
  ck_assert(xmlStrcmp(expr->next->argv[0], (xmlChar*)"attr") == 0);
  ck_assert(xmlStrcmp(expr->next->argv[1], (xmlChar*)"value") == 0);
  ck_assert(expr->next->next == 0);

  xQSearchExpr_free(expr);
}
END_TEST

/**
 * Test a single attribute condition followed by a combinator
 */
START_TEST (test_single_attr_combinator)
{
  xQSearchExpr* expr;
  xQStatusCode status;
  
  // string value
  status = xQSearchExpr_alloc_init(&expr, (xmlChar*)"elem1[attr=\"value\"] + elem2");
  
  ck_assert(status == XQ_OK);
  ck_assert(expr->operation == _xQ_findDescendantsByName);
  ck_assert(expr->argc == 2);
  ck_assert(xmlStrcmp(expr->argv[0], (xmlChar*)"elem1") == 0);
  ck_assert(expr->argv[1] == 0);
  ck_assert(expr->next != 0);
  
  ck_assert(expr->next->operation == _xQ_filterAttributeEquals);
  ck_assert(expr->next->argc == 2);
  ck_assert(xmlStrcmp(expr->next->argv[0], (xmlChar*)"attr") == 0);
  ck_assert(xmlStrcmp(expr->next->argv[1], (xmlChar*)"value") == 0);
  ck_assert(expr->next->next != 0);

  ck_assert(expr->next->next->operation == _xQ_findNextSiblingByName);
  ck_assert(expr->next->next->argc == 2);
  ck_assert(xmlStrcmp(expr->next->next->argv[0], (xmlChar*)"elem2") == 0);
  ck_assert(expr->next->next->argv[1] == 0);
  ck_assert(expr->next->next->next == 0);

  xQSearchExpr_free(expr);
}
END_TEST

/**
 * Test a dual attribute condition
 */
START_TEST (test_dual_attr)
{
  xQSearchExpr* expr;
  xQStatusCode status;
  
  status = xQSearchExpr_alloc_init(&expr, (xmlChar*)"elem1[attr=\"value\"][attr2=\"value2\"]");
  
  ck_assert(status == XQ_OK);
  ck_assert(expr->operation == _xQ_findDescendantsByName);
  ck_assert(expr->argc == 2);
  ck_assert(xmlStrcmp(expr->argv[0], (xmlChar*)"elem1") == 0);
  ck_assert(expr->argv[1] == 0);
  ck_assert(expr->next != 0);
  
  ck_assert(expr->next->operation == _xQ_filterAttributeEquals);
  ck_assert(expr->next->argc == 2);
  ck_assert(xmlStrcmp(expr->next->argv[0], (xmlChar*)"attr") == 0);
  ck_assert(xmlStrcmp(expr->next->argv[1], (xmlChar*)"value") == 0);
  ck_assert(expr->next->next != 0);

  ck_assert(expr->next->next->operation == _xQ_filterAttributeEquals);
  ck_assert(expr->next->next->argc == 2);
  ck_assert(xmlStrcmp(expr->next->next->argv[0], (xmlChar*)"attr2") == 0);
  ck_assert(xmlStrcmp(expr->next->next->argv[1], (xmlChar*)"value2") == 0);
  ck_assert(expr->next->next->next == 0);

  xQSearchExpr_free(expr);
}
END_TEST

/**
 * Test invalid expressions
 */
START_TEST (test_invalid_expressions)
{
  xQSearchExpr* expr;
  xQStatusCode status;
  
  // string
  status = xQSearchExpr_alloc_init(&expr, (xmlChar*)"'elem'");
  ck_assert(status != XQ_OK);
  ck_assert(expr == 0);
  
  // + empty
  status = xQSearchExpr_alloc_init(&expr, (xmlChar*)"+");
  ck_assert(status != XQ_OK);
  ck_assert(expr == 0);

  // > empty
  status = xQSearchExpr_alloc_init(&expr, (xmlChar*)">");
  ck_assert(status != XQ_OK);
  ck_assert(expr == 0);

  // =
  status = xQSearchExpr_alloc_init(&expr, (xmlChar*)"= elem");
  ck_assert(status != XQ_OK);
  ck_assert(expr == 0);

  // incomplete attrib
  status = xQSearchExpr_alloc_init(&expr, (xmlChar*)"elem[foo");
  ck_assert(status != XQ_OK);
  ck_assert(expr == 0);

  status = xQSearchExpr_alloc_init(&expr, (xmlChar*)"elem[foo=");
  ck_assert(status != XQ_OK);
  ck_assert(expr == 0);

  status = xQSearchExpr_alloc_init(&expr, (xmlChar*)"elem[foo=bar");
  ck_assert(status != XQ_OK);
  ck_assert(expr == 0);

  // invalid attrib
  status = xQSearchExpr_alloc_init(&expr, (xmlChar*)"elem[foo 'bar']");
  ck_assert(status != XQ_OK);
  ck_assert(expr == 0);
  
  // unterminated string
  status = xQSearchExpr_alloc_init(&expr, (xmlChar*)"elem[foo=\"bar]");
  ck_assert(status == XQ_INVALID_SEL_UNTERMINATED_STR);
  ck_assert(expr == 0);
  
}
END_TEST



/**
 * Test suite
 */
Suite* search_suite() {
  Suite* s = suite_create("xQ search");
  
  singleTestCase(s, tc_empty, "empty", test_empty_selector);
  singleTestCase(s, tc_single, "single", test_single_selector);
  singleTestCase(s, tc_ns_single, "namespace", test_ns_selector);
  singleTestCase(s, tc_dual, "dual", test_dual_selector);
  singleTestCase(s, tc_ns_dual, "dual namespace", test_dual_ns_selector);
  singleTestCase(s, tc_wildcard, "wildcard", test_wildcard_selector);
  
  singleTestCase(s, tc_plus_combi, "+ combinator", test_plus_combinator);
  singleTestCase(s, tc_gt_combi, "> combinator", test_gt_combinator);

  singleTestCase(s, tc_single_attr, "single attrib", test_single_attr);
  singleTestCase(s, tc_single_attr_combi, "single attrib +", test_single_attr_combinator);
  singleTestCase(s, tc_dual_attr, "dual attrib", test_dual_attr);

  singleTestCase(s, tc_invalid_expr, "invalid expressions", test_invalid_expressions);

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
