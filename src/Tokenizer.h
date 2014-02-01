
#ifndef JSHUND_TOKENIZER_H
#define JSHUND_TOKENIZER_H 1

#include <string.h>
#include <string>
#include <map>
#include <vector>

#include "Token.h"

namespace jshund {

  class Tokenizer {
    public:
      Tokenizer (bool debug) :
        _tokenMap(), _keywordMap(), _tokens(), _debug(debug) {

        _tokenMap["."]   = TOKEN_DOT;
        _tokenMap[","]   = TOKEN_COMMA;
        _tokenMap["{"]   = TOKEN_CURLY_OPEN;
        _tokenMap["}"]   = TOKEN_CURLY_CLOSE;
        _tokenMap["("]   = TOKEN_PARENS_OPEN;
        _tokenMap[")"]   = TOKEN_PARENS_CLOSE;
        _tokenMap["["]   = TOKEN_ANGULAR_OPEN;
        _tokenMap["]"]   = TOKEN_ANGULAR_CLOSE;
        _tokenMap[":"]   = TOKEN_COLON;
        _tokenMap["?"]   = TOKEN_QUESTION;
        _tokenMap[";"]   = TOKEN_SEMICOLON;
        _tokenMap["/*"]  = TOKEN_COMMENT_MULTI;
        _tokenMap["//"]  = TOKEN_COMMENT_SINGLE;
        _tokenMap["/="]  = TOKEN_DIV_ASSIGN;
        _tokenMap["/"]   = TOKEN_DIV;
        _tokenMap["++"]  = TOKEN_INCREMENT;
        _tokenMap["+="]  = TOKEN_PLUS_ASSIGN;
        _tokenMap["+"]   = TOKEN_PLUS;
        _tokenMap["--"]  = TOKEN_DECREMENT;
        _tokenMap["-="]  = TOKEN_MINUS_ASSIGN;
        _tokenMap["-"]   = TOKEN_MINUS;
        _tokenMap["*="]  = TOKEN_MUL_ASSIGN;
        _tokenMap["*"]   = TOKEN_MUL;
        _tokenMap["%="]  = TOKEN_MOD_ASSIGN;
        _tokenMap["%"]   = TOKEN_MOD;
        _tokenMap["!=="] = TOKEN_NE;
        _tokenMap["!="]  = TOKEN_NE;
        _tokenMap["!"]   = TOKEN_NOT;
        _tokenMap["&&="] = TOKEN_AND_ASSIGN;
        _tokenMap["&&"]  = TOKEN_AND;
        _tokenMap["&="]  = TOKEN_BIT_AND_ASSIGN;
        _tokenMap["&"]   = TOKEN_BIT_AND;
        _tokenMap["||="] = TOKEN_OR_ASSIGN;
        _tokenMap["||"]  = TOKEN_OR;
        _tokenMap["|="]  = TOKEN_BIT_OR_ASSIGN;
        _tokenMap["|"]   = TOKEN_BIT_OR;
        _tokenMap[">="]  = TOKEN_GE;
        _tokenMap[">"]   = TOKEN_GT;
        _tokenMap["<="]  = TOKEN_LE;
        _tokenMap["<"]   = TOKEN_LT;
        _tokenMap["==="] = TOKEN_IDENTITY;
        _tokenMap["=="]  = TOKEN_EQ;
        _tokenMap["="]   = TOKEN_ASSIGN;

        _keywordMap["true"]       = TOKEN_BOOL;
        _keywordMap["false"]      = TOKEN_BOOL;
        _keywordMap["null"]       = TOKEN_NULL;
        _keywordMap["undefined"]  = TOKEN_UNDEFINED;
        _keywordMap["case"]       = TOKEN_CASE;
        _keywordMap["default"]    = TOKEN_DEFAULT;
        _keywordMap["do"]         = TOKEN_DO;
        _keywordMap["else"]       = TOKEN_ELSE;
        _keywordMap["finally"]    = TOKEN_FINALLY;
        _keywordMap["if"]         = TOKEN_IF;
        _keywordMap["switch"]     = TOKEN_SWITCH;
        _keywordMap["this"]       = TOKEN_THIS;
        _keywordMap["void"]       = TOKEN_VOID;
        _keywordMap["while"]      = TOKEN_WHILE;
        _keywordMap["for"]        = TOKEN_FOR;
        _keywordMap["in"]         = TOKEN_IN;
        _keywordMap["with"]       = TOKEN_WITH;
        _keywordMap["new"]        = TOKEN_NEW;
        _keywordMap["break"]      = TOKEN_BREAK;
        _keywordMap["continue"]   = TOKEN_CONTINUE;
        _keywordMap["return"]     = TOKEN_RETURN;
        _keywordMap["throw"]      = TOKEN_THROW;
        _keywordMap["catch"]      = TOKEN_CATCH;
        _keywordMap["function"]   = TOKEN_FUNCTION;
        _keywordMap["var"]        = TOKEN_VAR;
        _keywordMap["delete"]     = TOKEN_DELETE;
        _keywordMap["typeof"]     = TOKEN_TYPEOF;
        _keywordMap["instanceof"] = TOKEN_INSTANCEOF;
      }

      ~Tokenizer () {
      }

      const std::vector<Token>& tokenize (char const* input) {
        _input    = input;
        _length   = ::strlen(input);
        _position = 0;
        _line     = 1; 
        _last     = TOKEN_UNKNOWN;

        _tokens.clear();

        doTokenize();

        if (_debug) {
          for (size_t i = 0; i < _tokens.size(); ++i) {
            _tokens[i].dump();
          }
        }

        return _tokens;
      }

    private:

      bool skipWhitespace () {
        while (true) {
          const char c = _input[_position];
          
          if (c == '\0') {
            return false;
          }

          if (c != ' ' && c != '\r' && c != '\n' && c != '\t' && c != '\f' && c != '\b') {
            return true;
          }

          if (c == '\r') {
            if (_input[_position + 1] == '\n') {
              ++_position;
              ++_line;
            }
          }
          else if (c == '\n') {
            ++_line;
          }

          _position++;
        }
      }

      Token handleString (const char terminator) {
        size_t now = _position + 1;

        while (true) {
          char n = _input[now];
          if (n == '\\') {
            ++now;
          }
          else if (n == terminator) {
            return makeToken(TOKEN_STRING_LITERAL, until(now + 1));
          }
          else if (n == '\0') {
            return makeToken(TOKEN_UNKNOWN);
          }
          ++now;
        }
      }
      
      Token handleNumber () {
        size_t now = _position;
        bool dotFound = false;
        bool expFound = false;
        
        while (true) {
          char n = _input[now];
          if (n >= '0' && n <= '9') {
            ++now;
          }
          else if (n == '.' && ! dotFound) {
            dotFound = true;
            ++now;
          }
          else if ((n == 'e' || n == 'E') && ! expFound) {
            // exponent
            expFound = true;
            ++now;
          }
          else {
            break;
          }
        }

        return makeToken(TOKEN_NUMBER, until(now));
      }

      Token handleRegex () {
        size_t now = _position + 1;
       /* 
    if (!/^[\\\/]{2}[^\\\/]/.test(paths[0])) {
328         joined = joined.replace(/^[\\\/]{2,}/, '\\');
329       }
330 
*/

        while (true) {
          char n = _input[now];
          if (n == '\\') {
            ++now;
          }
          else if (n == '/') {
            ++now;
            break;
          }
          else if (n == '\0') {
            return makeToken(TOKEN_UNKNOWN);
          }
          ++now;
        }

        char n = _input[now];
        // modifier
        while (n >= 'a' && n <= 'z') {
          n = _input[++now];
        }

        //std::cout << "found regex " << std::string(_input + _position, now - _position) << std::endl;
        return makeToken(TOKEN_REGEX, until(now));
      }

      Token handleId () {
        // name/keyword
        std::string name;
        bool canBeKeyword = true;
        size_t now = _position;
        
        while (true) {
          char n = _input[now];

          if (n >= 'a' && n <= 'z') {
            name.push_back(n);
          } 
          else if (n >= 'A' && n <= 'Z') {
            name.push_back(n);
            canBeKeyword = false;
          }
          else if (n >= '0' && n <= '9') {
            name.push_back(n);
            canBeKeyword = false;
          }
          else if (n == '_' || n == '$') {
            name.push_back(n);
          }
          else {
            break;
          }
          ++now;
        }
          
        if (canBeKeyword) {
          std::map<std::string, TokenType>::const_iterator it = _keywordMap.find(name);
          if (it != _keywordMap.end()) {
            return makeToken((*it).second, std::string(_input + _position, name.size()));
          }
        }
          
        return makeToken(TOKEN_NAME, std::string(_input + _position, name.size()));
      }

      Token handleCommentMulti () {
        size_t now = _position + 2;
        
        while (true) {
          char n1 = _input[now];
          char n2 = _input[now + 1];

          if (n1 == '\r' && n1 == '\n') {
            ++_line;
            now += 2;
          }
          else if (n1 == '\r' || n1 == '\n') {
            ++_line;
            ++now;
          }
          else if (n1 == '*' && n2 == '/') {
            return makeToken(TOKEN_COMMENT_MULTI, until(now + 2));
          }
          else if (n1 == '\0') {
            return makeToken(TOKEN_UNKNOWN);
          }
          else {
            ++now;
          }
        }
      }
      
      Token handleCommentSingle () {
        size_t now = _position + 2;

        while (true) {
          char n = _input[now];

          if (n == '\r' || n == '\n' || n == '\0') {
            return makeToken(TOKEN_COMMENT_SINGLE, until(now));
          }
          else {
            ++now;
          }
        }
      }
        
      Token getNext () {
        if (_position >= _length) {
          return makeToken(TOKEN_EOF);
        }

        if (! skipWhitespace()) {
          makeToken(TOKEN_EOF);
        }
        
        const char c = _input[_position];

        if (c == '\'') {
          return handleString('\'');
        }
        else if (c == '"') {
          return handleString('"');
        }
        else if ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || (c == '_' || c == '$')) {
          return handleId();
        }
        else if (c >= '0' && c <= '9') {
          return handleNumber();
        }
        else if (c == '.') {
          const char c2 = _input[_position + 1];
          if (c2 >= '0' && c2 <= '9') {
            return handleNumber();
          }
        }
        else if (c == '/') {
          const char c2 = _input[_position + 1];
          if (c2 == '*') {
            return handleCommentMulti();
          }
          else if (c2 == '/') {
            return handleCommentSingle();
          }
          if (_last == TOKEN_PARENS_OPEN || 
              _last == TOKEN_ASSIGN || 
              _last == TOKEN_COLON || 
              _last == TOKEN_COMMA ||
              _last == TOKEN_NOT) {
            return handleRegex();
          }
        }

        TokenMapType::const_iterator it;
        for (it = _tokenMap.begin(); it != _tokenMap.end(); ++it) {
          const size_t length = (*it).first.size();
          if (::memcmp(_input + _position, (*it).first.c_str(), length) == 0) {
            // found
            return makeToken((*it).second, (*it).first);
          }
        }

        return makeToken(TOKEN_UNKNOWN);
      }

      void doTokenize () {
        while (true) {
          Token token = getNext();
          if (token.type == TOKEN_EOF || token.type == TOKEN_UNKNOWN) {
            break;
          }

          addToken(token);
        }

        addToken(makeToken(TOKEN_EOF));
      }

      Token makeToken (TokenType type) {
        Token token;
        token.type  = type;
        token.line  = _line;
        token.value = "";

        ++_position;

        return token;
      }

      Token makeToken (TokenType type, std::string const& v) {
        Token token;
        token.type  = type;
        token.line  = _line;
        token.value = v;
        
        _position += v.size();

        //std::cout << "token: " << type << "," << v << std::endl;

        return token;
      }

      std::string until (const size_t end) {
        return std::string(_input + _position, end - _position);
      }

      void addToken (Token token) {
        _tokens.push_back(token);
        _last = token.type;
      }
     
      typedef std::map<std::string, TokenType> TokenMapType;

      TokenMapType _tokenMap;
      
      std::map<std::string, TokenType> _keywordMap;

      std::vector<Token> _tokens;

      TokenType _last;

      char const* _input;

      size_t _length;

      size_t _position;

      size_t _line;

      bool _debug;

  };
}

#endif

