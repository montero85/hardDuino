/*!\file timestamp.h
** \author 
** \copyright TODO
** \brief Concrete data for clocks and timestamps
** \details Define a concrete types with the full set of operations for things
**          like timestamps or clock ticks. Data type behaviour is modeled around the
**          behaviour of a rolling counter that increments, reaches a maximum and then wraps.
**          This module provides all (or most all) the necessary operations to sum, subtract,
**          compare timestamps with other timestamps or timestamps with unsigned integers.
**/
/****************************************************************/
#ifndef __TIMESTAMP_H
#define __TIMESTAMP_H

#include <stdint.h>
/****************************************************************/

/* TODO: stub -> Implement proper timestamp class */
template<typename T>
using timestamp = T;


/****************************************************************/
#endif /* __TIMESTAMP_H */
/****************************************************************/
