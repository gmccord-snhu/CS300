//============================================================================
// Name        : ProjectTwo.cpp
// Author      : Grant McCord
// Date        : 10/19/2024
// Version     : 1.0
// Copyright   : None
// Description : CS 300 Project Two
//
// This file implements advising assistance software for the Computer Science
// department at ABCU. Instructors can use this application to when talking
// to students.
//
// This program is implemented as a vector primarily due to small data source size
// and ease of both implementation and maintenance. Vectors provide dynamic sizing, 
// allowing the program to adjust as needed without  predefining a fixed size, which 
// is ideal for situations where the number of courses may change frequently during 
// execution. Additionally, vectors manage memory automatically, reducing the risk of 
// memory leaks and simplifying memory management compared to manual storage structures.
//
// In order to support efficient printing in sorted order, the file contents are inserted
// into the vector alpabetically by course number.  This approach ensures that when courses 
// are added, they are immediately placed in  their correct order, eliminating the need for 
// a separate sorting step after all courses have been loaded. This will improve printing 
// performance, especially if the data set grows larger.
//
// Overall, using a vector aligns well with the program's goals of simplicity, 
// maintainability, and efficiency given the expected data size and operations.
// 
// Requirements: (my notes for implementation and checklist for done)
// ==================================================================
// [x] Input: Design code to correctly read the course data file
//     - Prompts user for file name which contains course data
// [x] Menu: Design code to create a menu that prompts a user for menu options.
//     - Option 1: Load the file data into the data structure. 
//     - Option 2: Print an alphanumeric list of all the courses 
//     - Option 3: Print the course title and the prerequisites for any individual course.
//     - Option 9: Exit the program.
// [x] Develop working code to load data from the file into the data structure.
// [x] Print out a list of the courses in alphanumeric order
//     - Print all courses with prerequisites
// [x] Develop working code to print course information
//     - Prompt the user to enter the course number
//     - Print out requested course and prequisites
// [x] Cpp source file named 'ProjectTwo.cpp'
// [x] All source code contained in 'ProjectTwo.cpp'
//
//============================================================================

#include <iomanip>
#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <sstream>
#include <algorithm>

using namespace std;

//============================================================================
// Global definitions visible to all methods and classes
//============================================================================
class Course;
vector<Course> Courses;

/**
 * @brief Represents a course in the course planner.
 *
 * The Course class stores information about a specific course, including
 * its course number, name, and a list of prerequisites. It provides a method
 * to output the course details in a user-friendly format.
 *
 * Methods:
 * - `Output()`: Displays the course number, course name, and prerequisites in a formatted manner.
 */
class Course {

public:

    // A string representing the unique identifier for the course.
    string courseNumber;
    // A string representing the name of the course.
    string courseName;
    // A vector of strings representing the prerequisite course numbers.
    vector<string> prerequisites;

    /**
     * @brief Outputs the details of the course.
     *
     * This method prints the course number, course name, and any prerequisites
     */
    void Output() const {
        cout << "Course Number: " << courseNumber << ", Course Name: " << courseName;
        if (!prerequisites.empty()) {
            cout << ", Prerequisites: ";
            for (const auto& prereq : prerequisites) {
                cout << prereq << " ";
            }
        }
        cout << endl;
    }
};

/**
 * @brief Validates a course number string.
 *
 * This function checks if the provided course number meets the following
 * criteria:
 * 1. It contains no spaces.
 * 2. It is exactly 7 characters long.
 * 3. Each character is alphanumeric (A-Z, a-z, 0-9).
 *
 * @param courseNumber A string representing the course number to validate.
 * @return true if the course number is valid; false otherwise.
 */
bool IsValidCourseNumber(string courseNumber) {

    // Remove spaces from the course number
    courseNumber.erase(
        remove_if(courseNumber.begin(), courseNumber.end(), [](unsigned char c) { return isspace(c); }),
        courseNumber.end());

    // Check if the length of the course number is exactly 7
    if (courseNumber.size() != 7) {
        return false;
    }

    // Check each character to ensure it's alphanumeric
    for (char c : courseNumber) {
        if (!isalnum(c)) {
            return false;
        }
    }

    return true;  // Valid course number
}

/**
 * @brief Checks if the given course name is valid.
 *
 * A course name is considered valid if it is not empty after
 * stripping whitespace and does not exceed 55 characters in length.
 *
 * @param courseName The name of the course to validate.
 * @return true if the course name is valid, false otherwise.
 */
bool IsValidCourseName(const string& courseName) {
    string trimmedName = courseName;
    trimmedName.erase(trimmedName.begin(), find_if(trimmedName.begin(), trimmedName.end(), [](unsigned char ch) {
        return !isspace(ch);
        }));
    trimmedName.erase(find_if(trimmedName.rbegin(), trimmedName.rend(), [](unsigned char ch) {
        return !isspace(ch);
        }).base(), trimmedName.end());

    return !trimmedName.empty() && trimmedName.size() <= 55;
}

/**
 * @brief Inserts a new course into the vector of courses in sorted order by course number.
 *
 * @param courses A reference to a vector of Course objects.
 * @param newCourse The Course object to be inserted.
 */
void InsertCourseInSortedOrder(vector<Course>& courses, const Course& newCourse) {

    unsigned int index = 0;

    // Find the correct index for the new course by comparing course numbers
    while (index < courses.size() && courses[index].courseNumber < newCourse.courseNumber) {
        index++;
    }

    // Insert the new course at the found index
    // This step increases the size of the vector by one
    // (add a temporary Course object to increase vector size)
    courses.push_back(Course());

    // Shift elements to maintain sorted order
    for (int i = courses.size() - 1; i > index; --i) {
        // Move elements to the right
        courses[i] = courses[i - 1];
    }

    // Assign the new course to the sorted position
    courses[index] = newCourse;
}

/**
 * @brief Loads courses from a specified file.
 *
 * Loads course data from the provided fileName. Course data in the file
 * must be in a comma delimited format:
 * - <course code>,<course name>,<prerequisite course codes separated with ','>
 *
 * @param fileName The name of the file containing course data.
 * @param courses A vector to store the loaded Course objects.
 * @throws exception if any errors occur while loading courses.
 */
void LoadCoursesFromFile(const string& fileName, vector<Course>& courses) {

    // file handle
    ifstream file(fileName);

    // check open status
    if (!file) {
        throw runtime_error("ERROR: File does not exist");
    }

    // process file contents
    string line;
    while (getline(file, line)) {
        istringstream stream(line);
        string token;
        vector<string> tokens;

        // Tokenize the line by comma
        while (getline(stream, token, ',')) {
            tokens.push_back(token);
        }

        // Min size required is 2 : course number, name
        if (tokens.size() < 2) {
            throw runtime_error("ERROR: Invalid course entry: " + line);
        }

        // Validate course number
        if (!IsValidCourseNumber(tokens[0])) {
            throw runtime_error("ERROR: Invalid course number: " + line);
        }

        // Validate course name
        if (!IsValidCourseName(tokens[1])) {
            throw runtime_error("ERROR: Invalid course name: " + line);
        }

        Course newCourse;
        newCourse.courseNumber = tokens[0];
        newCourse.courseName = tokens[1];

        // Process prerequisites
        for (size_t i = 2; i < tokens.size(); ++i) {
            if (!IsValidCourseNumber(tokens[i])) {
                throw runtime_error("ERROR: Invalid prerequisite course number: " + line);
            }
            newCourse.prerequisites.push_back(tokens[i]);
        }

        // Insert the course into the vector in sorted order
        InsertCourseInSortedOrder(courses, newCourse);
    }
}

/**
 * @brief Prints the details of all courses in the provided vector.
 *
 * This function iterates through each Course object in the vector
 * and calls the Output method to display its details.
 *
 * @param courses A constant reference to a vector of Course objects
 *                that contains the list of available courses.
 */
void PrintCourses(const vector<Course>& courses) {
    for (const auto& course : courses) {

        // Call the Output method of each Course object
        course.Output();
    }
}

/**
 * @brief Prints the details of a specified course.
 *
 * This function searches for a course in the provided vector of courses
 * by its course number. If the course is found, its details are printed
 * using the Output method of the Course class. If the course is not found,
 * an appropriate message is displayed.
 *
 * @param courseNumber A string representing the course number to search for.
 * @param courses A constant reference to a vector of Course objects
 */
void PrintCourseDetails(const string& courseNumber, const vector<Course>& courses) {
    for (const auto& course : courses) {
        if (course.courseNumber == courseNumber) {
            course.Output();
            return;
        }
    }
    cout << "Course not found: " << courseNumber << endl;
}

/**
 * @brief Displays the main menu for the course planner application.
 *
 * This function prints the options available to the user
 */
void DisplayMenu() {
    cout << "Welcome to the course planner.\n";
    cout << "1. Load Data Structure.\n";
    cout << "2. Print Course List.\n";
    cout << "3. Print Course.\n";
    cout << "9. Exit\n";
}

/**
 * @brief Main function
 */
int main() {

    vector<Course> courses;
    string fileName;

    // Run until user opts to exit
    while (true) {

        // Show the menu options
        DisplayMenu();
        cout << "What would you like to do? ";

        // Get user input
        int choice;
        cin >> choice;

        // Process user choice
        switch (choice) {
        case 1: {

            // Get the filename from the user
            cout << "Enter the file name: ";
            cin >> fileName;

            // Load courses from the specified file
            try {
                LoadCoursesFromFile(fileName, courses);
                cout << "Courses loaded successfully from " << fileName << ".\n";
            }
            catch (const exception& e) {
                cout << e.what() << endl; // Handle file load errors
            }
            break;
        }
        case 2: {

            // Prompt to load data
            if (courses.empty()) {
                cout << "Load the data first." << endl;
            }
            else {
                // Print all loaded courses
                PrintCourses(courses);
            }
            break;
        }
        case 3: {

            // Get the course number from the user
            cout << "What course do you want to know about? ";
            string courseNumber;
            cin >> courseNumber;

            // Print details for the specified course number
            PrintCourseDetails(courseNumber, courses);
            break;
        }
        case 9: {
            // Exit message
            cout << "Thank you for using the course planner!" << endl;
            return 0; // Exit the program
        }
        default: {
            // Handle invalid input
            cout << choice << " is not a valid option." << endl;
            break;
        }
        }
    }
}