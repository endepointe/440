/* This is a skeleton code for Optimized Merge Sort,
 * you can make modifications as long as you meet
   all question requirements*/
/* This record_class.h contains the class Records,
 * which can be used to store tuples form Emp.csv
 * (stored in the EmpRecords) and Dept.csv
 * (Stored in DeptRecords) files.
*/
#include <bits/stdc++.h>

using namespace std;

class Records{
    public:

    struct EmpRecord {
        int eid;
        string ename;
        int age;
        double salary;
    }emp_record;

    struct DeptRecord {
        int did;
        string dname;
        double budget;
        int managerid;
    }dept_record;

    /*** You can add more variables if you want below ***/

    int no_values = 0; //You can use this to check if
                       // you've don't have any more tuples
    int number_of_emp_records = 0; // Tracks number of emp_records
                                   // you have on the buffer
    int number_of_dept_records = 0; // Track number of dept_records
                                    // you have on the buffer
};

// Grab a single block from the Emp.csv file and put
// it inside the EmpRecord structure of the Records Class
Records Grab_Emp_Record(fstream &empin) {
    string line, word;
    Records emp;
    // grab entire line
    if (getline(empin, line, '\n')) {
        // turn line into a stream
        stringstream s(line);
        // gets everything in stream up to comma
        getline(s, word,',');
        emp.emp_record.eid = stoi(word);
        getline(s, word, ',');
        emp.emp_record.ename = word;
        getline(s, word, ',');
        emp.emp_record.age = stoi(word);
        getline(s, word, ',');
        emp.emp_record.salary = stod(word);

        //Ensuring that you cannot use both structure
        //(EmpEecord, DeptRecord) at the same memory block / time
        emp.dept_record.did = 0;
        emp.dept_record.dname = "";
        emp.dept_record.budget = 0;
        emp.dept_record.managerid = 0;

        return emp;
    } else {
        emp.no_values = -1;
        return emp;
    }
}

// Grab a single block from the Dept.csv file and put it inside the DeptRecord structure of the Records Class
Records Grab_Dept_Record(fstream &deptin) {
    string line, word;
    //DeptRecord dept;
    Records dept;
    if (getline(deptin, line, '\n')) {
        stringstream s(line);
        getline(s, word,',');
        dept.dept_record.did = stoi(word);
        getline(s, word, ',');
        dept.dept_record.dname = word;
        getline(s, word, ',');
        dept.dept_record.budget = stod(word);
        getline(s, word, ',');
        dept.dept_record.managerid = stoi(word);

        //Ensuring that you cannot use both structure (EmpEecord, DeptRecord) at the same memory block / time
        dept.emp_record.eid = 0;
        dept.emp_record.ename = "";
        dept.emp_record.age = 0;
        dept.emp_record.salary = 0;

        return dept;
    } else {
        dept.no_values = -1;
        return dept;
    }
}

void swap(Records* r1, Records* r2) {
    Records r = *r1;
    *r1 = *r2;
    *r2 = r;
}

int emp_partition(Records records[], int low, int high) {
    int pivot = records[high].emp_record.eid;
    int i = (low - 1);

    for (int j = low; j <= high - 1; j++) {
        if (records[j].emp_record.eid < pivot) {
            i++;
            swap(&records[i], &records[j]);
        }
    }
    swap(&records[i + 1], &records[high]);
    return (i + 1);
}

int dept_partition(Records records[], int low, int high) {
    int pivot = records[high].dept_record.managerid;
    int i = (low - 1);

    for (int j = low; j <= high - 1; j++) {
        if (records[j].dept_record.managerid < pivot) {
            i++;
            swap(&records[i], &records[j]);
        }
    }
    swap(&records[i + 1], &records[high]);
    return (i + 1);
}

void quickSortEmp(Records records[], int low, int high) {
    if (low < high) {
        int pi = emp_partition(records, low, high);
        quickSortEmp(records, low, pi - 1);
        quickSortEmp(records, pi + 1, high);
    }
}
void quickSortDept(Records records[], int low, int high) {
    if (low < high) {
        int pi = dept_partition(records, low, high);
        quickSortDept(records, low, pi - 1);
        quickSortDept(records, pi + 1, high);
    }
}
