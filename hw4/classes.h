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

            string hbs = htobs(createHash(record.id));
            string iBits = hbs.substr((hbs.length()) - i, i);

            cout << "hbs of " << record.id << " is " << hbs << endl;
            cout << "last " << i << " bits: " << iBits << endl;

            // if average number of records >= 70%, increment numBlocks
            /*
            if (percentageFilled >= .7) {
                numBlocks++;
                // reorder records within blocks
            }
            */

            // scan pageDirectory for pageDirectory[i] == iBits
            if (findBlock(iBits)) {
                //open file check if there is space
                //the loop is here to open the 16bit filename, filling
                //the additional space with zeros to match correct file.
                string block;
                for (int i = 0; i < 16-iBits.length(); i++) {
                    block += "0";
                }
                block += iBits;
                block += ".block";
                cout << "open block: " << block << endl;
                //if there is space, insert record into block
                vector<string> data;
                readBlockData(block, data);
                cout << "first line of " << block << " is " << data.at(0) << endl;
                cout << "second line of " << block << " is " << data.at(1) << endl;
                cout << "third line of " << block << " is " << data.at(2) << endl;
                cout << data.at(0) << " / " << PAGE_SIZE << " = ";
                cout << (float)(stoi(data.at(0)) + record.size) / (float)(PAGE_SIZE) << endl;
                // if the current block size + new record size are <
                // PAGE_SIZE, add the record to the block.
                if (((float)(stoi(data.at(0))+record.size) <
                            (float)(PAGE_SIZE))) {
                    addRecord(hbs, record, iBits, false);
                    updateBlockSize(block,record);
                    updateBlockRecordCount(block);
                    updateBlockHashList(block,hbs);
                    cout << "add record to block: " << block << endl;
                } else {
                    // create an overflow block.
                    // For now, just add the records until
                    // I have a better solution

                    // Current solution would be to keep the block size
                    // < than PAGE_SIZE and just increment the number
                    // of records in the block.
                    // Then, to check if there was overflow, check if the
                    // sum of the record.size of all records in
                    // block >= PAGE_SIZE,

                }
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
                        updateBlockSize(block,record);
                        updateBlockRecordCount(block);
                        updateBlockHashList(block,hbs);
                        addRecord(hbs, record, iBits, true);
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
        // only need 16 bits
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
            cout << "create block file " << binstr << ".block." << endl;
            stringstream str;
            fstream file;

            str << binstr << ".block";
            file.open(str.str().c_str(), fstream::out);

            // write starting block size of 0 to line 1
            // the number of records in block on line 2
            // and the csv of hashvalues as binary strings on line 3
            file << 0 << endl;
            file << 0 << endl;
            file << endl;

            file.close();
        }

        // the pageDirectory block has already been found.
        // just open the file
        void addRecord(string bs, Record &r, string bits, bool flipped) {
            // open the file
            fstream bf;
            stringstream str;
            if (!flipped) {
                str << bs << ".block";
            } else {
                // open the file matching the flipped bit in the bs
                string temp = bs.substr(0,bs.length() - i);
                string bitFlip = bits;
                if (bs.front() == '1') {
                    bitFlip.front() = '0';
                } else {
                    bitFlip.front() = '1';
                }
                temp += bitFlip;
                str << bitFlip << ".block";
                cout << "addRecord: bitFlip/bs = "<<bitFlip<<"/"<<bs<<endl;
            }

            bf.open(str.str().c_str(), fstream::app);

            cout << "ADD RECORD: ";
            cout<<r.id<<","<<r.name<<","<<r.bio<<","<<r.manager_id<<endl;

            bf<<r.id<<","<<r.name<<","<<r.bio<<","<<r.manager_id<<endl;

            bf.close();
        }

        // update line 1 of block file
        void updateBlockSize(string filename, Record &record) {
            ifstream ifs;
            ofstream ofs;
            ifs.open(filename);
            ofs.open("_del_");
            string line;
            int size;

            getline(ifs,line);
            size = stoi(line);

            size += record.size;

            ofs << size << endl;
            ofs << ifs.rdbuf();

            ofs.close();
            ifs.close();

            if (remove(filename.c_str()) != 0) {
                perror("Error deleting file.");
            } else {
                rename("_del_", filename.c_str());
            }
        }

        // update line 2 of block file
        void updateBlockRecordCount(string filename) {
            ifstream ifs;
            ofstream ofs;
            ifs.open(filename);
            ofs.open("_del_");
            string line;
            int size, count;

            getline(ifs,line);
            size = stoi(line);
            getline(ifs,line);
            count = stoi(line);

            count++;

            ofs << size << endl;
            ofs << count << endl;
            ofs << ifs.rdbuf();

            ofs.close();
            ifs.close();

            if (remove(filename.c_str()) != 0) {
                perror("Error deleting file.");
            } else {
                rename("_del_", filename.c_str());
            }
        }

        // update line 3 of block file
        void updateBlockHashList(string filename, string recordHash) {
            ifstream ifs;
            ofstream ofs;
            ifs.open(filename);
            ofs.open("_del_");

            string line,hash;
            int size, count;
            stringstream ss;
            vector<string> h;

            // line 1
            getline(ifs,line);
            size = stoi(line);

            // line 2
            getline(ifs,line);
            count = stoi(line);

            // line 3
            while (getline(ifs, line)) {
                ss << line;
                while (getline(ss,hash,',')) {
                    h.push_back(hash);
                }
                ss.clear();
            }

            h.push_back(recordHash);

            ofs << size << endl;
            ofs << count << endl;

            for (vector<string>::iterator it = h.begin();
                    it != h.end(); ++it) {
                if (it != h.begin()) {
                    ofs << ',';
                }
                ofs << *it;
            }
            ofs << endl;

            ofs << ifs.rdbuf();

            ofs.close();
            ifs.close();

            if (remove(filename.c_str()) != 0) {
                perror("Error deleting file.");
            } else {
                rename("_del_", filename.c_str());
            }
        }

        // reads the block data of given filename:
        // line 1: size of block (must be < 4096 and < .7
        // line 2: number of records in block
        // line 3: csv of records
        void readBlockData(string filename, vector<string> &data) {
            cout << "read file: " << filename << endl;
            ifstream ifs;
            ifs.open(filename, ios::in);
            string text;
            int num = 0;

            if (ifs.is_open()) {
                while (num < 3) {
                    getline(ifs, text);
                    data.push_back(text);
                    num++;
                }
            } else {
                cout << "Error opening file." << endl;
            }

            ifs.close();
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
