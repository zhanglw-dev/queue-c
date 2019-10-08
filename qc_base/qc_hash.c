#include "qc_prelude.h"
#include "qc_thread.h"
#include "qc_hash.h"


/*////////////////////////////////////////////////////////////////////////////////
//                               Hash Table
////////////////////////////////////////////////////////////////////////////////*/


struct __HashEntry{
    HashEntry *next;
    void *key;
    void *data;
};


struct __QcHashTbl{
    HashEntry **pp_bucket;
    unsigned int bucket_count;
    unsigned int entry_count;
    unsigned int cur_bucket_no;
    HashEntry *cur_entry;
    QcRWLock *rwlock;
    HASH_HASHFUNC HashFunc;
    HASH_COMPAREFUNC CompareFunc;
    HASH_DESTROYFUNC DestroyFunc;
};



QcHashTbl* qc_hashtbl_create(unsigned int bucket_count, int with_rwlock, HASH_HASHFUNC HashFunc, \
                             HASH_COMPAREFUNC CompareFunc, HASH_DESTROYFUNC DestroyFunc)
{
    QcHashTbl *hashtbl;

    if(0 == bucket_count)
    {
        return NULL;
    }

    qc_malloc(hashtbl, sizeof(QcHashTbl));
    if(NULL == hashtbl)
    {
        return NULL;
    }

    hashtbl->entry_count = 0;
    hashtbl->bucket_count = bucket_count;
    hashtbl->rwlock = qc_thread_rwlock_create();
    if(NULL == hashtbl->rwlock)
    {
        qc_free(hashtbl);
        hashtbl = NULL;
        return NULL;
    }

    qc_malloc(hashtbl->pp_bucket, bucket_count*sizeof(HashEntry *));
    if(NULL == hashtbl->pp_bucket)
    {
        if(NULL != hashtbl)
		{
			qc_free(hashtbl);
			hashtbl = NULL;
		}
        return NULL;
    }

    memset(hashtbl->pp_bucket, 0, bucket_count * sizeof(HashEntry *));

    hashtbl->cur_entry = NULL;
    hashtbl->cur_bucket_no = 0;
    hashtbl->HashFunc = HashFunc;
    hashtbl->CompareFunc = CompareFunc;
    hashtbl->DestroyFunc = DestroyFunc;

    return hashtbl;
}


void qc_hashtbl_destroy( QcHashTbl *hashtbl)
{
    HashEntry **pp_bucket;
    HashEntry  *entry;
    HashEntry  *free_entry;
    unsigned int i;

    qc_assert(hashtbl);

    pp_bucket = hashtbl->pp_bucket;
    for(i = 0; i < hashtbl->bucket_count; i++)
    {
        entry = pp_bucket[i];
        while(NULL != entry)
        {
            if(NULL != hashtbl->DestroyFunc)
            {
                (*hashtbl->DestroyFunc)(entry->key, entry->data);
            }
            free_entry = entry;
            entry = entry->next;

            qc_free(free_entry);
			free_entry = NULL;
        }
    }
	if(NULL != pp_bucket)
	{
		qc_free(pp_bucket);
		pp_bucket = NULL;
	}

    hashtbl->pp_bucket = NULL;

    if(hashtbl->rwlock)
        qc_thread_rwlock_destroy(hashtbl->rwlock);

	qc_free(hashtbl);
    hashtbl = NULL;
}


int qc_hashtbl_count(QcHashTbl *hashtbl)
{
    return hashtbl->entry_count;
}


int	qc_hashtbl_insert(QcHashTbl *hashtbl, void *key, void *data)
{
    unsigned int index;
    HashEntry	*entry;
    HashEntry	*new_entry;

    qc_assert(hashtbl);
    qc_assert(key);
    qc_assert(data);

    index = (*hashtbl->HashFunc)(key, hashtbl->bucket_count);
    entry = (hashtbl->pp_bucket)[index];

    qc_malloc(new_entry, sizeof(HashEntry));

    if(NULL == new_entry)
    {
	    return -1;
    }
	memset(new_entry, 0, sizeof(HashEntry));
    new_entry->key  = key;
    new_entry->data = data;
    new_entry->next = entry;

    (hashtbl->pp_bucket)[index] = new_entry;
    hashtbl->entry_count += 1;

    return 0;
}


void* qc_hashtbl_find(QcHashTbl *hashtbl, void *key)
{
	unsigned int index;
	HashEntry* entry;

    qc_assert(hashtbl);
    qc_assert(key);

	index = (*hashtbl->HashFunc)(key, hashtbl->bucket_count);
	entry = (hashtbl->pp_bucket)[index];

	while(NULL != entry)
	{
		if(0 == (*hashtbl->CompareFunc)(entry->key, key))
		{
			return entry->data;
		}
		entry = entry->next;
	}

    return NULL;
}


int qc_hashtbl_delete(QcHashTbl *hashtbl, void *key)
{
	unsigned int index;
	HashEntry* entry;
	HashEntry* prev_entry;

    qc_assert(hashtbl);
    qc_assert(key);

	index = (*hashtbl->HashFunc)(key, hashtbl->bucket_count);
	entry = (hashtbl->pp_bucket)[index];
    prev_entry = entry;

    while(NULL != entry)
    {
        if((*hashtbl->CompareFunc)(entry->key, key) == 0)
        {
            if(prev_entry == entry)
            {
                hashtbl->pp_bucket[index] = entry->next;
            }
            else
            {
                prev_entry->next = entry->next;
            }

            if(NULL != hashtbl->DestroyFunc)
            {
                (*hashtbl->DestroyFunc)(entry->key, entry->data);
            }
            if(NULL != entry)
			{
				qc_free(entry);
				entry = NULL;
			}

            hashtbl->entry_count -= 1;

            return 1;
        }

        prev_entry = entry;
        entry = entry->next;
    }

    return 0;
}


void qc_hashtbl_enumbegin(QcHashTbl *hashtbl)
{
    hashtbl->cur_bucket_no = 0;
    hashtbl->cur_entry = hashtbl->pp_bucket[0];
}


void* qc_hashtbl_enumnext(QcHashTbl *hashtbl)
{
    void *data;

	while(NULL == hashtbl->cur_entry)
	{
		hashtbl->cur_bucket_no += 1;
		if(hashtbl->cur_bucket_no >= hashtbl->bucket_count)
		{
			return NULL;
		}
		hashtbl->cur_entry = hashtbl->pp_bucket[hashtbl->cur_bucket_no];
	}

	data = hashtbl->cur_entry->data;

    hashtbl->cur_entry = hashtbl->cur_entry->next;

    return data;
}

/*
unsigned int qc_hashint(void *key, unsigned int bucket_count)
{
	return ((long)key) % (long)bucket_count;
}
*/

unsigned int qc_hashstring(void *str, unsigned int bucket_count)
{
    unsigned char *psz;
    unsigned int hash_value;
    unsigned int ret;
    unsigned int i;

    psz = (unsigned char *)str;
    hash_value = 0;

    i = 0;
    ret = 0;
    while(*psz != '\0')
    {
	    if(i == 5)
	    {
		    i = 0;
		    ret += hash_value;
		    hash_value = 0;
	    }
	    hash_value += hash_value << 3;
	    hash_value += (unsigned int)tolower(*psz);
	    psz++;
	    i++;
    }

    ret += hash_value;

    return ret % bucket_count;
}


/*
 *	HashBin( void *str )
 *	Calculate the hash value of a string.
 *	Parameters:
 *		void *str,		the string that need calculate.
 *	Return Values:
 *		the hash value of the string.
 */
unsigned int qc_hashbin(void *key, unsigned int uLength, unsigned int bucket_count)
{
    unsigned char *psz;
    unsigned int hash_value;
    unsigned int ret;
    unsigned int i;
    unsigned int j;

    psz = (unsigned char *)key;
    hash_value = 0;

    j = 0;
    ret = 0;
    i = 0;
    while(i < uLength)
    {
	    if(j == 5)
	    {
		    j = 0;
		    ret += hash_value;
		    hash_value = 0;
	    }
	    hash_value += hash_value << 3;
	    hash_value += (unsigned int)tolower( *psz );
	    psz++;
	    j++;
	    i++;
    }

    ret += hash_value;

    return ret % bucket_count;
}


/*
 *	BinCompare( )
 *	Compare if two binary key is equal.
 *	Return Values:
 *		1		equal
 *		0		not equal
 */
unsigned int qc_bincompare( void *str1, int str1_len, void *str2, int str2_len )
{
	char *psz1, *psz2;
	int	i;

	if(str1_len != str2_len)
	{
		return 0;
	}

	psz1 = (char *)str1;
	psz2 = (char *)str2;

	i = 0;
	while(i < str1_len)
	{
		if(*psz1 != *psz2)
		{
			return 0;
		}
		psz1++;
		psz2++;
		i++;
	}

	return 1;
}


/*
 *	StrCompare( )
 *	Compare if two string is equal.
 *	Return Values:
 *	 1  str1 greater than str2
 *	 0  str1 equal to str2
 *  -1  str1 less than str2
 */
int qc_strcompare( void *str1, void *str2 )
{
#ifdef IS_WINDOWS
	return _stricmp((char *)str1, (char *)str2);
#else
	/*return stricmp((char *)str1, (char *)str2);*/
    return strcasecmp((char *)str1, (char *)str2); /*to Linux*/

#endif
}


void qc_hashtbl_w_lock(QcHashTbl *hashtbl)
{
    if(NULL != hashtbl->rwlock)
    {
        qc_thread_wrlock_lock(hashtbl->rwlock);
    }
}


void qc_hashtbl_w_unlock(QcHashTbl *hashtbl)
{
    if(NULL != hashtbl->rwlock)
    {
        qc_thread_wrlock_unlock(hashtbl->rwlock);
    }
}


void qc_hashtbl_r_lock(QcHashTbl *hashtbl)
{
    if(NULL != hashtbl->rwlock)
    {
        qc_thread_rdlock_lock(hashtbl->rwlock);
    }
}


void qc_hashtbl_r_unlock(QcHashTbl *hashtbl)
{
    if(NULL != hashtbl->rwlock)
    {
        qc_thread_rdlock_unlock(hashtbl->rwlock);
    }
}
