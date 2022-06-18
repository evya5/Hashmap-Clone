#include "hashmap.h"
#include <stdlib.h>
#include <assert.h>
#include <stdio.h>

// Vector tests

void *int_ptr_cpy (const void *p)
{
  if (p == NULL)
    return NULL;
  int **a = malloc (sizeof (int *));
  if (a == NULL)
    return NULL;
  *a = *((int **) p);
  return a;
}

int int_ptr_cmp (const void *p1, const void *p2)
{
  if (p1 == NULL || p2 == NULL)
    return 0;
  return *((int **) p1) == *((int **) p2);
}

void int_ptr_free (void **p)
{
  if (p == NULL || *p == NULL)
    return;
  free (*p);
  *p = NULL;
}

void *int_cpy (const void *p)
{
  if (p == NULL)
    return NULL;
  int *a = malloc (sizeof (int));
  if (a == NULL)
    return NULL;
  *a = *((int *) p);
  return a;
}

int int_cmp (const void *p1, const void *p2)
{
  if (p1 == NULL || p2 == NULL)
    return 0;
  return *((int *) p1) == *((int *) p2);
}

void int_free (void **p)
{
  if (p == NULL || *p == NULL)
    return;
  free (*p);
  *p = NULL;
}

void test_vector_alloc ()
{
  // Test any function is NULL
  vector *v = vector_alloc (NULL, int_ptr_cmp, int_ptr_free);
  assert(v == NULL);
  v = vector_alloc (int_ptr_cpy, NULL, int_ptr_free);
  assert(v == NULL);
  v = vector_alloc (int_ptr_cpy, int_ptr_cmp, NULL);
  assert(v == NULL);

  v = vector_alloc (int_ptr_cpy, int_ptr_cmp, int_ptr_free);
  if (v == NULL)
    exit (EXIT_FAILURE);

  assert(v->size == 0);
  assert(v->capacity == VECTOR_INITIAL_CAP);
  assert(v->data != NULL);
  assert(v->elem_copy_func == int_ptr_cpy);
  assert(v->elem_cmp_func == int_ptr_cmp);
  assert(v->elem_free_func == int_ptr_free);
  vector_free (&v);
}

void *null_cpy (const void *p)
{
  if (p == NULL)
    return (void *)p;
  return NULL;
}

void test_vector_push_back_failed_allocations()
{
  vector *v = vector_alloc (null_cpy, int_ptr_cmp, int_ptr_free);
  if (v == NULL)
    exit (EXIT_FAILURE);

  for (int i = 1; i <= 10; ++i)
    {
      int *p = &i;
      assert(vector_push_back (v, &p) == 0);
      assert(v->size == 0);
    }

  vector_free (&v);
}

void test_vector_push_back ()
{
	test_vector_push_back_failed_allocations();
  vector *v = vector_alloc (int_ptr_cpy, int_ptr_cmp, int_ptr_free);
  if (v == NULL)
    exit (EXIT_FAILURE);

  // Check pointers are inserted correctly
  for (int i = 1; i <= 10; ++i)
    {
      int *p = &i;
      assert(vector_push_back (v, &p) == 1);
      assert(v->size == (size_t) i);
    }
  // Check that NULL can be inserted correctly **as a value**
  int *a = NULL;
  assert(vector_push_back (v, &a) == 1);
  assert(v->size == 11);

  // Check that NULL cannot be inserted directly
  assert(vector_push_back (v, NULL) == 0);
  assert(v->size == 11);
  assert(vector_push_back (NULL, &a) == 0);
  assert(v->size == 11);

  // Check that the vector resizes correctly
  int b = 5;
  a = &b;
  while (vector_get_load_factor (v) < VECTOR_MAX_LOAD_FACTOR)
    {
      assert(v->capacity == VECTOR_INITIAL_CAP);
      assert(vector_push_back (v, &a) == 1);
    }

  assert(vector_push_back (v, &a) == 1);
  assert(v->capacity == VECTOR_INITIAL_CAP * VECTOR_GROWTH_FACTOR);

  vector_free (&v);

}

void test_vector_get_load_factor ()
{
  vector *v = vector_alloc (int_ptr_cpy, int_ptr_cmp, int_ptr_free);
  if (v == NULL)
    exit (EXIT_FAILURE);
  assert(vector_get_load_factor (NULL) == -1);
  for (int i = 1; i <= 10; ++i)
    {
      int *p = &i;
      vector_push_back (v, &p);
      assert(vector_get_load_factor (v)
             == ((double) (i) / (double) VECTOR_INITIAL_CAP));
    }

  vector_free (&v);
}

void test_vector_at (void)
{
  vector *v = vector_alloc (int_cpy, int_cmp, int_free);
  if (v == NULL)
    exit (EXIT_FAILURE);

  for (int i = 0; i < 10; ++i)
    {
      vector_push_back (v, &i);
    }

  for (int i = 0; i < 10; ++i)
    {
      assert(*((int *) vector_at (v, i)) == i);
    }

  // Check invalid indexes
  assert(vector_at (v, -1) == NULL);
  assert(vector_at (NULL, 0) == NULL);
  assert(vector_at (v, v->size) == NULL);

  vector_free (&v);
}

void test_vector_find ()
{
  vector *v = vector_alloc (int_ptr_cpy, int_ptr_cmp, int_ptr_free);
  if (v == NULL)
    exit (EXIT_FAILURE);

  for (int i = 1; i <= 10; ++i)
    {
      int *p = &i;
      vector_push_back (v, &p);
    }
  // Check that NULL can be found
  int *a = NULL;
  vector_push_back (v, &a);

  assert(vector_find (v, &a) == 10);

  vector_free (&v);

  v = vector_alloc (int_cpy, int_cmp, int_free);
  if (v == NULL)
    exit (EXIT_FAILURE);

  for (int i = 0; i < 10; ++i)
    {
      vector_push_back (v, &i);
    }

  for (int i = 0; i < 10; ++i)
    {
      assert(vector_find (v, &i) == i);
    }

  // Check invalid values
  int t = 5;
  assert(vector_find (NULL, &t) == -1);
  t = -1;
  char x[] = "This should not be found";
  assert(vector_find (v, &t) == -1);
  assert(vector_find (v, NULL) == -1);
  assert(vector_find (v, x) == -1);

  vector_free (&v);

}

void test_vector_erase (void)
{
  vector *v = vector_alloc (int_cpy, int_cmp, int_free);
  if (v == NULL)
    exit (EXIT_FAILURE);

  for (int i = 0; i < 7; ++i)
    {
      vector_push_back (v, &i);
    }
  assert(vector_erase (v, 0) == 1);
  assert(vector_erase (v, 0) == 1);
  assert(v->size == 5);
  for (int i = 0; i < 5; ++i)
    {
      assert(*((int *) vector_at (v, i)) == i + 2);
    }
  int i = 0;
  assert(vector_find (v, &i) == -1);
  i = 1;
  assert(vector_find (v, &i) == -1);

  // Check invalid indexes
  assert(vector_erase (v, -1) == 0);
  assert(vector_erase (NULL, 0) == 0);
  assert(v->size == 5);
  assert(vector_erase (v, v->size) == 0);
  assert(v->size == 5);

  // Check vector changes size properly
  while (vector_get_load_factor (v) > VECTOR_MIN_LOAD_FACTOR)
    {
      assert(v->capacity == VECTOR_INITIAL_CAP);
      assert(vector_erase (v, 0) == 1);
    }

  assert(vector_erase (v, 0) == 1);
  assert(v->capacity == VECTOR_INITIAL_CAP / VECTOR_GROWTH_FACTOR);

  vector_free (&v);
}

void test_vector_clear (void)
{
  vector *v = vector_alloc (int_cpy, int_cmp, int_free);
  if (v == NULL)
    exit (EXIT_FAILURE);

  for (int i = 0; i < 7; ++i)
    {
      vector_push_back (v, &i);
    }
  // Just to make sure the program does not crash
  vector_clear (NULL);
  vector_clear (v);
  assert(v->size == 0);
  assert(vector_erase (v, 0) == 0);
  assert(vector_at (v, 0) == NULL);
  int i = 0;
  assert(vector_find (v, &i) == -1);
  assert(vector_push_back(v, &i) == 1);

  vector_free (&v);
}

int main ()
{
  printf ("Vector tests start:\n\n");
  test_vector_alloc ();
  printf ("Passed alloc test.\n");
  test_vector_get_load_factor ();
  printf ("Passed get_load_factor test.\n");
  test_vector_push_back ();
  printf ("Passed push_back test.\n");
  test_vector_at ();
  printf ("Passed vector_at test.\n");
  test_vector_find ();
  printf ("Passed vector_find test.\n");
  test_vector_erase ();
  printf ("Passed vector_erase test.\n");
  test_vector_clear ();
  printf ("Passed vector_clear test.\n");

  printf ("\nPassed all tests :)\n");
  return 0;
}