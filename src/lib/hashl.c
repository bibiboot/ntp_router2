#include "hashl.h"
#include "globals.h"

/**
 * @brief Add structure to the hashtable
 *        Key is the sequence number
 */
void add_to_hash(dict_node *h)
{
    HASH_ADD_INT( globals.hashl, key, h);
}

/**
 * @brief Find a structure based on the key
 *        which is a sequence number
 */
dict_node *find_in_hash(int key)
{
    dict_node *h;
    HASH_FIND_INT( globals.hashl, &key, h);
    return h;
}
