#include <iostream>
#include <vector>
#include <string>
#include <sstream>
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

vector<string> split_inputs(const string& line) {
    vector<string> inputs;
    istringstream iss(line);
    string input;
    while (iss >> input) {
        inputs.push_back(input == ".EMPTY" ? "" : input);
    }
    return inputs;
}

vector<string> split_states(const string& line) {
    vector<string> states;
    istringstream iss(line);
    string state;
    while (iss >> state) {
        states.push_back(state);
    }
    return states;
}

bool is_accepting_state(const string& state, const vector<string>& states) {
    // Debugging: output checks for accepting state
    // cerr << "Checking if state is accepting: " << state << endl;
    bool state_has_exclamation = !state.empty() && state.back() == '!';
    bool state_in_list_with_exclamation = find(states.begin(), states.end(), state + '!') != states.end();

    // cerr << "State has '!': " << state_has_exclamation << endl;
    // cerr << "State exists in list with '!': " << state_in_list_with_exclamation << endl;

    return state_has_exclamation || state_in_list_with_exclamation;
}

int main() {
    vector<string> states;
    vector<Transition> transitions;
    vector<string> inputs;

    string line;

    // Read .STATES section
    while (getline(cin, line)) {
        if (line == ".STATES") break;
    }
    while (getline(cin, line) && line != ".TRANSITIONS") {
        if (!line.empty()) {
            vector<string> line_states = split_states(line);
            states.insert(states.end(), line_states.begin(), line_states.end());
        }
    }

    // Debugging: Output parsed states
    // cerr << "Parsed States:\n";
    // for (const string& state : states) {
    //     cerr << state << endl;
    // }

    // Read .TRANSITIONS section
    while (getline(cin, line) && line != ".INPUT") {
        if (!line.empty()) {
            istringstream iss(line);
            string from, to;
            vector<string> symbols;

            // Read from-state
            iss >> from;

            // Read symbols until the last token, which is the to-state
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

    // Debugging: Output parsed transitions
    // cerr << "\nParsed Transitions:\n";
    // for (const Transition& t : transitions) {
    //     cerr << t.from << " -> [";
    //     for (const string& sym : t.symbols) {
    //         cerr << sym << ", ";
    //     }
    //     cerr << "] -> " << t.to << endl;
    // }

    // Read .INPUT section
    while (getline(cin, line)) {
        if (!line.empty()) {
            vector<string> line_inputs = split_inputs(line);
            inputs.insert(inputs.end(), line_inputs.begin(), line_inputs.end());
        }
    }

    // Debugging: Output parsed inputs
    // cerr << "\nParsed Inputs:\n";
    // for (const string& input : inputs) {
    //     cerr << input << endl;
    // }

    // Process each input
    for (const string& input : inputs) {
        string current_state = states.front();
        bool valid = true;

        // cerr << "\nProcessing input: " << input << endl;
        // cerr << "Starting at state: " << current_state << endl;

        for (char c : input) {
            bool transition_found = false;
            // cerr << "Reading character: " << c << endl;
            for (const Transition& t : transitions) {
                string base_state = current_state;
                if (!current_state.empty() && current_state.back() == '!') {
                    base_state.pop_back();
                }

                if (t.from == current_state || t.from == base_state) {
                    // cerr << "Checking transition from state: " << t.from << endl;
                    for (const string& sym : t.symbols) {
                        // cerr << "Comparing symbol: " << sym << " with character: " << c << endl;
                        if (symbol_matches(sym, c)) {
                            current_state = t.to;
                            // cerr << "Transition found to state: " << current_state << endl;
                            transition_found = true;
                            break;
                        }
                    }
                    if (transition_found) break;
                }
            }
            if (!transition_found) {
                // cerr << "No transition found for character: " << c << endl;
                valid = false;
                break;
            }
        }

        // Check if the current state is accepting
        // cerr << "Final state after processing: " << current_state << endl;
        if (valid && is_accepting_state(current_state, states)) {
            cout << (input.empty() ? ".EMPTY" : input) << " true" << endl;
        } else {
            cout << (input.empty() ? ".EMPTY" : input) << " false" << endl;
        }
    }

    return 0;
}
