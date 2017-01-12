#ifndef INC_ExitException_hpp__
#define INC_ExitException_hpp__

#include <antlr/config.hpp>
#include <antlr/ANTLRException.hpp>

#ifdef ANTLR_CXX_SUPPORTS_NAMESPACE
namespace antlr {
#endif

/** Baseclass for exceptions thrown by classes implementing the Exit
 * interface.
 * @see Exit
 */
class ANTLR_API ExitException : public ANTLRException {
public:
	ExitException() 
	: ANTLRException()	
	{
	}
	ExitException(const ANTLR_USE_NAMESPACE(std)string& s)
	: ANTLRException(s)
	{
	}
	virtual ~ExitException() throw()
	{
	}
};

#ifdef ANTLR_CXX_SUPPORTS_NAMESPACE
}
#endif

#endif //INC_ExitException_hpp__
