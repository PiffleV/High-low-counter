#include <vector>
#include <variant>
#include <algorithm>
#include <iterator>
#include <iostream>
#include <fstream>
#include <filesystem>
#include <unordered_map>
using namespace std;
// custom error
class outOfBounds : public runtime_error {
public:
    explicit outOfBounds(const std::string& message) : runtime_error(message){

    }
};

// Simple Deck Builder
vector<int> newDeck(int decks) { 
    vector<int> n{};
    for (int i = 1; i <= 13; i++) { // 13 cards in a deck
        for (int q = 0; q < 4*decks; q++) { // 4 cards per deck
            n.push_back(i);
        }
    }
    return n;
}

// Get File or create if doesnt exist
fstream retFile(string file, string base) {
    if (!filesystem::exists(file)) {
        cout << "File " << file << " does not exist, using template" << endl;
        fstream f (file, fstream::in | fstream::out | fstream::app); // Idk why but adding app makes this work
        if (f.is_open()) { // Handle Errors
            f << base;
            f.close();
        } else {
            cerr << "Error Opening File" << endl;
        }
        cerr << "Success Creating File!" << endl;
        fstream fileRead (file, fstream::in);// Redefine file as read only
        return fileRead;
    } else {
        fstream f (file, fstream::in); // If file exists return readable
        cerr << "Success Opening File!" << endl;
        return f;
    }
}
// Command to int
const unordered_map<string, int> commandValues {
    {"e", -1},
    {"h", 14},
    {"p", 15}
};
// Make sure int is in bounds and is an int
int sanitizeInt(string s) {
    int value = -1;
    while (value == -1) {
        if (commandValues.count(s)) {
            return commandValues.at(s);
        }
        try { // Check if number
            value = stoi(s);
            if (value > 13 or value < 1) {
                throw outOfBounds("Out of Bounds");
            }
        } catch (const invalid_argument& e) {
            cout << "Not a number" << endl;
            getline(cin >> ws, s);
        } catch (const outOfBounds& e) {
            cout << "Out of bounds" << endl;
            value = -1;
            getline(cin >> ws, s);
        } catch (...) {
            cout << "Unexpected Error." << endl;
            getline(cin >> ws, s);
        }
    }
    return value;
}

// For a command
int cardCommand(int value, vector<int> deck) {
    if (value == commandValues.at("h")) {
        cout << "e to exit, p to peek" << endl;
        return 1;
    } else if (value == commandValues.at("p")) {
        cout << "[";
        for (auto card : deck) {
            cout << card << ", ";
        }
        cout << "\b \b\b \b]" << endl;
        return 1;
    }
    return 0;
}

// Calculate EV
float ev(float winChance, float mult) {
    return (winChance*(mult-1))-(1-winChance);
}

int main() {
    vector<int> deck = newDeck(3);
    fstream dataFile = retFile("multData.csv", // Num, Higher Mult, Lower Mult, Same Mult
R"(1,1.05,1.05,13
2,1.15,1.15,13
3,1.3,5.8,13
4,1.45,3.85,13
5,1.65,2.9,13
6,1.95,2.3,13
7,1.9,1.9,13
8,2.3,1.95,13
9,2.9,1.65,13
10,3.85,1.45,13
11,5.8,1.3,13
12,11.5,1.15,13
13,1.05,1.05,13)");
    if (!dataFile.is_open()) {
        cerr << "Error Opening File." << endl;
        return 0;
    }
    std::ostringstream fullData;
    fullData << dataFile.rdbuf();
    std::string data = fullData.str();
    dataFile.close(); // Read full file
    cerr << "Success Reading File!" << endl;
    std::istringstream lineStream(data);
    std::string lineImmutable;
    unordered_map<int, unordered_map<char, float>> multValues;
    while (getline(lineStream, lineImmutable)) { // Each line
        try {
            string line = lineImmutable;
            int type = -1;
            float multData[3] = {0,0,0};
            int pos = -1;
            while (line.find(",") != string::npos) { // for each comma
                if (pos == -1) { // add num type is first loop
                    type = stoi(line.substr(0, line.find(",")));
                    line = line.erase(0, line.find(",") + 1);
                    pos++;
                }
                multData[pos] = stof(line.substr(0, line.find(","))); // else add to the mult array
                line = line.erase(0, line.find(",") + 1);
                pos++;
            }
            multData[pos] = stof(line);
            unordered_map<char, float> multPerNum{{'h',multData[0]},{'l',multData[1]},{'s',multData[2]}};
            multValues[type] = multPerNum; // format into unordered map
        } catch (...) {
            cerr << "Error reading file multData.csv, possibly mangled" << endl;
            return 0;
        }
    }
    cerr << "Success Parsing multData.csv!" << endl;
    cout << "Make sure this starts on a shuffle (cards left = 155)" << endl; // Reminder
    std::string card;
    while (card != "exit") {
        cout << "Enter card value, or h to view commands" << endl;
        getline(cin >> ws, card);
        int value = sanitizeInt(card);
        if (value == -1) {
            return 0;
        } if (value > 13) {
            cardCommand(value, deck);
            continue;
        }
        auto item = find(deck.begin(), deck.end(), value);
        if (item == deck.end()) { // Account for desync
            cerr << "Deck here and unb deck are desynced; terminating program" << endl;
            return 0;
        }
        deck.erase(item); // deck len will never be 0 here
        int numLower, numHigher, numSame;
        numLower = numHigher = numSame = 0;
        for (int i : deck) {
            if (i > value) {
                numHigher++;
            } else if (i < value) {
                numLower++;
            } else {
                numSame++;
            }
        }
        float highChance = ((float)numHigher/deck.size()); // Calculate percentages and EVs
        float lowChance = ((float)numLower/deck.size());
        float sameChance = ((float)numSame/deck.size());
        unordered_map<char, float> multForCard = multValues.at(value);
        float evHigh = ev(highChance,multForCard['h']);
        float evLow = ev(lowChance,multForCard['l']);
        float evSame = ev(sameChance,multForCard['s']);
        cout << "Higher %: " << to_string(highChance*100) << "%, EV: " << to_string(evHigh) << ", " << endl;
        cout << "Lower %: " << to_string(lowChance*100) << "%, EV: " << to_string(evLow) << ", " << endl;
        cout << "Same %: " << to_string(sameChance*100) << "%, EV: " << to_string(evSame) << ", " << endl;
        dealerInput: cout << "Enter dealer card, or h to view commands" << endl; // Get and remove dealer's card
        getline(cin >> ws, card);
        value = sanitizeInt(card);
        if (value > 13) {
            cardCommand(value, deck);
            goto dealerInput; // Cleaner than a weird while loop personally
        }
        if (value == -1) {
            return 0;
        }
        item = find(deck.begin(), deck.end(), value);
        if (item == deck.end()) { // Account for desync
            cerr << "Deck here and unb deck are desynced; terminating program" << endl;
            return 0;
        }
        deck.erase(item); // Account for shuffling
        if (deck.size() == 0) {
            deck = newDeck(3);
        }
    }
    return 1;
}