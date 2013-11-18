
#ifndef JSHUND_VALIDATOR_H
#define JSHUND_VALIDATOR_H 1

#include <string>
#include <vector>
#include <map>

#include "Token.h"

namespace jshund {
  using namespace std;

  class Validator {
    public:
      Validator () {
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

        StateEnum             type;
        int                   line;
        int                   brackets;
        map<string, Variable> variables;
        bool                  expectDeclaration;
      };

    public:
      bool validate (const string& filename, const vector<Token>& tokens) {
        int errors = 0;
        int brackets = 0;

        State startState(STATE_NOTHING, 1, brackets);

        vector<State> states;
        states.push_back(startState);

        for (size_t i = 0, n = tokens.size(); i < n; ++i) {
          State& currentState = states[states.size() - 1];
          Token const& currentToken = tokens[i];

          if (currentToken.type == TOKEN_NAME) {
            if (i > 0 && tokens[i - 1].type == TOKEN_DOT) {
              // compound name
              continue;
            }

            const string name = currentToken.value;

            if (currentState.type == STATE_VARDECL && currentState.expectDeclaration) {
              states[states.size() - 2].variables.insert(pair<string, Variable>(name, Variable(currentToken.line, false)));

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

                map<string, Variable>::iterator it = s.variables.find(name);
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
              map<string, Variable>::const_iterator it = currentState.variables.begin();

              while (it != currentState.variables.end()) {
                if (! (*it).second.used) {
                  cerr << filename << ":" << (*it).second.line << ": unused local variable '" << (*it).first << "'" << endl;
                  ++errors;
                }
                ++it;
              }
              states.pop_back();
            }
          }

          else if (currentToken.type == TOKEN_EOF) {
            // end of file
            map<string, Variable>::const_iterator it = currentState.variables.begin();

            while (it != currentState.variables.end()) {
              if (! (*it).second.used) {
                cerr << filename << ":" << (*it).second.line << ": unused global variable '" << (*it).first << "'" << endl;
                ++errors;
              }
              ++it;
            }
            states.pop_back();
          }
        }

        return (errors == 0);
      }

  };
}

#endif

