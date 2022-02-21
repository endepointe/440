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

	// Create the index
	LinearHashIndex emp_index("EmployeeIndex");
	emp_index.createFromFile("Employee.csv");
	emp_index.save();
	emp_index.load();

	// Loop to lookup IDs until user is ready to quit
	while (true) {
		printf("input id: ");
		int id; scanf("%d", &id);
		Record r = emp_index.findRecordById(id);
		if (r.id < 0) {
			printf("ID %d not found\n", id);
		}
		else {
			r.print();
		}
	}

	return 0;
}
