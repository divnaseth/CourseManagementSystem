#include <iostream>
#include <string>
#include <mysql/mysql.h>

using namespace std;

// Database credentials
const char* HOST = "localhost";
const char* USER = "root";
const char* PASS = "your_password"; // Replace with your MySQL password
const char* DB = "course_management_db";
const int PORT = 3306;

void displayMenu() {
    cout << "\n===============================\n";
    cout << "   COURSE MANAGEMENT SYSTEM    \n";
    cout << "===============================\n";
    cout << "1. Register New Student\n";
    cout << "2. Add New Course\n";
    cout << "3. Enroll Student in Course\n";
    cout << "4. Update Student Grade\n";
    cout << "5. View Student Transcript\n";
    cout << "6. Generate System Report\n";
    cout << "7. Exit\n";
    cout << "Enter your choice: ";
}

int main() {
    MYSQL* conn = mysql_init(0);
    conn = mysql_real_connect(conn, HOST, USER, PASS, DB, PORT, NULL, 0);

    if (!conn) {
        cerr << "MySQL Initialization Failed!" << endl;
        return 1;
    } else {
        cout << "Successfully connected to MySQL Database!\n";
    }

    int choice;
    do {
        displayMenu();
        cin >> choice;

        if (cin.fail()) {
            cin.clear();
            cin.ignore(10000, '\n');
            cout << "Invalid input. Please enter a number.\n";
            continue;
        }

        switch (choice) {
            case 1: {
                string fname, lname, email;
                cout << "Enter First Name: ";
                cin >> fname;
                cout << "Enter Last Name: ";
                cin >> lname;
                cout << "Enter Email: ";
                cin >> email;

                string query = "INSERT INTO Students (first_name, last_name, email) VALUES ('" + fname + "', '" + lname + "', '" + email + "')";
                if (mysql_query(conn, query.c_str()) == 0) {
                    cout << "Student registered successfully!\n";
                } else {
                    cerr << "Error: " << mysql_error(conn) << "\n";
                }
                break;
            }
            case 2: {
                string code, name;
                int credits;
                cout << "Enter Course Code (e.g., CS101): ";
                cin >> code;
                cout << "Enter Course Name: ";
                cin.ignore();
                getline(cin, name);
                cout << "Enter Credits: ";
                cin >> credits;

                string query = "INSERT INTO Courses (course_code, course_name, credits) VALUES ('" + code + "', '" + name + "', " + to_string(credits) + ")";
                if (mysql_query(conn, query.c_str()) == 0) {
                    cout << "Course added successfully!\n";
                } else {
                    cerr << "Error: " << mysql_error(conn) << "\n";
                }
                break;
            }
            case 3: {
                int sId, cId;
                cout << "Enter Student ID: ";
                cin >> sId;
                cout << "Enter Course ID: ";
                cin >> cId;

                string query = "INSERT INTO Enrollments (student_id, course_id) VALUES (" + to_string(sId) + ", " + to_string(cId) + ")";
                if (mysql_query(conn, query.c_str()) == 0) {
                    cout << "Student enrolled successfully!\n";
                } else {
                    cerr << "Error: " << mysql_error(conn) << "\n";
                }
                break;
            }
            case 4: {
                int enrollmentId;
                string grade;
                cout << "Enter Enrollment ID: ";
                cin >> enrollmentId;
                cout << "Enter Grade (e.g., A, B+): ";
                cin >> grade;

                string query = "UPDATE Enrollments SET grade = '" + grade + "' WHERE enrollment_id = " + to_string(enrollmentId);
                if (mysql_query(conn, query.c_str()) == 0) {
                    cout << "Grade updated successfully!\n";
                } else {
                    cerr << "Error: " << mysql_error(conn) << "\n";
                }
                break;
            }
            case 5: {
                int sId;
                cout << "Enter Student ID to view transcript: ";
                cin >> sId;

                string query = "SELECT s.first_name, s.last_name, c.course_code, c.course_name, e.grade "
                               "FROM Students s "
                               "JOIN Enrollments e ON s.student_id = e.student_id "
                               "JOIN Courses c ON e.course_id = c.course_id "
                               "WHERE s.student_id = " + to_string(sId);

                if (mysql_query(conn, query.c_str()) == 0) {
                    MYSQL_RES* res = mysql_store_result(conn);
                    MYSQL_ROW row;
                    
                    cout << "\n--- Student Transcript ---\n";
                    bool studentFound = false;
                    while ((row = mysql_fetch_row(res))) {
                        if (!studentFound) {
                            cout << "Student Name: " << row[0] << " " << row[1] << "\n";
                            cout << "----------------------------------------\n";
                            cout << "Code\tCourse Name\t\tGrade\n";
                            cout << "----------------------------------------\n";
                            studentFound = true;
                        }
                        cout << row[2] << "\t" << row[3] << "\t\t" << row[4] << "\n";
                    }
                    if (!studentFound) {
                        cout << "No enrollment records found for Student ID: " << sId << "\n";
                    }
                    mysql_free_result(res);
                } else {
                    cerr << "Error: " << mysql_error(conn) << "\n";
                }
                break;
            }
            case 6: {
                string query = "SELECT "
                               "(SELECT COUNT(*) FROM Students) AS total_students, "
                               "(SELECT COUNT(*) FROM Courses) AS total_courses, "
                               "(SELECT COUNT(*) FROM Enrollments) AS total_enrollments";

                if (mysql_query(conn, query.c_str()) == 0) {
                    MYSQL_RES* res = mysql_store_result(conn);
                    MYSQL_ROW row = mysql_fetch_row(res);
                    if (row) {
                        cout << "\n=== SYSTEM SUMMARY REPORT ===\n";
                        cout << "Total Registered Students: " << row[0] << "\n";
                        cout << "Total Active Courses:     " << row[1] << "\n";
                        cout << "Total Course Enrollments: " << row[2] << "\n";
                        cout << "=============================\n";
                    }
                    mysql_free_result(res);
                } else {
                    cerr << "Error: " << mysql_error(conn) << "\n";
                }
                break;
            }
            case 7:
                cout << "Exiting program. Goodbye!\n";
                break;
            default:
                cout << "Invalid choice. Try again.\n";
        }
    } while (choice != 7);

    mysql_close(conn);
    return 0;
}

