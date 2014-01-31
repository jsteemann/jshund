
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
        std::map<std::string, Variable> declaredVariables;
        std::map<std::string, Variable> phantoms;
        bool                            expectDeclaration;
      };

    public:
      int validate (std::string const& filename, std::vector<Token> const& tokens) {
        int errors = 0;
        int brackets = 0;

        _filename = filename;

        State startState(STATE_NOTHING, 1, brackets);

        std::vector<State> states;
        states.push_back(startState);

        for (size_t i = 0, n = tokens.size(); i < n; ++i) {
          State& currentState = states[states.size() - 1];
          Token const& currentToken = tokens[i];
            
          if (currentState.type == STATE_VARDECL && 
              ! currentState.expectDeclaration && 
              currentToken.type == TOKEN_COMMA &&
              currentState.brackets == brackets) {
            // std::cout << "FOUND COMMA\n";
            currentState.expectDeclaration = true;
          }

          else if (currentToken.type == TOKEN_NAME) {
            if (i > 0 && tokens[i - 1].type == TOKEN_DOT) {
              // compound name, e.g. foo.bar
              //                         ^^^
              continue;
            }

            const std::string name = currentToken.value;

            if (currentState.type == STATE_FUNCARG) {
              // std::cout << "FOUND FUNCTION ARGUMENT '" << name << "'" << std::endl;

              currentState.declaredVariables.insert(std::pair<std::string, Variable>(name, Variable(currentToken.line, true)));
            }
            else if (currentState.type == STATE_VARDECL && currentState.expectDeclaration) {
              // std::cout << "FOUND DECLARATION OF VAR '" << name << "'" << std::endl;

              if (states.size() <= 1) {
                std::cerr << _filename << ':' << currentToken.line << ": encountered an invalid state during parsing" << std::endl;
                return 1;
              }

              bool wasPhantom = states[states.size() - 2].phantoms.erase(name);
              // if (wasPhantom) {
              //   std::cout << "IGNORING PHANTOM '" << name << "'\n";
              // }
              states[states.size() - 2].declaredVariables.insert(std::pair<std::string, Variable>(name, Variable(currentToken.line, wasPhantom)));

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

              // std::cout << "FOUND USAGE OF VAR '" << name << "'" << std::endl;
              while (1) {
                State& s = states[j];

                std::map<std::string, Variable>::iterator it = s.declaredVariables.find(name);

                if (it != s.declaredVariables.end()) {
                  (*it).second.used = true;
                  break;
                }

                if (j-- == 0) {
                  currentState.phantoms.insert(std::pair<std::string, Variable>(name, Variable(currentToken.line, false)));
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
              State funcState(STATE_FUNCBODY, currentToken.line, brackets);

              std::map<std::string, Variable>::const_iterator it = currentState.declaredVariables.begin();
              while (it != currentState.declaredVariables.end()) {
                // std::cout << "PUSHING FUNCTION ARG '" << (*it).first << "'" << std::endl;
                funcState.declaredVariables.insert(std::pair<std::string, Variable>((*it).first, (*it).second));
                ++it;
              }

              states.pop_back();
              states.push_back(funcState);
            }
            else if (currentState.type == STATE_FUNCBODY && currentState.brackets == brackets) {
              // end of function body
              std::map<std::string, Variable>::const_iterator it = currentState.declaredVariables.begin();

              while (it != currentState.declaredVariables.end()) {
                if (! (*it).second.used) {
                  reportError((*it).second.line, (*it).first, false);
                  ++errors;
                }
                ++it;
              }

              // insert phantoms
              if (states.size() > 1) {
                State& prevState = states[states.size() - 2];

                it = currentState.phantoms.begin();

                while (it != currentState.phantoms.end()) {
                  // std::cout << "PUSHING PHANTOM '" << (*it).first << "'" << std::endl;
                  prevState.phantoms.insert(std::pair<std::string, Variable>((*it).first, (*it).second));
                  ++it;
                }
              }
              states.pop_back();
            }
          }

          else if (currentToken.type == TOKEN_EOF && _globals) {
            // end of file
            std::map<std::string, Variable>::const_iterator it = currentState.declaredVariables.begin();

            while (it != currentState.declaredVariables.end()) {
              if (! (*it).second.used) {
                reportError((*it).second.line, (*it).first, true);
                ++errors;
              }
              ++it;
            }
            states.pop_back();
          }
        }

        return errors;
      }

      void reportError (int line, std::string const& name, bool global) {
        std::cout << _filename << ':' << line << ':'  
                  << (global ? _colorGlobal : _colorLocal) << " unused " << (global ? "global" : "local") 
                  << " variable '" << name << '\'' << _colorEnd  
                  << std::endl;
      }

    private:

      std::string _colorGlobal;
      
      std::string _colorLocal;
      
      std::string _colorEnd;

      std::string _filename;

      bool _globals;

  };
}

#endif

