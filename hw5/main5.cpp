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
int emp_temp_files_count = 0, dept_temp_files_count = 0;
//fstream sorted_emp_temp_file;
//fstream sorted_dept_temp_file;

bool all_emp_records_read = false;
bool all_dept_records_read = false;

void Write_Emp(fstream& of, Records& emp)
{
    of << emp.emp_record.eid << ",";
    of << emp.emp_record.ename << ",";
    of << emp.emp_record.age << ",";
    of << emp.emp_record.salary << endl;
}

void Write_Dept(fstream& of, Records& dept)
{
    of << dept.dept_record.did << ",";
    of << dept.dept_record.dname << ",";
    of << dept.dept_record.budget << ",";
    of << dept.dept_record.managerid << endl;
}

void Write_Emp_Join_Dept(fstream& of, Records& r)
{
    of << r.dept_record.managerid << ", ";
    of << r.dept_record.did << ",";
    of << r.dept_record.dname << ",";
    of << r.dept_record.budget << ",";
    //of << emp.emp_record.eid << ",";
    of << r.emp_record.ename << ",";
    of << r.emp_record.age << ",";
    of << r.emp_record.salary << endl;

}
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

        emp_temp_files[emp_temp_files_count].open(empstr, ios::out);

        quickSortEmp(buffers, 0, end);

        while (i <= (end))
        {
            Write_Emp(emp_temp_files[emp_temp_files_count], buffers[i]);
            i++;
        }

        emp_temp_files[emp_temp_files_count].close();
        emp_temp_files_count++;
    }

    if ((bname.compare("dept") == 0))
    {
        int i = 0;
        deptstr = "dept" + to_string(dept_temp_files_count);
        dept_temp_files[dept_temp_files_count].open(deptstr, ios::out);

        quickSortDept(buffers, 0, end);

        while (i <= end)
        {
            Write_Dept(dept_temp_files[dept_temp_files_count], buffers[i]);
            i++;
        }

        dept_temp_files[dept_temp_files_count].close();
        dept_temp_files_count++;
    }

    return;
}

//Prints out the attributes from empRecord and deptRecord
//when a join condition is met
//and puts it in file Join.csv
void PrintJoin(fstream& join_file, Records& r, int i) {
    //Write_Emp_Join_Dept(join_file, r[i]);
    return;
}

//Use main memory to Merge and Join tuples
//which are already sorted in 'runs' of the relations Dept and Emp
void Merge_Join_Runs(fstream& sorted_emp_file){

    for (int i = 0; i < emp_temp_files_count; i++)
    {
        string empstr;
        empstr = "emp" + to_string(i);
        emp_temp_files[i].open(empstr, ios::in);
        buffers[i] = Grab_Emp_Record(emp_temp_files[i]);

        //while (buffers[i].no_values != -1)
        while (true)
        {
            int m = INT_MAX;
            int idx = -1;

            for (int i = 0; i < emp_temp_files_count; i++)
            {
                //if (buffers[i].emp_record.eid < 0) continue;
                if (buffers[i].emp_record.eid < m)
                {
                    m = buffers[i].emp_record.eid;
                    idx = i;
                }
            }

            //if (idx < 0) break;
            if (buffers[i].no_values == -1) break;
            Write_Emp(sorted_emp_file, buffers[idx]);
            buffers[idx] = Grab_Emp_Record(emp_temp_files[idx]);
        }
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

    fstream sorted_emp_temp_file;
    sorted_emp_temp_file.open("sorted_emp_temp_file", ios::out | ios::app);
    Merge_Join_Runs(sorted_emp_temp_file);
    //sorted_emp_temp_file.close();


    //Please delete the temporary files (runs) after you've joined both Emp.csv and Dept.csv

    for (int i = 0; i < emp_temp_files_count; i++) {
        string str = "emp" + to_string(i);
        //unlink(str.c_str());
    }

    for (int i = 0; i < dept_temp_files_count; i++) {
        string str = "dept" + to_string(i);
        //unlink(str.c_str());
    }

    return 0;
}
