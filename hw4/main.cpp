#include <string>
#include <ios>
#include <fstream>
#include <vector>
#include <string>
#include <string.h>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <cmath>
#include "classes.h"
using namespace std;


int main(int argc, char* const argv[]) {
    bool cont = true;
    string input;
    int id;
	// Create the index
	LinearHashIndex emp_index("EmployeeIndex");
	emp_index.createFromFile("Employee.csv");
	emp_index.save();
	emp_index.load();

	// Loop to lookup IDs until user is ready to quit
    while (cont) {
        try {
            cout << "Enter Employee ID [(q)uit, exit]: ";
            getline(cin, input);
            id = stoi(input);
            Record r = emp_index.findRecordById(id);
            if (r.id < 0) {
                printf("ID %d not found\n", id);
            }
            else {
                r.print();
            }
        } catch (const invalid_argument &ia) {
            if (input.compare("q") == 0 ||
                input.compare("quit") == 0 ||
                input.compare("exit") == 0) {
                cont = false;
            } else {
                cout << "Not a valid entry." << endl;
            }
        }
    }
    cout << "EXITING..."<< endl;

	return 0;
}
