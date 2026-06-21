School Management System
A console-based School Management System built in C++ that brings together core data structures and algorithms to run a full school workflow — logins, students, teachers, classes, attendance, library, fees, and announcements — all backed by simple file storage.

Overview

This project simulates how a school could digitize its day-to-day administration. Instead of separate spreadsheets for students, teachers, attendance, and fees, everything is handled through one role-based console application, with each module backed by a different data structure chosen to fit the problem it solves.

Features by Role

Admin
- Add/remove system users and assign roles (Admin, Teacher, Student, Librarian)
- Add, update, and delete teachers
- Create classes with sections, enroll students, and assign subjects/teachers
- Add and view announcements
- Add, update, and delete student records
- Manage fee records and view/pay outstanding fees
- View all stored user credentials
Teacher
- View assigned classes, sections, and subjects
- Enter/update student grades for assigned subjects
- Mark attendance for students in assigned sections
- View all classes
Student
- View personal grades
- View personal attendance history
- View school announcements
- Check fee payment status

Librarian
- Add new books to the library catalog
- Issue books to verified students
- Process book returns
- Display the full book catalog with availability

Data Structures & Algorithms Used

Each module is built around a data structure suited to its access pattern:

| Module | Structure Used | Why |
|---|---
| Login System | `vector` | Simple linear list of user records with username lookup |
| Student Manager | Singly **Linked List** (with embedded course list per student) | Dynamic insert/delete of student records without resizing overhead |
| Teacher Manager | `stack` | Demonstrates stack-based storage/retrieval (LIFO) of teacher records |
| Classroom Manager | Nested `queue` (Classes → Sections → Students/Subjects) | Models the natural ordering of enrollment and class structure |
| Attendance Manager | Sorted `vector` + **Binary Search** | Fast lookup of attendance records by student ID |
| Library Manager | **Binary Heap** (`make_heap`/`push_heap`) | Efficient priority-based storage of the book catalog |
| Fee Manager | **Binary Search Tree (BST)** | Fast insert/search of fee records keyed by student ID |
| Announcement Manager | **Binary Search Tree (BST)** | Keeps announcements organized and retrievable in sorted order |

Tech Stack

Language: C++
Standard Library: STL containers (`vector`, `stack`, `queue`), file I/O (`fstream`), string streams (`sstream`)
Persistence: Plain text files (`users.txt`, `students.txt`, `teachers.txt`, `classrooms.txt`, `attendance.txt`, `library.txt`, `fees.txt`, `announcements.txt`)

Project Structure

```
SchoolManagementSystem/
├── main.cpp          # All classes + main() entry point
├── users.txt          # Generated at runtime
├── students.txt        # Generated at runtime
├── teachers.txt        # Generated at runtime
├── classrooms.txt      # Generated at runtime
├── attendance.txt       # Generated at runtime
├── library.txt          # Generated at runtime
├── fees.txt              # Generated at runtime
├── announcements.txt     # Generated at runtime
└── README.md
```

How to Build & Run

1. Clone the repository:
   ```
   git clone https://github.com/ZK151/school-management-system.git
   cd school-management-system
   ```
2. Compile with g++:
   ```
   g++ -std=c++17 -o school_system main.cpp
   ```
3. Run it:
   ```
   ./school_system
   ```
4. On first run, a default Admin account is created automatically:
   Username: admin
   Password: 1234

 Notes

- All data is persisted to local `.txt` files, so records survive between runs.
- Role-based access is enforced at login — each user can only access menus matching their assigned role.

Author

Muhammad Zawar Hussain
- GitHub: [@ZK151](https://github.com/ZK151)
- LinkedIn: [muhammad-zawar-hussain](https://www.linkedin.com/in/muhammad-zawar-hussain-477b73319)
