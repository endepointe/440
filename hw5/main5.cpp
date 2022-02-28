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
fstream sorted_emp_temp_file, sorted_dept_temp_file;
fstream emp_temp_files[buffer_size];
fstream dept_temp_files[buffer_size];
int emp_temp_files_count = 0, dept_temp_files_count = 0;
int emp_count = 0, dept_count = 0;
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
void PrintJoin(fstream& of, Records& er, Records& dr)
{
    of << dr.dept_record.managerid << ", ";
    of << dr.dept_record.did << ",";
    of << dr.dept_record.dname << ",";
    of << dr.dept_record.budget << ",";
    //of << emp.emp_record.eid << ",";
    of << er.emp_record.ename << ",";
    of << er.emp_record.age << ",";
    of << er.emp_record.salary << endl;

}
void PrintEmp(Records& r)
{
    cout << r.emp_record.eid << ", ";
    cout << r.emp_record.ename << ", ";
    cout << r.emp_record.age << ", ";
    cout << r.emp_record.salary << "\n";
}

void PrintDept(Records& r)
{
    cout << r.dept_record.did << ", ";
    cout << r.dept_record.dname << ", ";
    cout << r.dept_record.budget << ", ";
    cout << r.dept_record.managerid << "\n";
}
//Use main memory to Merge and Join tuples
//which are already sorted in 'runs' of the relations Dept and Emp
void Merge_Join_Runs(fstream& joinfile){

    sorted_emp_temp_file.open("sorted_emp_temp_file", ios::out | ios::app);
    sorted_dept_temp_file.open("sorted_dept_temp_file", ios::out | ios::app);

    for (int count = 0; count < emp_temp_files_count; count++)
    {
            string empstr;
            empstr = "emp" + to_string(count);
            emp_temp_files[count].open(empstr, ios::in);
    }

    for (int count = 0; count < dept_temp_files_count; count++)
    {
            string deptstr;
            deptstr = "dept" + to_string(count);
            dept_temp_files[count].open(deptstr, ios::in);
    }

    // write out sorted_emp_file
    int i = 0;
    int idx = 0;
    while (i < emp_count)
    {
        for (int j = 0; j < emp_temp_files_count; j++)
        {
            Records r = Grab_Emp_Record(emp_temp_files[j]);
            if (r.no_values == -1)
            {
                continue;
            }

            buffers[idx] = r;
            idx++;
            i++;
        }

        if (idx == buffer_size)
        {
            quickSortEmp(buffers, 0, buffer_size - 1);
            idx = 0;
            for (int n = 0; n < buffer_size; n++)
            {
                Write_Emp(sorted_emp_temp_file, buffers[n]);
            }
        }
    }
    // if there are records left in the buffer, write them out
    for (int n = 0; n < idx; n++)
    {
        Write_Emp(sorted_emp_temp_file, buffers[n]);
    }


    // write out sorted_dept_file
    i = 0;
    idx = 0;
    while (i < dept_count)
    {
        for (int j = 0; j < dept_temp_files_count; j++)
        {
            Records r = Grab_Dept_Record(dept_temp_files[j]);
            if (r.no_values == -1)
            {
                continue;
            }

            buffers[idx] = r;
            idx++;
            i++;
        }

        if (idx == buffer_size)
        {
            quickSortDept(buffers, 0, buffer_size - 1);
            idx = 0;
            for (int n = 0; n < buffer_size; n++)
            {
                Write_Dept(sorted_dept_temp_file, buffers[n]);
            }
        }
    }
    // if there are records left in the buffer, write them out
    for (int n = 0; n < idx; n++)
    {
        Write_Dept(sorted_dept_temp_file, buffers[n]);
    }

    sorted_emp_temp_file.close();
    sorted_dept_temp_file.close();

    //and store the Joined new tuples using PrintJoin()

    sorted_emp_temp_file.open("sorted_emp_temp_file", ios::in);
    sorted_dept_temp_file.open("sorted_dept_temp_file", ios::in);

    Records e, d;
    e = Grab_Emp_Record(sorted_emp_temp_file);
    d = Grab_Dept_Record(sorted_dept_temp_file);

    while (e.no_values != -1)
    {
        if (e.emp_record.eid < d.dept_record.managerid)
        {
            // grab the next record in emp file
            //e = Grab_Emp_Record(sorted_emp_temp_file);
            Records te = e;
            if (te.no_values != -1)
            {
                e = Grab_Emp_Record(sorted_emp_temp_file);
            }
        }

        if (e.emp_record.eid > d.dept_record.managerid)
        {
            // grab the next record in dept file
            Records td = d;
            if (td.no_values != -1)
            {
                d = Grab_Dept_Record(sorted_dept_temp_file);
            }
            else {
                Records te = e;
                if (te.no_values != -1)
                {
                    e = Grab_Emp_Record(sorted_emp_temp_file);
                }
            }
        }

        if (e.emp_record.eid == d.dept_record.managerid)
        {
            PrintJoin(joinfile,e,d);
            // grab the next record in dept file
            Records td = d;
            if (td.no_values != -1)
            {
                d = Grab_Dept_Record(sorted_dept_temp_file);
            }
            else {
                Records te = e;
                if (te.no_values != -1)
                {
                    e = Grab_Emp_Record(sorted_emp_temp_file);
                }
            }
        }
    }

    sorted_emp_temp_file.close();
    sorted_dept_temp_file.close();

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
        emp_count++;
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
        dept_count++;
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
    Merge_Join_Runs(joinout);

    joinout.close();

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
