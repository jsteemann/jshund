
#ifndef JSHUND_TOKEN_H
#define JSHUND_TOKEN_H 1

namespace jshund {
  using namespace std;

  enum TokenType {
    TOKEN_UNKNOWN = 0,

    TOKEN_STRING_LITERAL = 1, //
    TOKEN_NUMBER,
    TOKEN_BOOL, //
    TOKEN_REGEX,
    TOKEN_NULL, //
    TOKEN_UNDEFINED, //
    

    TOKEN_BREAK = 10,
    TOKEN_CASE,
    TOKEN_DEFAULT,
    TOKEN_DO,
    TOKEN_ELSE,
    TOKEN_FINALLY,
    TOKEN_IF,
    TOKEN_SWITCH,
    TOKEN_THIS,
    TOKEN_VOID,
    TOKEN_WHILE,
    TOKEN_CONTINUE,
    TOKEN_RETURN,
    TOKEN_THROW,
    TOKEN_CATCH,
    TOKEN_FUNCTION,
    TOKEN_FOR,
    TOKEN_IN,
    TOKEN_WITH,
    TOKEN_VAR,
    TOKEN_NEW,
    TOKEN_DELETE,
    TOKEN_TYPEOF,
    TOKEN_INSTANCEOF,

    TOKEN_DOT = 50, //
    TOKEN_COMMA, //
    TOKEN_CURLY_OPEN, //
    TOKEN_CURLY_CLOSE, //
    TOKEN_PARENS_OPEN, //
    TOKEN_PARENS_CLOSE, //
    TOKEN_ANGULAR_OPEN, //
    TOKEN_ANGULAR_CLOSE, //
    TOKEN_COLON, //
    TOKEN_QUESTION, //
    TOKEN_SEMICOLON, //
    
    TOKEN_COMMENT_SINGLE = 70, //
    TOKEN_COMMENT_MULTI, //

    TOKEN_NAME = 75, //

    TOKEN_EQ = 80, //
    TOKEN_NE, //
    TOKEN_GT, //
    TOKEN_GE, // 
    TOKEN_LT, //
    TOKEN_LE, //
    TOKEN_IDENTITY,
    TOKEN_AND, //
    TOKEN_OR, //
    TOKEN_NOT, //
    TOKEN_PLUS, //
    TOKEN_MINUS, //
    TOKEN_MUL, //
    TOKEN_DIV, //
    TOKEN_MOD, //
    
    TOKEN_ASSIGN = 100, //
    TOKEN_PLUS_ASSIGN, //
    TOKEN_MINUS_ASSIGN, //
    TOKEN_MUL_ASSIGN, //
    TOKEN_DIV_ASSIGN, //
    TOKEN_MOD_ASSIGN, //
    TOKEN_AND_ASSIGN, //
    TOKEN_OR_ASSIGN, //
    
    TOKEN_BIT_AND, //
    TOKEN_BIT_AND_ASSIGN, //
    TOKEN_BIT_OR, //
    TOKEN_BIT_OR_ASSIGN, //

    TOKEN_INCREMENT, //
    TOKEN_DECREMENT, //

    TOKEN_EOF
  };

  struct Token {
    TokenType type;
    string    value;
    size_t    line;
  };
}

#endif

