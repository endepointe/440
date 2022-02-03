/* This is a skeleton code for two-pass multi-way sorting, you can make modifications as long as you meet 
   all question requirements*/  
//Currently this program will just load the records in the buffers (main memory) and print them when filled up.
//And continue this process untill the full Emp.csv is read. 

#include <unistd.h>
#include <bits/stdc++.h>
#include <cassert>
#include <algorithm>
using namespace std;

//defines how many buffers are available in the Main Memory 
#define buffer_size 22

int num_files = 0;
fstream files[buffer_size];

struct EmpRecord {
    int eid;
    string ename;
    int age;
    double salary;
};
EmpRecord buffers[buffer_size]; // this structure contains 22 buffers; available as your main memory.

// Grab a single block from the Emp.csv file, in theory if a block was larger than
// one tuple, this function would return an array of EmpRecords and the entire if
// and else statement would be wrapped in a loop for x times based on block size
EmpRecord Grab_Emp_Record(fstream &empin) {
    string line, word;
    EmpRecord  emp;
    // grab entire line
    if (getline(empin, line, '\n')) {
        // turn line into a stream
        stringstream s(line);
        // gets everything in stream up to comma
        getline(s, word,',');
        emp.eid = stoi(word);
        getline(s, word, ',');
        emp.ename = word;
        getline(s, word, ',');
        emp.age = stoi(word);
        getline(s, word, ',');
        emp.salary = stod(word);
        return emp;
    } else {
        emp.eid = -1;
        return emp;
    }
}

void Write_Emp_Record(fstream& out, EmpRecord& r)
{
	out << r.eid << "," << r.ename << "," << r.age << "," << r.salary << endl;
}


//Printing whatever is stored in the buffers of Main Memory 
//Can come in handy to see if you've sorted the records in your main memory properly.
void Print_Buffers(int cur_size) {
    for (int i = 0; i < cur_size; i++)
    {
        cout << i << " " << buffers[i].eid << " " << buffers[i].ename << " " << buffers[i].age << " " << buffers[i].salary << endl;
    }
}


bool Less(const EmpRecord& a, const EmpRecord& b)
{
	return a.eid < b.eid;
}


//Sorting the buffers in main_memory based on 'eid' and storing the sorted records into a temporary file 
//You can change return type and arguments as you want. 
void Sort_in_Main_Memory(int count){
    sort(buffers, buffers + count, Less);
	
	assert(num_files < buffer_size);
	
	stringstream ss;
	ss << "tmp" << num_files << ".txt";
	
	files[num_files].open(ss.str().c_str(), fstream::out);
	for(int i = 0; i < count; ++i) {
		Write_Emp_Record(files[num_files], buffers[i]);
	}
	
	files[num_files].close();
	num_files++;
    return;
}

//You can use this function to merge your M-1 runs using the buffers in main memory and storing them in sorted file 'EmpSorted.csv'(Final Output File) 
//You can change return type and arguments as you want. 
void Merge_Runs_in_Main_Memory(fstream& sorted_file){
	
    for(int i = 0; i < num_files; ++i) {
	  stringstream ss;
		ss << "tmp" << i << ".txt";
	
	  files[i].open(ss.str().c_str(), ios::in);
	  
	  buffers[i] = Grab_Emp_Record(files[i]);
	}
	
	while(true) {
		int m = INT_MAX, idx = -1;
		for(int i = 0; i < num_files; ++i) {
			if(buffers[i].eid < 0) continue;
			if(buffers[i].eid < m) {
				m = buffers[i].eid;
				idx = i;
			}
		}
		if(idx < 0) break;
		
		Write_Emp_Record(sorted_file, buffers[idx]);
		buffers[idx] = Grab_Emp_Record(files[idx]);
	}
}

int main() {
  // open file streams to read and write
  fstream input_file;
  input_file.open("Emp.csv", ios::in);
 
  // flags check when relations are done being read
  bool flag = true;
  int cur_size = 0;
  
  /*First Pass: The following loop will read each block put it into main_memory 
    and sort them then will put them into a temporary file for 2nd pass */
  while (flag) {
      // FOR BLOCK IN RELATION EMP

      // grabs a block
      EmpRecord  single_EmpRecord  = Grab_Emp_Record(input_file);
      // checks if filestream is empty
      if (single_EmpRecord.eid == -1) {
          flag = false;
          Print_Buffers(cur_size); // The main_memory is not filled up but there are some leftover data that needs to be sorted.
      }
      else if(cur_size + 1 <= buffer_size){
          //Memory is not full store current record into buffers.
          buffers[cur_size] = single_EmpRecord ;
          cur_size += 1;
      }
      else{
          //memory is full now. Sort the blocks in Main Memory and Store it in a temporary file (runs)
          cout << "Main Memory is full. Time to sort and store sorted blocks in a temporary file" << endl;
          Print_Buffers(buffer_size);
          Sort_in_Main_Memory(buffer_size);
          
          //After sorting start again. Clearing memory and putting the current one into main memory.
          cur_size = 0;
          buffers[cur_size] = single_EmpRecord;
          cur_size += 1;
      }
      
  }
  if(cur_size > 0) {
	  Print_Buffers(cur_size);
      Sort_in_Main_Memory(cur_size);
  }
  
  input_file.close();
  
  /* Implement 2nd Pass: Read the temporary sorted files and utilize main_memory to store sorted runs into the EmpSorted.csv*/

  //Uncomment when you are ready to store the sorted relation
  fstream sorted_file;  
  sorted_file.open("EmpSorted.csv", ios::out | ios::app);
  
  //Pseudocode
  Merge_Runs_in_Main_Memory(sorted_file);
  
  //You can delete the temporary sorted files (runs) after you're done if you want. It's okay if you don't.
	for(int i = 0; i < num_files; ++i) {
		stringstream ss;
		ss << "tmp" << i << ".txt";
		unlink(ss.str().c_str());
	}
  return 0;
}
