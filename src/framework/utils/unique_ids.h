/*!\file unique_ids.h
** \author
** \copyright TODO
** \brief Template class for unique identifiers.
** \details This header provides mostly a template class to reserve
**          unique identifiers that can be used for various purposes.
**/
/****************************************************************/

#ifndef __UNIQUE_IDS_H
#define __UNIQUE_IDS_H

#include <unordered_set>
#include <limits>

/*!    \brief Failed to find a Unique ID.
**
** Unique ID search failed to find a value.
** All IDs have been taken.
**/
class FailedIDAllocation: public std::exception
{
public:
    const char *what(void)
    {
        return "Failed to find a free unique ID.";
    }
};

/*!    \brief Class template for Unique Identifiers.
**
** This template class allows to allocate and reserve generic
** purpose (typically) numeric identifiers. T will be the the
** underlining ID type.
**
** Once an ID is reserved, it won't be returned a second time
** by an instance of this class unless it is previously released.
**
** T is not required to be a numeric type but it should grow monotonically,
** support comparing, wrapping at maximum value and ++ operation.
**
** Implementation note: search from new values will each time start from
** the last allocation. This allows to have a means of somehow separating newer
** and older allocations. It may be useful for debugging.
** It will though require to store the last allocation and thus it is
** less efficient in terms of memory. It shouldn't be a problem.
**/
template<typename T>
class UniqueId
{
public:
    UniqueId(): lastAllocated {std::numeric_limits<T>::max()} {};

    /*!    \brief Reserve a new ID.
    **
    ** \return Reserved ID of type T.
    **
    ** \throws FailedIDAllocation if there are no IDs available.
    **
    ** Allocate and reserve a new unique ID.
    **/
    T alloc(void)
    {
        T startValue = lastAllocated;

        do
        {
            /* Search for a free ID starting from the one after
             * the last allocated one.
             * The loop is meant to roll-over and give up searching
             * when we reached back to the last allocation.
             */
            lastAllocated++;
            if(allocated.count(lastAllocated) == 0)
            {
                /*Free ID: mark it as allocated and return it. */
                allocated.insert(lastAllocated);
                return lastAllocated;
            }
        }
        while(lastAllocated != startValue);

        /* Failed to find a free ID.*/
        throw FailedIDAllocation();
    }

    /*!    \brief Release an ID.
    **
    ** \param[in] id - ID to release of type T.
    **
    ** Release a previously allocated ID and make it available
    ** for use. It won't raise any error if ID is already available.
    **/
    void release(T id)
    {
        allocated.erase(id);
    }

private:
    T lastAllocated;
    std::unordered_set<T> allocated;
};

#endif /* __UNIQUE_IDS_H */
