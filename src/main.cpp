#include <vector>
#include <variant>
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
int main() {
    vector<int> deck = newDeck(4);
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
    std::string data;
    dataFile >> data;
    cerr << "Success Reading File!" << endl;
    std::istringstream lineStream (data);
    std::string line;
    unordered_map<int, unordered_map<char, float>> multValues;
    while (getline(lineStream, line)) {
        try {
            int type = -1;
            float multData[3];
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
            unordered_map<char, float> multPerNum{{'h',multData[0]},{'l',multData[1]},{'s',multData[2]}};
            multValues[type] = multPerNum; // format into unordered map
        } catch (...) {
            cerr << "Error reading file multData.csv, possibly mangled" << endl;
            return 0;
        }
    }
    cerr << "Success Parsing multData.csv!" << endl;
    cout << "Make sure this starts on a shuffle (cards left = 155)" << endl; // Reminder
    return 0;
}