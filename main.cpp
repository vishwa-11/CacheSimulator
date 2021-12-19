#include <iostream>
#include <vector>
#include <math.h>
#include <fstream>
#include <map>
using namespace std;

vector<char> HextoBin(string hex2) {
    unsigned long bin = 0;
    string bin2 = "";
    unsigned long dec = 0;

    int power = 0;
    //first convert to decimal



    for (int i = hex2.length() - 1; i >=0; --i) {
        if (isdigit(hex2[i])) {
            dec += ((int)hex2[i] - 48) * pow(16, power);

        }
        else {
            if (hex2[i] == 'A' || hex2[i] == 'a') {
                dec += 10 * pow(16, power);

            }
            else if (hex2[i] == 'B' || hex2[i] == 'b') {
                dec += 11 * pow(16, power);

            }
            else if (hex2[i] == 'C' || hex2[i] == 'c') {
                dec += 12 * pow(16, power);

            }
            else if (hex2[i] == 'D' || hex2[i] == 'd') {
                dec += 13 * pow(16, power);

            }
            else if (hex2[i] == 'E' || hex2[i] == 'e') {
                dec += 14 * pow(16, power);

            }
            else if (hex2[i] == 'F' || hex2[i] == 'f') {
                dec += 15 * pow(16, power);

            }

            else {
                break;
            }
        }
        power++;
    }

    while (dec > 0) {
        if (dec % 2 == 0) {
            bin2.push_back('0');

        }
        else {
            bin2.push_back('1');
        }

        dec /= 2;
    }

    if (bin2.length() < 32) {
        while (bin2.length() < 32) {
            bin2.push_back('0');
        }
    }

    reverse(bin2.begin(), bin2.end());

    vector<char> bin3;

    for (int i = 0; i < 32; i++) {
        bin3.push_back(bin2[i]);

    }
    return bin3;
}

int BinToDec(string bin) {
    int answer = 0;
    int power = 0;

    for (int i = bin.length() - 1; i >= 0; i--) {
        answer += ((int)bin[i] - 48) * pow(2, power);
        power++;
    }

    return answer;

}

vector<vector<char>> loadAddresses() {
    fstream inFile;
    inFile.open("gcc.txt");
    vector<vector<char>> adds;

    string add;
    vector<char> bin;

    if (inFile.is_open()) {

        while (getline(inFile, add, ' ')) {
            getline(inFile, add, ' ');
            bin = HextoBin(add);

            adds.push_back(bin);
            getline(inFile, add, ' ');
            getline(inFile, add, ' ');
        }

    }

    else {
        cout << "not open";
    }

    return adds;
}

void DirectMap(vector<vector<char>> adds) {
    int sets;
    int bytesPerBlock;

    cout << "How many sets: ";
    cin >> sets;
    cout << endl;
    cout << "How many bytes per block: ";
    cin >> bytesPerBlock;
    cout << endl;

    int total = 0;
    int hits = 0;
    float hitRate;

    int offsetSize = log2(bytesPerBlock);
    int setSize = log2(sets);
    int tagSize = 32 - offsetSize - setSize;


    int tag;
    int line;

    string temp = "";

    map<int, long> DMap;

    for (int i = 0; i < sets; i++) {
        DMap[i] = -1;
    }

    for (int i = 0; i < adds.size(); i++) {
        for (int j = 0; j < tagSize; j++) {
            temp.push_back(adds[i][j]);
        }

        tag = BinToDec(temp);
        temp = "";

        for (int j = tagSize; j < tagSize + setSize; j++) {
            temp.push_back(adds[i][j]);
        }
        line = BinToDec(temp);
        temp = "";

        if (DMap[line] != tag) {
            total++;
            DMap[line] = tag;
        }
        else {
            total++;
            hits++;
        }

    }

    hitRate = (float)hits / (float)total;

    cout << "The hit rate for the Direct Mapped cache is: " << hitRate << endl;

}

void SA_LRU(vector<vector<char>> adds) {
    int sets;
    int blocks;
    int bytesPerBlock;

    cout << "How many sets: ";
    cin >> sets;
    cout << endl;

    cout << "How many blocks: ";
    cin >> blocks;
    cout << endl;

    int setAssociativity = blocks / sets;
    cout << "The cache is " << setAssociativity << "-way associative." << endl;

    cout << "How many bytes per block: ";
    cin >> bytesPerBlock;
    cout << endl;

    int total = 0;
    int hits = 0;
    float hitRate;

    int offsetSize = log2(bytesPerBlock);
    int setSize = log2(sets);
    int tagSize = 32 - offsetSize - setSize;

    int tag;
    int line;
    bool match = false;

    string temp = "";

    map<int, vector<long>> SMap;

    for (int i = 0; i < sets; i++) {
        vector<long> tags;

        for (int j = 0; j < setAssociativity; j++) {
            tags.push_back(-1);
        }

        SMap[i] = tags;
    }

    for (int i = 0; i < adds.size(); i++) {

        for (int j = 0; j < tagSize; j++) {
            temp.push_back(adds[i][j]);
        }

        tag = BinToDec(temp);
        temp = "";



        for (int j = tagSize; j < tagSize + setSize; j++) {
            temp.push_back(adds[i][j]);
        }

        line = BinToDec(temp);
        temp = "";

        bool match = false;

        for (int j = 0; j < setAssociativity; j++) {
            if (SMap[line][j] == tag) {
                vector<long> tags2;
                tags2.push_back(SMap[line][j]);
                for (int k = 0; k < setAssociativity; k++) {
                    if (k != j) {
                        tags2.push_back(SMap[line][k]);
                    }
                }

                SMap[line] = tags2;
                match = true;
                hits++;
                total++;
            }
        }

        if (!match) {
            vector<long> tags2;
            tags2.push_back(tag);

            for (int k = 0; k < setAssociativity - 1; k++) {
                tags2.push_back(SMap[line][k]);
            }
            total++;
            SMap[line] = tags2;
        }

        else {
            match = false;
            continue;
        }

    }

    hitRate = (float)hits / (float)total;

    cout << "The hit rate for the " << setAssociativity << "-way Set Associative cache with LRU is " << hitRate << endl;

}

void SA_FIFO(vector<vector<char>> adds) {
    int sets;
    int blocks;
    int bytesPerBlock;

    cout << "How many sets: ";
    cin >> sets;
    cout << endl;

    cout << "How many blocks: ";
    cin >> blocks;
    cout << endl;

    int setAssociativity = blocks / sets;
    cout << "The cache is " << setAssociativity << "-way associative." << endl;

    cout << "How many bytes per block: ";
    cin >> bytesPerBlock;
    cout << endl;

    int total = 0;
    int hits = 0;
    float hitRate;

    int offsetSize = log2(bytesPerBlock);
    int setSize = log2(sets);
    int tagSize = 32 - offsetSize - setSize;

    int tag;
    int line;
    bool match = false;

    string temp = "";

    map<int, vector<long>> SMap;

    for (int i = 0; i < sets; i++) {
        vector<long> tags;

        for (int j = 0; j < setAssociativity; j++) {
            tags.push_back(-1);
        }

        SMap[i] = tags;
    }

    for (int i = 0; i < adds.size(); i++) {

        for (int j = 0; j < tagSize; j++) {
            temp.push_back(adds[i][j]);
        }

        tag = BinToDec(temp);
        temp = "";



        for (int j = tagSize; j < tagSize + setSize; j++) {
            temp.push_back(adds[i][j]);
        }

        line = BinToDec(temp);
        temp = "";

        bool match = false;

        for (int j = 0; j < setAssociativity; j++) {
            if (SMap[line][j] == tag) {
                match = true;
                hits++;
                total++;
                break;
            }
        }

        if (!match) {
            vector<long> tags2;
            tags2.push_back(tag);

            for (int k = 0; k < setAssociativity - 1; k++) {
                tags2.push_back(SMap[line][k]);
            }
            total++;
            SMap[line] = tags2;
        }

        else {
            match = false;
            continue;
        }

    }

    hitRate = (float)hits / (float)total;

    cout << "The hit rate for the " << setAssociativity << "-way Set Associative cache with FIFO is " << hitRate << endl;

}

void FA_LRU(vector<vector<char>> adds) {
    int blocks;
    int bytesPerBlock;

    cout << "How many blocks: ";
    cin >> blocks;
    cout << endl;
    cout << "How many bytes per block: ";
    cin >> bytesPerBlock;
    cout << endl;

    int total = 0;
    int hits = 0;
    float hitRate;

    int offsetSize = log2(bytesPerBlock);
    int tagSize = 32 - offsetSize;


    int tag;

    string temp = "";

    vector<long> tags;
    vector<long> tags2;

    for (int i = 0; i < blocks; i++) {
        tags.push_back(-1);
    }

    bool match = false;

    for (int i = 0; i < adds.size(); i++) {
        for (int j = 0; j < tagSize; j++) {
            temp.push_back(adds[i][j]);
        }

        tag = BinToDec(temp);
        temp = "";

        for (int j = 0; j < blocks; j++) {
            if (tags[j] == tag) {
                hits++;
                total++;

                tags2.push_back(tags[j]);

                for (int k = 0; k < blocks; k++) {
                    if (k != j) {
                        tags2.push_back(tags[k]);
                    }
                }

                tags = tags2;
                tags2.clear();
                match = true;
                break;
            }
        }

        if (!match) {
            tags2.push_back(tag);

            for (int j = 0; j < blocks - 1; j++) {
                tags2.push_back(tags[j]);
            }

            total++;

            tags = tags2;
            tags2.clear();
        }

        else {
            match = false;
            continue;
        }

    }

    hitRate = (float)hits / (float)total;

    cout << "The hit rate for the Fully Associative cache with LRU is: " << hitRate << endl;
}

void FA_FIFO(vector<vector<char>> adds) {
    int blocks;
    int bytesPerBlock;

    cout << "How many blocks: ";
    cin >> blocks;
    cout << endl;
    cout << "How many bytes per block: ";
    cin >> bytesPerBlock;
    cout << endl;

    int total = 0;
    int hits = 0;
    float hitRate;

    int offsetSize = log2(bytesPerBlock);
    int tagSize = 32 - offsetSize;


    int tag;

    string temp = "";

    vector<long> tags;
    vector<long> tags2;

    for (int i = 0; i < blocks; i++) {
        tags.push_back(-1);
    }

    bool match = false;

    for (int i = 0; i < adds.size(); i++) {
        for (int j = 0; j < tagSize; j++) {
            temp.push_back(adds[i][j]);
        }

        tag = BinToDec(temp);
        temp = "";

        for (int j = 0; j < blocks; j++) {
            if (tags[j] == tag) {
                hits++;
                total++;
                match = true;
                break;
            }
        }

        if (!match) {
            tags2.push_back(tag);

            for (int j = 0; j < blocks - 1; j++) {
                tags2.push_back(tags[j]);
            }

            total++;

            tags = tags2;
            tags2.clear();
        }

        else {
            match = false;
            continue;
        }

    }

    hitRate = (float)hits / (float)total;

    cout << "The hit rate for the Fully Associative cache with FIFO is: " << hitRate << endl;
}

int main() {

    vector<vector<char>> adds = loadAddresses();

    DirectMap(adds);

    SA_LRU(adds);

    SA_FIFO(adds);

    FA_LRU(adds);

    FA_FIFO(adds);

}

