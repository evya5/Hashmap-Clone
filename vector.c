#include "vector.h"

/**
 * Dynamically allocates a new vector.
 * @param elem_copy_func func which copies the element stored in the vector (returns
 * dynamically allocated copy).
 * @param elem_cmp_func func which is used to compare elements stored in the vector.
 * @param elem_free_func func which frees elements stored in the vector.
 * @return pointer to dynamically allocated vector.
 * @if_fail return NULL.c
 */
vector *
vector_alloc (vector_elem_cpy elem_copy_func, vector_elem_cmp elem_cmp_func,
              vector_elem_free elem_free_func)
{
    vector *new_vector = malloc (sizeof (vector));
    if (new_vector == NULL || elem_copy_func == NULL || elem_cmp_func == NULL
        || elem_free_func == NULL)
        return NULL;
    new_vector->capacity = VECTOR_INITIAL_CAP;
    new_vector->size = 0;
    new_vector->data = malloc (new_vector->capacity * sizeof (void *));
    if (new_vector->data == NULL)
        {
            free (new_vector);
            new_vector = NULL;
            return NULL;
        }
    new_vector->elem_copy_func = elem_copy_func;
    new_vector->elem_cmp_func = elem_cmp_func;
    new_vector->elem_free_func = elem_free_func;
    return new_vector;
}

/**
 * Frees a vector and the elements the vector itself allocated.
 * @param p_vector pointer to dynamically allocated pointer to vector.
 */
void vector_free (vector **p_vector)
{
    if (p_vector == NULL)
        return;
    for (int i = 0; i < (*p_vector)->size; i++)
        {
            (*p_vector)->elem_free_func ((*p_vector)->data[i]);
        }
    free ((*p_vector)->data);
    free (*p_vector);
}

/**
 * Returns the element at the given index.
 * @param vector pointer to a vector.
 * @param ind the index of the element we want to get.
 * @return the element at the given index if exists (the element itself, not a copy of it),
 * NULL otherwise.
 */
void *vector_at (const vector *vector, size_t ind)
{
    if (vector == NULL || vector->data == NULL || vector->size >= ind)
        return NULL;
    else
        return vector->data[ind];
}

/**
 * Gets a value and checks if the value is in the vector.
 * @param vector a pointer to vector.
 * @param value the value to look for.
 * @return the index of the given value if it is in the vector ([0, vector_size - 1]).
 * Returns -1 if no such value in the vector.
 */
int vector_find (const vector *vector, const void *value)
{
    if (vector == NULL || vector->data == NULL || value == NULL)
        return -1;
    for (int i = 0; i < vector->size; ++i)
        {
            if (vector->elem_cmp_func (vector->data[i], value))
                return i;
        }
    return -1;
}

/**
 * Adds a new value to the back (index vector_size) of the vector.
 * @param vector a pointer to vector.
 * @param value the value to be added to the vector.
 * @return 1 if the adding has been done successfully, 0 otherwise.
 */
int vector_push_back (vector *vector, const void *value)
{
    if (vector == NULL || value == NULL)
        return 0;
    double load_factor = vector_get_load_factor (vector);
    if (load_factor > VECTOR_MAX_LOAD_FACTOR)
        {
            vector->capacity *= VECTOR_GROWTH_FACTOR;
            void **temp = (void **) realloc (vector->data,
                                             sizeof (void *)
                                             * vector->capacity);
            if (temp == NULL)
                {
                    vector_free (&vector);
                    return 0;
                }
            vector->data = temp;
        }
    vector->data[vector->size] = &value;
    vector->size++;
    return 1;
}

/**
 * This function returns the load factor of the vector.
 * @param vector a vector.
 * @return the vector's load factor, -1 if the function failed.
 */
double vector_get_load_factor (const vector *vector)
{
    if (vector == NULL || vector->capacity <= 0)
        return -1;
    else
        return (double) vector->size / vector->capacity;
}

/**
 * Removes the element at the given index from the vector. alters the indices of the remaining
 * elements so that there are no empty indices in the range [0, size-1] (inclusive).
 * @param vector a pointer to vector.
 * @param ind the index of the element to be removed.
 * @return 1 if the removing has been done successfully, 0 otherwise.
 */
int vector_erase (vector *vector, size_t ind)
{
    if (vector == NULL || vector->data == NULL || vector->size >= ind)
        return 0;
    vector->elem_free_func (vector->data[ind]);
    vector->data[ind] = NULL;
    for (unsigned int i = ind + 1; i < vector->size; ++i)
        {

        }

}

/**
 * Deletes all the elements in the vector.
 * @param vector vector a pointer to vector.
 */
void vector_clear (vector *vector)
{

}