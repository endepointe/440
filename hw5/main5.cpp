/* This is a skeleton code for Optimized Merge Sort,
 * you can make modifications as long as you meet
   all question requirements*/

#include <bits/stdc++.h>
#include <unistd.h>
#include "record_class.h"

using namespace std;

//defines how many blocks are available in the Main Memory
#define buffer_size 22

Records buffers[buffer_size]; //use this class object of size
                              //22 as your main memory
fstream emp_temp_files[buffer_size];
fstream dept_temp_files[buffer_size];
int emp_temp_files_idx = 0, dept_temp_files_idx = 0;
fstream sorted_emp_temp_file;
fstream sorted_dept_temp_file;
bool all_emp_records_read = false;
bool all_dept_records_read = false;

/***You can change return type and arguments as you want.***/

//Sorting the buffers in main_memory and storing the sorted
//records into a temporary file (runs)
void Sort_Buffer(){
    //Remember: You can use only [AT MOST] 22 blocks for sorting the
    //          the records / tuples and create the runs
    fstream empfile,deptfile;
    string empstr,deptstr;

    empstr = "emp" + to_string(emp_temp_files_idx);
    deptstr = "dept" + to_string(dept_temp_files_idx);

    quickSortEmp(buffers,0,buffer_size - 1);

    empfile.open(empstr, ios::out);

    empfile << buffers[emp_temp_files_idx].emp_record.eid << ",";
    empfile << buffers[emp_temp_files_idx].emp_record.ename << ",";
    empfile << buffers[emp_temp_files_idx].emp_record.age << ",";
    empfile << buffers[emp_temp_files_idx].emp_record.salary << endl;

    empfile.close();

    quickSortDept(buffers,0,buffer_size - 1);

    deptfile.open(deptstr, ios::out);

    deptfile << buffers[emp_temp_files_idx].dept_record.did << ",";
    deptfile << buffers[emp_temp_files_idx].dept_record.dname << ",";
    deptfile << buffers[emp_temp_files_idx].dept_record.budget << ",";
    deptfile << buffers[emp_temp_files_idx].dept_record.managerid << endl;

    deptfile.close();

    return;
}

//Prints out the attributes from empRecord and deptRecord
//when a join condition is met
//and puts it in file Join.csv
void PrintJoin() {

    return;
}

//Use main memory to Merge and Join tuples
//which are already sorted in 'runs' of the relations Dept and Emp
void Merge_Join_Runs(){


    //and store the Joined new tuples using PrintJoin()
    return;
}

int main() {

    //Open file streams to read and write
    //Opening out two relations Emp.csv
    //and Dept.csv which we want to join
    fstream empin;
    fstream deptin;
    empin.open("Emp.csv", ios::in);
    deptin.open("Dept.csv", ios::in);

    //Creating the Join.csv file where we will store our joined results
    fstream joinout;
    joinout.open("Join.csv", ios::out | ios::app);

    //1. Create runs for Dept and Emp which are sorted using Sort_Buffer()
    Records e,d;
    int i = 0;
    while (empin.good() && i < buffer_size) {
        e = Grab_Emp_Record(empin);
        buffers[i].emp_record = e.emp_record;
        buffers[i].number_of_emp_records = i+1;
        buffers[i].no_values += buffers[i].number_of_emp_records;
        i++;
    }

    empin.close();

    i = 0;

    while (deptin.good() && i < buffer_size) {
        d = Grab_Dept_Record(deptin);
        buffers[i].dept_record = d.dept_record;
        buffers[i].number_of_dept_records = i+1;
        buffers[i].no_values += buffers[i].number_of_dept_records;
        i++;
    }

    deptin.close();

    Sort_Buffer();

    //2. Use Merge_Join_Runs() to Join the runs of Dept and Emp relations


    //Please delete the temporary files (runs) after you've joined both Emp.csv and Dept.csv

    for (int i = 0; i <= emp_temp_files_idx; i++) {
        string str = "emp" + to_string(i);
        //unlink(str.c_str());
    }

    for (int i = 0; i <= dept_temp_files_idx; i++) {
        string str = "dept" + to_string(i);
        //unlink(str.c_str());
    }

    return 0;
}
