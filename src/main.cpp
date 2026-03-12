#include <vector>
#include <variant>
#include <algorithm>
#include <iterator>
#include <iostream>
#include <fstream>
#include <filesystem>
#include <unordered_map>
using namespace std;
vector<int> newDeck(int decks) { // Simple Deck Builder
    vector<int> n{};
    for (int i = 1; i <= 13; i++) { // 13 cards in a deck
        for (int q = 0; q < 4*decks; q++) { // 4 cards per deck
            n.push_back(i);
        }
    }
    return n;
}
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
int sanitizeInt(string s) {
    int value = -1;
    while (value == -1) {
        if (s == "exit") {
            return -1;
        }
        try { // Check if number
            value = stoi(s);
        } catch (const invalid_argument& e) {
            cout << "Not a number" << endl;
            getline(cin >> ws, s);
        } catch (...) {
            cout << "Unexpected Error." << endl;
            getline(cin >> ws, s);
        }
    }
    return value;
}
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
        cout << "Enter card value, or type \"exit\" to exit" << endl;
        getline(cin >> ws, card);
        int value = sanitizeInt(card);
        if (value == -1) {
            return 0;
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
        float highChance = ((float)numHigher/deck.size());
        float lowChance = ((float)numLower/deck.size());
        float sameChance = ((float)numSame/deck.size());
        unordered_map<char, float> multForCard = multValues[value];
        float evHigh = ev(highChance,multForCard['h']);
        float evLow = ev(lowChance,multForCard['l']);
        float evSame = ev(sameChance,multForCard['s']);
        cout << "Higher %: " << to_string(highChance*100) << "%, EV: " << to_string(evHigh) << ", " << endl;
        cout << "Lower %: " << to_string(lowChance*100) << "%, EV: " << to_string(evLow) << ", " << endl;
        cout << "Same %: " << to_string(sameChance*100) << "%, EV: " << to_string(evSame) << ", " << endl;
        cout << "Enter dealer card, or type \"exit\" to exit" << endl;
        getline(cin >> ws, card);
        value = sanitizeInt(card);
        if (value == -1) {
            return 0;
        }
        item = find(deck.begin(), deck.end(), value);
        if (item == deck.end()) { // Account for desync
            cerr << "Deck here and unb deck are desynced; terminating program" << endl;
            return 0;
        }
        deck.erase(item);
        if (deck.size() == 0) {
            deck = newDeck(3);
        }
    }
    return 0;
}