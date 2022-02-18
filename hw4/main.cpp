/*
Skeleton code for linear hash indexing
*/

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

    // Loop to lookup IDs until user is ready to quit

    return 0;
}

/*
 * Questions:
 *
 * 1 - what does a single block read look like?
 *
 * 2 - What does the EmployeeIndex file look like?
 *
 * 3 - how is the variable length array supposed to look
 *     after every insertion of a record?
 *
 *     are we supposed to alter it after every single insert?
 *
 *     variable length record lecture starts at 47:14 on jan 27

 2a - improved block based nested loop - 160M

 2b - optimized merge-sort join ~ 3k
    - why not hash join
 */
