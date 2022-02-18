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
        int size;

        Record(vector<string> fields) {
            id = stoi(fields[0]);
            name = fields[1];
            bio = fields[2];
            manager_id = stoi(fields[3]);
            size = 16 + bio.length() + name.length();
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
                // insert two blocks into the pageDirectory, 0 and 1
                // and
                // create files that represent blocks
                for (int i = 0; i < numBlocks; i++) {
                    pageDirectory.push_back(i);
                    createFile(pageDirectory.at(i));
                }
            }

            i = (int)(ceil(log2(numBlocks)));

            percentageFilled = (float)numRecords / (float)numBlocks;

            string binstr = htobs(createHash(record.id));
            string iBits = binstr.substr((binstr.length()) - i, i);

            // if average number of records >= 70%, increment numBlocks
            /*
            if (percentageFilled >= .7) {
                numBlocks++;
                // reorder records within blocks
            }
            */

            // check the last i bits of hashValue
            // if the last i bits are greater than the numBlocks,
            // flip the msb of the last i bits
            if (bstoi(iBits) >= numBlocks) {
                cout << "flip this bit: " << iBits.front() << endl;
                string bitFlip = iBits;
                if (iBits.front() == '1') {
                    bitFlip.front() = '0';
                }
                cout << "bitFlip: " << bitFlip << endl;
            }

            // Add record to the temp file that matches the
            // index block.
            // creating overflow block if necessary

        }

        int createHash(int id) {
            return id % (int)(pow(2.0,16.0));
        }

        // converts a hash value to binary string
        string htobs(int hash) {
            return bitset<16>(hash).to_string();
        }

        // converts a binary string to an integer
        int bstoi(string binary) {
            return stoi(binary, nullptr, 2);
        }

        bool checkPageDirectory() {
            cout << "check page directory for given value." << endl;
            return false;
        }

        void createFile(int idx) {
            string binstr = htobs(idx);
            cout << "create temp file " << binstr << ".temp." << endl;
            stringstream str;
            fstream file;
            str << idx << ".temp";
            file.open(str.str().c_str(), fstream::out);
            file.close();
        }

        void addRecord(fstream &file, Record &r) {
            cout << "add record to file." << endl;
            file<<r.id<<","<<r.name<<","<<r.bio<<","<<r.manager_id<<endl;
            numRecords++;
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
