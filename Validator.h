
#ifndef JSHUND_VALIDATOR_H
#define JSHUND_VALIDATOR_H 1

#include <unistd.h>
#include <string>
#include <vector>
#include <map>

#include "Token.h"

namespace jshund {
  class Validator {
    public:
      Validator (bool colors, bool globals) :
        _globals(globals) {

        if (colors && ::isatty(STDOUT_FILENO)) {
          _colorGlobal   = "\x1b[0;33m";
          _colorLocal    = "\x1b[0;31m";
          _colorEnd      = "\x1b[0m";
        }
      }

      ~Validator () {
      }

    private:
      
      struct Variable {
        Variable (int line, bool used) : 
          line(line), used(used) {
        };

        int     line;
        bool    used;
      };
        
      typedef enum {
        STATE_NOTHING,
        STATE_VARDECL,
        STATE_FUNCARG,
        STATE_FUNCBODY
      }
      StateEnum;
      
      struct State {
        State (StateEnum type, int line, int brackets) : 
          type(type), line(line), brackets(brackets), expectDeclaration(false) {
        };

        StateEnum                       type;
        int                             line;
        int                             brackets;
        std::map<std::string, Variable> variables;
        bool                            expectDeclaration;
      };

    public:
      int validate (std::string const& filename, std::vector<Token> const& tokens) {
        int errors = 0;
        int brackets = 0;

        State startState(STATE_NOTHING, 1, brackets);

        std::vector<State> states;
        states.push_back(startState);

        for (size_t i = 0, n = tokens.size(); i < n; ++i) {
          State& currentState = states[states.size() - 1];
          Token const& currentToken = tokens[i];

          if (currentToken.type == TOKEN_NAME) {
            if (i > 0 && tokens[i - 1].type == TOKEN_DOT) {
              // compound name
              continue;
            }

            const std::string name = currentToken.value;

            if (currentState.type == STATE_VARDECL && currentState.expectDeclaration) {
              states[states.size() - 2].variables.insert(std::pair<std::string, Variable>(name, Variable(currentToken.line, false)));

              Token next = tokens[i + 1];
              if (next.type == TOKEN_ASSIGN) {
                currentState.expectDeclaration = false;
              }
              else if (next.type == TOKEN_COMMA) {
                // skip one token
                ++i;
                currentState.expectDeclaration = true;
              }
            }
            else {
              size_t j = states.size() - 1;

              // cout << "FOUND USAGE OF VAR '" << name << "'" << endl;
              while (1) {
                State& s = states[j];

                std::map<std::string, Variable>::iterator it = s.variables.find(name);

                if (it != s.variables.end()) {
                  (*it).second.used = true;
                  break;
                }

                if (j-- == 0) {
                  break;
                }
              }
            }
          }

          else if (currentToken.type == TOKEN_FUNCTION) {
            Token next = tokens[i + 1];
            if (next.type == TOKEN_NAME) {
              // skip one token
              ++i;
            }
            
            State funcState(STATE_FUNCARG, currentToken.line, brackets);
            states.push_back(funcState);
          }
          
          else if (currentToken.type == TOKEN_VAR) {
            State varState(STATE_VARDECL, currentToken.line, brackets);
            varState.expectDeclaration = true;
            states.push_back(varState);
          }

          else if (currentToken.type == TOKEN_SEMICOLON) {
            if (currentState.type == STATE_VARDECL) {
              states.pop_back();
            }
          }
          
          else if (currentToken.type == TOKEN_PARENS_OPEN || 
                   currentToken.type == TOKEN_CURLY_OPEN || 
                   currentToken.type == TOKEN_ANGULAR_OPEN) {
            brackets++;
          }

          else if (currentToken.type == TOKEN_PARENS_CLOSE || 
                   currentToken.type == TOKEN_CURLY_CLOSE || 
                   currentToken.type == TOKEN_ANGULAR_CLOSE) {
            --brackets;
            
            if (currentState.type == STATE_FUNCARG && currentState.brackets == brackets) {
              // end of function argument list
              states.pop_back();
              
              State funcState(STATE_FUNCBODY, currentToken.line, brackets);
              states.push_back(funcState);
            }
            else if (currentState.type == STATE_FUNCBODY && currentState.brackets == brackets) {
              // end of function body
              std::map<std::string, Variable>::const_iterator it = currentState.variables.begin();

              while (it != currentState.variables.end()) {
                if (! (*it).second.used) {
                  reportError(filename, (*it).second.line, (*it).first, false);
                  ++errors;
                }
                ++it;
              }
              states.pop_back();
            }
          }

          else if (currentToken.type == TOKEN_EOF && _globals) {
            // end of file
            std::map<std::string, Variable>::const_iterator it = currentState.variables.begin();

            while (it != currentState.variables.end()) {
              if (! (*it).second.used) {
                reportError(filename, (*it).second.line, (*it).first, true);
                ++errors;
              }
              ++it;
            }
            states.pop_back();
          }
        }

        return errors;
      }

      void reportError (std::string const& filename, int line, std::string const& name, bool global) {
        std::cout << filename << ':' << line 
                  << (global ? _colorGlobal : _colorLocal) << ": unused " << (global ? "global" : "local") 
                  << " variable '" << name << '\'' << _colorEnd  
                  << std::endl;
      }

    private:

      std::string _colorGlobal;
      
      std::string _colorLocal;
      
      std::string _colorEnd;

      bool _globals;

  };
}

#endif

