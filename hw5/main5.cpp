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
int emp_temp_files_count = 0, dept_temp_files_count = 0;
fstream sorted_emp_temp_file;
fstream sorted_dept_temp_file;
bool all_emp_records_read = false;
bool all_dept_records_read = false;

/***You can change return type and arguments as you want.***/

//Sorting the buffers in main_memory and storing the sorted
//records into a temporary file (runs)
void Sort_Buffer(string bname, int end){
    //Remember: You can use only [AT MOST] 22 blocks for sorting the
    //          the records / tuples and create the runs
    fstream empfile,deptfile;
    string empstr,deptstr;

    if ((bname.compare("emp")) == 0)
    {
        int i = 0;
        empstr = "emp" + to_string(emp_temp_files_count);
        empfile.open(empstr, ios::out);

        quickSortEmp(buffers, 0, end);

        while (i <= (end))
        {
            empfile << buffers[i].emp_record.eid << ",";
            empfile << buffers[i].emp_record.ename << ",";
            empfile << buffers[i].emp_record.age << ",";
            empfile << buffers[i].emp_record.salary << endl;
            i++;
        }

        empfile.close();
        emp_temp_files_count++;
    }

    if ((bname.compare("dept") == 0))
    {
        int i = 0;
        deptstr = "dept" + to_string(dept_temp_files_count);
        deptfile.open(deptstr, ios::out);

        quickSortDept(buffers,0, end);

        while (i <= end)
        {
            deptfile << buffers[i].dept_record.did << ",";
            deptfile << buffers[i].dept_record.dname << ",";
            deptfile << buffers[i].dept_record.budget << ",";
            deptfile << buffers[i].dept_record.managerid << endl;
            i++;
        }

        deptfile.close();
        dept_temp_files_count++;
    }

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
void Merge_Join_Runs(fstream& sorted_emp){
    fstream empin,deptin;
    string empstr,deptstr;
    Records r;

    for (int i = 0; i < emp_temp_files_count; i++)
    {
        empstr = "emp" + to_string(i);
        empin.open(empstr, ios::in);
        buffers[i] = Grab_Emp_Record(empin);

        /*
        while (true)
        {
            int m = INT_MAX;
            int idx = -1;

            for (int i = 0; i < emp_temp_files_count; i++)
            {
                if (buffers[i].emp_record.eid < 0) continue;
                if (buffers[i].emp_record.eid < m)
                {
                    m = buffers[i].emp_record.eid;
                    idx = i;
                }
            }

            if (idx < 0) break;
            sorted_emp << buffers[idx].emp_record.eid << ",";
            sorted_emp << buffers[idx].emp_record.ename << ",";
            sorted_emp << buffers[idx].emp_record.age << ",";
            sorted_emp << buffers[idx].emp_record.salary << endl;
            buffers[i] = Grab_Emp_Record(empin);
        }
        */
    }

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
    //Records e,d;
    int i = 0;
    Records r = Grab_Emp_Record(empin);

    while (r.no_values != -1)
    {
        buffers[i].emp_record = r.emp_record;
        i++;
        if (i == buffer_size)
        {
            Sort_Buffer("emp", i - 1);
            i = 0;
        }
        r = Grab_Emp_Record(empin);
    }
    Sort_Buffer("emp", i - 1);

    i = 0;
    r = Grab_Dept_Record(deptin);

    while (r.no_values != -1)
    {
        buffers[i].dept_record = r.dept_record;
        i++;
        if (i == buffer_size)
        {
            Sort_Buffer("dept", i - 1);
            i = 0;
        }
        r = Grab_Dept_Record(deptin);
    }
    Sort_Buffer("dept", i - 1);

    empin.close();
    deptin.close();

    //2. Use Merge_Join_Runs() to Join the runs of Dept and Emp relations

    sorted_emp_temp_file.open("sort_emp_temp_file", ios::app);
    Merge_Join_Runs(sorted_emp_temp_file);


    //Please delete the temporary files (runs) after you've joined both Emp.csv and Dept.csv

    for (int i = 0; i < emp_temp_files_count; i++) {
        string str = "emp" + to_string(i);
        unlink(str.c_str());
    }

    for (int i = 0; i < dept_temp_files_count; i++) {
        string str = "dept" + to_string(i);
        //unlink(str.c_str());
    }

    return 0;
}
