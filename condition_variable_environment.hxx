/*!
 * \file
 * \brief Defines the interface to the condition_variable_environment class.
 */
#ifndef CONDITION_VARIABLE_ENVIRONMENT_HXX_HDCY91AV
#define CONDITION_VARIABLE_ENVIRONMENT_HXX_HDCY91AV

#include "label.hxx"

/*!
 * \class condition_variable_environment
 * \brief Encapsulates an environment for the use of condition variables in
 *  nested function calls.
 *
 * This environment should be instantiated for each call to a function f
 * is called inside a transaction if f could wait in any of its sub-calls.
 * We term this function f the <em>goal function</em> of a particular
 * environment.
 *
 * \author Andres Jaan Tack (tack@cs.wisc.edu)
 */
class condition_variable_environment
{
public:
	condition_variable_environment ();

	/*!
	 * \brief Determines whether or not the environment has been activated.
	 *
	 * If the environment is active, then a wait should be performed, followed by
	 * a second call to the goal function.
	 */
	inline bool active() const	{ return itIsActive; }

	/*!
	 * \brief Activate the environment. See active().
	 */
	inline void activate(label innerJumpPoint)	{
		itIsActive = true;
		inner.jumpPoint = innerJumpPoint;
	}
	inline void deactivate()   { itIsActive = false; }

private:
	struct frame
	{
		frame();
		
		label stackBase;
		label jumpPoint;
	};

public:
	frame inner;
	frame outer;
	
private:
	bool itIsActive;
};

#endif /* end of include guard: CONDITION_VARIABLE_ENVIRONMENT_HXX_HDCY91AV */
