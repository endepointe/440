#include <string>
#include <vector>
#include <string>
#include <iostream>
#include <sstream>
#include <bitset>
#include <cstdio>
#include <assert.h>
using namespace std;


char* writeInt(char* buf, int v)
{
	int* p = (int*)buf;
	*p = v;
	++p;
	return (char*)p;
}


char* readInt(char* buf, int& v)
{
	int* p = (int*)buf;
	v = *p;
	++p;
	return (char*)p;
}


char* writeStr(char* buf, string& v)
{
	char* p = buf;
	for (int i = 0; i < (int)v.length(); ++i) {
		*p = v[i];
		p++;
	}
	return (char*)p;
}


char* readStr(char* buf, int len, string& v)
{
	v = string(buf, len);
	return (char*)buf + len;
}



struct DirHeader
{
	int numBlocks;
	int block_size;
	int nextFreePage;
	int numRecords;
	int i;
};


struct BlockHeader
{
	int next;
	int bytes_left;
};


class Record {
public:
	int id, manager_id;
	std::string bio, name;

	Record(vector<std::string> fields) {
		id = stoi(fields[0]);
		name = fields[1];
		bio = fields[2];
		manager_id = stoi(fields[3]);
	}

	Record()
	{
		id = manager_id = -1;
	}

	int getlen()
	{
		return 4 * sizeof(int) + (int)bio.length() + (int)name.length();
	}

	void write(char* buf)
	{
		buf = writeInt(buf, id);
		buf = writeInt(buf, manager_id);
		buf = writeInt(buf, (int)name.length());
		buf = writeStr(buf, name);
		buf = writeInt(buf, (int)bio.length());
		buf = writeStr(buf, bio);
	}

	char* read(char* buf)
	{
		buf = readInt(buf, id);
		buf = readInt(buf, manager_id);
		int len = 0;
		buf = readInt(buf, len);
		buf = readStr(buf, len, name);
		buf = readInt(buf, len);
		buf = readStr(buf, len, bio);
		return buf;
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
	const double THRESHOLD = 0.5;

	vector<int> pageDirectory;  // Where pageDirectory[h(id)] gives page index of block
								// can scan to pages using index*PAGE_SIZE as offset (using seek function)
	int numBlocks; // n
	int i;
	int numRecords; // Records in index
	int nextFreePage; // Next page to write to
	int block_size;
	string fName;
	FILE* fp;
	bool noExpand = false;


	int hashCode(int id)
	{
		return id % (1 << 16);
	}


	int search(int id)
	{
		int h = hashCode(id);
		h &= (1 << i) - 1;
		return h;
	}


	int alloc()
	{
		char* block = (char*)new char[block_size];
		assert(block);

		memset(block, 0, block_size);

		BlockHeader* h = (BlockHeader*)block;
		h->next = -1;
		h->bytes_left = block_size - sizeof(BlockHeader);

		fseek(fp, nextFreePage * block_size, SEEK_SET);

		int ret = fwrite(block, block_size, 1, fp);
		assert(ret == 1);

		delete[] block;
		return nextFreePage++;
	}


	void insert(int& block, Record record)
	{
		if (block < 0) {
			block = alloc();
		}

		char* buf = (char*)new char[block_size];
		assert(buf);

		fseek(fp, block * block_size, SEEK_SET);

		int ret = fread(buf, block_size, 1, fp);
		assert(ret == 1);

		int rlen = record.getlen();

		BlockHeader* h = (BlockHeader*)buf;
		if (h->bytes_left >= rlen) {
			char* p = buf + block_size - h->bytes_left;
			record.write(p);
			h->bytes_left -= rlen;

			fseek(fp, block * block_size, SEEK_SET);

			ret = fwrite(buf, block_size, 1, fp);
			assert(ret == 1);

			delete[] buf;
			return;
		}

		bool needsave = false;
		if (h->next < 0) {
			needsave = true;
		}

		insert(h->next, record);

		if (needsave) {
			fseek(fp, block * block_size, SEEK_SET);

			ret = fwrite(buf, block_size, 1, fp);
			assert(ret == 1);
		}

		delete[] buf;
	}


	// Insert new record into index
	void insertRecord(Record record) {

		// Add record to the index in the correct block, creating overflow block if necessary
		int idx = search(record.id);
		if (idx < numBlocks) {
			insert(pageDirectory[idx], record);
		}
		else {
			idx &= ~(1 << (i - 1));
			insert(pageDirectory[idx], record);
		}

		numRecords++;

		// Take neccessary steps if capacity is reached
		double ratio = (double)nextFreePage / numBlocks;
		if (ratio >= THRESHOLD) {
			pageDirectory.push_back(-1);
			numBlocks++;

			if (!noExpand && numBlocks > (1 << i)) {
				redist();
			}
		}
	}


	Record searchRecord(int block, int id)
	{
		if (block < 0) return Record();

		char* buf = (char*)new char[block_size];
		assert(buf);

		fseek(fp, block * block_size, SEEK_SET);

		int ret = fread(buf, block_size, 1, fp);
		assert(ret == 1);

		BlockHeader* h = (BlockHeader*)buf;
		char* s = buf + sizeof(BlockHeader);
		char* e = buf + block_size - h->bytes_left;

		while (s < e) {
			Record r;
			s = r.read(s);

			if (r.id == id) {
				delete[] buf;
				return r;
			}
		}

		int next = h->next;
		delete[] buf;

		return searchRecord(next, id);
	}


	void redist()
	{
		LinearHashIndex I("tmp");
		I.numBlocks = numBlocks;
		I.i = i + 1;
		I.numRecords = 0;
		I.block_size = block_size;
		I.nextFreePage = 0;
		I.pageDirectory.resize(numBlocks, -1);
		I.noExpand = true;

		for (int j = 0; j < numBlocks; ++j) {
			redist(pageDirectory[j], I);
		}

		I.save();
		fclose(fp);
		fp = NULL;
		//save();

		{
			stringstream cmd;
			cmd << "rm " << fName;
			system(cmd.str().c_str());
		}

		{
			stringstream cmd;
			cmd << "mv tmp " << fName;
			system(cmd.str().c_str());
		}

		load();
	}


	void redist(int bid, LinearHashIndex& I)
	{
		if (bid < 0) return;

		char* buf = (char*)new char[block_size];
		assert(buf);

		fseek(fp, bid * block_size, SEEK_SET);

		int ret = fread(buf, block_size, 1, fp);
		assert(ret == 1);

		BlockHeader* h = (BlockHeader*)buf;

		char* s = buf + sizeof(BlockHeader);
		char* e = buf + block_size - h->bytes_left;

		while (s < e) {
			Record r;
			s = r.read(s);

			I.insertRecord(r);
		}

		int next = h->next;
		delete[] buf;

		redist(next, I);
	}


public:
	LinearHashIndex(string indexFileName) {
		numBlocks = 0;
		i = 0;
		numRecords = 0;
		block_size = 0;
		fName = indexFileName;
		nextFreePage = 0;
		fp = NULL;
		
		load();
	}

	~LinearHashIndex()
	{
		save();
	}


	void load()
	{
		if (fp) return;
		fp = fopen(fName.c_str(), "rb+");
		if (!fp) {
			fp = fopen(fName.c_str(), "wb");
			assert(fp);
			fclose(fp);

			fp = fopen(fName.c_str(), "rb+");
		}
		assert(fp);

		fseek(fp, 0, SEEK_END);
		int flen = (int)ftell(fp);
		if (flen == 0) {
			block_size = PAGE_SIZE;
			i = 1;
			numBlocks = 2;
			pageDirectory.resize(numBlocks, -1);
		}
		else {
			assert(flen >= (int)sizeof(DirHeader));
			fseek(fp, -(int)sizeof(DirHeader), SEEK_END);

			DirHeader header;
			int ret = fread(&header, sizeof(header), 1, fp);
			assert(ret == 1);

			numBlocks = header.numBlocks;
			numRecords = header.numRecords;
			nextFreePage = header.nextFreePage;
			block_size = header.block_size;
			i = header.i;
			pageDirectory.clear();

			int total = sizeof(DirHeader) + numBlocks * sizeof(int) + block_size * nextFreePage;
			assert(flen == total);

			fseek(fp, block_size * nextFreePage, SEEK_SET);

			for (int j = 0; j < numBlocks; ++j) {
				int block = -1;
				int ret = fread(&block, sizeof(int), 1, fp);
				assert(ret == 1);
				assert((block >= 0 && block < nextFreePage) || block == -1);
				pageDirectory.push_back(block);
			}
		}
	}


	void save()
	{
		if (!fp) return;

		fseek(fp, block_size * nextFreePage, SEEK_SET);

		for (int j = 0; j < numBlocks; ++j) {
			int block = pageDirectory[j];
			int ret = fwrite(&block, sizeof(int), 1, fp);
			assert(ret == 1);
		}

		DirHeader header;
		header.numBlocks = numBlocks;
		header.numRecords = numRecords;
		header.nextFreePage = nextFreePage;
		header.block_size = block_size;
		header.i = i;

		int ret = fwrite(&header, sizeof(header), 1, fp);
		assert(ret == 1);

		fclose(fp);
		fp = NULL;
	}


	// Read csv file and add records to the index
	void createFromFile(string csvFName) {
		FILE* fpp = fopen(csvFName.c_str(), "r");

		char buf[10000];
		while (fgets(buf, 10000, fpp)) {
			Record r;
			char* s = strtok(buf, ",");
			assert(s);
			r.id = atoi(s);
			s = strtok(NULL, ",");
			assert(s);
			r.name = s;
			s = strtok(NULL, ",");
			assert(s);
			r.bio = s;
			s = strtok(NULL, ",");
			assert(s);
			r.manager_id = atoi(s);

			Record r2 = findRecordById(r.id);
			if (r2.id < 0) {
				insertRecord(r);
				

				//r2 = findRecordById(r.id);
			}
			//printf("%d %d\n", ++id, this->numRecords);
		}

		fclose(fpp);
	}

	// Given an ID, find the relevant record and print it
	Record findRecordById(int id) {
		if (numBlocks == 0) return Record();

		int idx = search(id);
		if (idx < numBlocks) {
			Record res = searchRecord(pageDirectory[idx], id);
			if (res.id >= 0) return res;
		}
		idx &= ~(1 << (i - 1));
		return searchRecord(pageDirectory[idx], id);
	}
};


