#ifndef NCAP_INVOKE_HH
#define NCAP_INVOKE_HH

class ncoLexer;
class ncoParser;
// Define selector that handles nested include files
// These variables are public so the parser/lexer can see them
extern ANTLR_USE_NAMESPACE(antlr)TokenStreamSelector selector;
extern ncoParser *parser;
extern ncoLexer *lexer;

#endif // NCAP_INVOKE_HH
