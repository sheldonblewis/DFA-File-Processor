#include <iostream>
#include <vector>
#include <string>
#include <sstream>
#include <cctype>
#include <algorithm>

using namespace std;

struct Transition {
    string from;
    vector<string> symbols;
    string to;
};

bool symbol_matches(const string& symbol, char c) {
    // Handle ranges like a-z, A-Z, and single characters
    if (symbol.size() == 3 && symbol[1] == '-') {
        return c >= symbol[0] && c <= symbol[2];
    }
    return symbol == string(1, c);
}

bool is_accepting_state(const string& state, const vector<string>& states) {
    string base_state = state;
    if (!base_state.empty() && base_state.back() == '!') {
        base_state.pop_back();
    }
    return find(states.begin(), states.end(), base_state + '!') != states.end();
}

string tokenize(const string& input, const vector<string>& states, const vector<Transition>& transitions) {
    size_t index = 0;
    string token;
    string current_state = states.front();
    string last_accepting_state;
    size_t last_accepting_pos = string::npos;
    string last_valid_token;

    while (index < input.size()) {
        bool transition_found = false;
        char current_char = input[index];

        // Debug output for current state and input character
        // cerr << "Current state: " << current_state << ", reading character: '" << current_char << "'" << endl;

        // Search for valid transitions
        for (const Transition& t : transitions) {
            string base_state = current_state;
            if (!base_state.empty() && base_state.back() == '!') {
                base_state.pop_back();
            }

            if (t.from == current_state || t.from == base_state) {
                // cerr << "Checking transition from state: " << t.from << endl;
                for (const string& sym : t.symbols) {
                    // cerr << "Comparing symbol: " << sym << " with character: '" << current_char << "'" << endl;
                    if (symbol_matches(sym, current_char)) {
                        current_state = t.to;
                        token += current_char;
                        transition_found = true;
                        // cerr << "Transition found to state: " << current_state << endl;

                        // Check if the new state is accepting
                        if (is_accepting_state(current_state, states)) {
                            last_accepting_state = current_state;
                            last_accepting_pos = index;
                            last_valid_token = token;
                            // cerr << "Accepting state reached: " << current_state << endl;
                        }

                        break;
                    }
                }
                if (transition_found) break;
            }
        }

        if (!transition_found) {
            if (last_accepting_pos != string::npos) {
                // Output the last valid token
                cout << last_valid_token << endl;
                index = last_accepting_pos + 1;
                current_state = states.front();
                token.clear();
                last_accepting_pos = string::npos;
            } else {
                // cerr << "ERROR: No valid token found at position " << index << endl;
                cout.flush();
                cerr.flush();
                // cerr << "ERROR" << endl;
                return "ERROR";
            }
        } else {
            index++;
        }
    }

    // If the loop ends and the current state is not accepting, output an error
    if (token.size() > 0 && !is_accepting_state(current_state, states)) {
        cerr << "ERROR: Reached end of input without being in an accepting state." << endl;
        cerr << "ERROR" << endl;
        return "ERROR";
    }

    // Output any remaining valid token at the end
    if (last_accepting_pos != string::npos && !last_valid_token.empty()) {
        cout << last_valid_token << endl;
    }
    return "SUCCESS";
}

int main() {
    vector<string> states;
    vector<Transition> transitions;
    string input;

    string line;

    // Read .STATES section
    while (getline(cin, line)) {
        if (line == ".STATES") break;
    }
    while (getline(cin, line) && line != ".TRANSITIONS") {
        if (!line.empty()) {
            istringstream iss(line);
            string state;
            while (iss >> state) {
                states.push_back(state);
            }
        }
    }

    // Read .TRANSITIONS section
    while (getline(cin, line) && line != ".INPUT") {
        if (!line.empty()) {
            istringstream iss(line);
            string from, to;
            vector<string> symbols;
            iss >> from;

            string token;
            while (iss >> token) {
                if (iss.peek() == EOF) {
                    to = token;  // Last token is the to-state
                    break;
                } else {
                    symbols.push_back(token);  // Add symbol to the list
                }
            }
            transitions.push_back({from, symbols, to});
        }
    }

    // Read .INPUT section
    while (getline(cin, line)) {
        if (!line.empty()) {
            input += line;
        }
    }

    // Perform tokenization using Simplified Maximal Munch
    if (input.empty() || tokenize(input, states, transitions) == "ERROR") {
        cerr << "ERROR: Tokenization failed." << endl;
    }

    return 0;
}
