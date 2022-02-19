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
                    createBlockFile(pageDirectory.at(i));
                }
            }

            i = (int)(ceil(log2(numBlocks)));

            //percentageFilled = (float)numRecords / (float)numBlocks;

            string hbs = htobs(createHash(record.id));
            string iBits = hbs.substr((hbs.length()) - i, i);

            // if average number of records >= 70%, increment numBlocks
            /*
            if (percentageFilled >= .7) {
                numBlocks++;
                // reorder records within blocks
            }
            */

            // scan pageDirectory for pageDirectory[i] == iBits
            if (findBlock(iBits)) {
                //check if there is space in matching block file
                fstream fs;
                stringstream ss;
                string size;
                ss << hbs << ".temp";
                fs.open(ss.str().c_str(), fstream::in);
                cout << "reading file: " << ss.str().c_str() << endl;
                getline(fs, size);
                cout << "size of block: " << size << endl;
                fs.close();
                //if there is space, insert record into block
                //addRecord(hbs, record, false);
            } else {
                // flip the msb of the last i bits
                if (bstoi(iBits) >= numBlocks) {
                    cout << "flip this bit: " << iBits.front() << endl;
                    string bitFlip = iBits;
                    if (iBits.front() == '1') {
                        bitFlip.front() = '0';
                    } else {
                        bitFlip.front() = '1';
                    }
                    cout << "search again for flipped bit" << endl;
                    if (findBlock(bitFlip)) {
                        cout << "insert record into matched block." << endl;
                        //addRecord(hbs, record, true);
                    } else {
                        cout << "add a new block." << endl;
                        numBlocks++;
                    }
                }
            }
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

        // scans pageDirectory for matching i bits
        bool findBlock(string bits) {
            bool found = false;
            string temp, block;
            cout << "check page directory for last: ";
            cout << i << "bits of "<< bits << endl;
            for (vector<int>::iterator it = pageDirectory.begin();
                it != pageDirectory.end(); ++it) {
                temp = htobs(*it);
                block = temp.substr(temp.length()-i, temp.length());
                cout << "pd("<<*it<<"): "<< block << endl;
                // block found
                if (block.compare(bits) == 0) {
                    cout << block << " == " << bits << endl;
                    found = true;
                    break;
                }
            }
            return found;
        }

        // creates block file, using the idx to create a 16 bit
        // filename. eg: idx=1, then fn=0000000000000001.temp
        void createBlockFile(int idx) {
            string binstr = htobs(idx);
            cout << "create temp file " << binstr << ".temp." << endl;
            stringstream str;
            fstream file;

            str << binstr << ".temp";
            file.open(str.str().c_str(), fstream::out);

            // write starting block size of 0 to line 1
            // the number of records in block on line 2
            // and the csv of hashvalues as binary strings on line 3
            file << 0 << endl;
            file << 0 << endl;
            file << " " << endl;

            file.close();
        }

        // the pageDirectory block has already been found.
        // just open the file
        void addRecord(string bs, Record &r, bool flipped) {
            // open the file
            fstream bf;
            stringstream str;
            if (!flipped) {
                str << bs << ".temp";
            } else {
                // open the file matching the flipped bit in the bs
                string bitFlip = bs;
                if (bs.front() == '1') {
                    bitFlip.front() = '0';
                } else {
                    bitFlip.front() = '1';
                }
                str << bitFlip << ".temp";
                cout << "addRecord: bitFlip/bs = "<<bitFlip<<"/"<<bs<<endl;
            }

            bf.open(str.str().c_str(), fstream::out);

            // read block size
            int size = readBlockSize(bf);

            // read record count
            int count = readRecordCount(bf);

            // if all is well, write the record to the block
            if (size + r.size > PAGE_SIZE) {
                bf<<r.id<<","<<r.name<<","<<r.bio<<","<<r.manager_id<<endl;
                numRecords++;
            }
            bf.close();

            // update the record header (the top 3 lines) with the
            // new data:
            // 1 size
            // 2 count
            // 3 csv of records
        }

        int readBlockSize(fstream &file) {
            string size;
            getline(file, size);
            return stoi(size);
        }

        // reads the second line of a block file,
        // returning the number of records in the block.
        int readRecordCount(fstream &file) {
            string count;
            getline(file, count);
            return stoi(count);
        }

        void writeBlockSize(fstream &file, Record &r, int size) {
            file << r.size + size << endl;
        }

        void writeRecordSize(fstream &file, Record &r, int size) {

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
