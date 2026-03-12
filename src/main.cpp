#include <vector>
#include <iostream>
using namespace std;
vector<int> newDeck(int decks) {
    vector<int> n{};
    for (int i = 1; i <= 13; i++) {
        for (int q = 0; q < 4*decks; q++) {
            n.push_back(i);
        }
    }
    return n;
}
int main() {
    cout << "Make sure this starts on a shuffle (cards left = 155)" << endl;
    vector<int> n = newDeck(4);
    for (auto q : n) {
        cout << q << endl;
    }
    return 0;
}