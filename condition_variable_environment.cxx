/*!
 * \file
 * \brief Implements the condition_variable_environment class.
 * \author Andres Jaan Tack (tack@cs.wisc.edu)
 */
#include "condition_variable_environment.hxx"
#include <cstddef>

condition_variable_environment::condition_variable_environment() :
	itIsActive(false)
{	}

condition_variable_environment::frame::frame() :
	stackBase(NULL), jumpPoint(NULL)
{	}
