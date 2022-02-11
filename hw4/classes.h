#include <string>
#include <vector>
#include <string>
#include <iostream>
#include <sstream>
#include <bitset>
using namespace std;

class Record {
    public:
        int id, manager_id; // fixed size of 8 bytes
        string bio, name; // [0, 500], [0, 200] bytes

        Record(vector<string> fields) {
            id = stoi(fields[0]);
            name = fields[1];
            bio = fields[2];
            manager_id = stoi(fields[3]);
        }

        void print() {
            cout << "\tID: " << id << "\n";
            cout << "\tNAME: " << name << "\n";
            cout << "\tBIO: " << bio << "\n";
            cout << "\tMANAGER_ID: " << manager_id << "\n";
        }
};


class LinearHashIndex {

    private:
        const int PAGE_SIZE = 4096;

        vector<int> pageDirectory;  // Where pageDirectory[h(id)] gives
                                    // page index of block.
                                    //
                                    // can scan to pages using
                                    // index*PAGE_SIZE
                                    // as offset (using seek function)
        int numBlocks; // n
        int i; // the number of bits reqd to store numBlocks
               // 2^(i-1) < numBlocks <= 2^(i)
        int numRecords; // Records in index
        int nextFreePage; // Next page to write to
        float percentageFilled;
        string fName;

        // Insert new record into index
        void insertRecord(Record record) {

            // No records written to index yet
            if (numRecords == 0) {
                // Initialize index with first blocks (start with 2)
                numBlocks = 2;
            }

            // Add record to the index in the correct block,
            // creating overflow block if necessary
            int hashValue = record.id % (int)(pow(2.0, 16.0));

            // I need to use i to determine the number of bits
            // to use as the directory index entries. I was going
            // about this wrong for an hour :(

            i = (int)log2(numBlocks);
            string bitValue = bitset<64>(hashValue).to_string();
            cout << "hashValue: " << hashValue << endl;
            cout << "bitValue: " << bitValue << endl;
            cout << "log2(" << numBlocks << "): " << i << endl;
            pageDirectory.push_back(i);
            numRecords = pageDirectory.size();

            // Take neccessary steps if capacity is reached

        }

        void getPageDirectory() {
            for (vector<int>::iterator it = pageDirectory.begin();
                                            it != pageDirectory.end();
                                            ++it) {
                cout << " " << *it;
            }

        }

    public:
        LinearHashIndex(string indexFileName) {
            numBlocks = 0;
            i = 0;
            numRecords = 0;
            numBlocks = 0;
            percentageFilled = 0.0;
            fName = indexFileName;
        }

        // Read csv file and add records to the index
        void createFromFile(string csvFName) {
            ifstream inF;
            inF.open(csvFName);

            if (!inF) {
                cerr << "Cannot open file: " << csvFName << endl;
            }

            vector<string> recordFields;
            string line, field;

            while (getline(inF, line, '\n')) {

                stringstream ss(line);
                // id
                getline(ss, field, ',');
                recordFields.push_back(field);
                // name
                getline(ss, field, ',');
                recordFields.push_back(field);
                // bio
                getline(ss, field, ',');
                recordFields.push_back(field);
                // manager_id
                getline(ss, field, ',');
                recordFields.push_back(field);

                Record newRecord(recordFields);
                insertRecord(newRecord);

                recordFields.clear();
            }

            inF.close();
        }

        // Given an ID, find the relevant record and print it
        Record findRecordById(int id) {
            // read max of 2 blocks, ideally 1 unless bit flip

            // put the hashed values into a vector that
            // matches the directory index key.
        }
};
