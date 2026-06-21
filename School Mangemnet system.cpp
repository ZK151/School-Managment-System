#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <sstream>
#include <algorithm>
#include <stack>
#include <queue>

using namespace std;

// ==========================================
// 1. LOGIN SYSTEM
// ==========================================
class LoginSystem {
private:
    struct User {
        string username;
        string password;
        string role; // Admin, Staff, Teacher, Student
    };
    vector<User> users;
    string filename = "users.txt";

    int findUserIndex(const string &username) {
        for (size_t i = 0; i < users.size(); i++) {
            if (users[i].username == username) return i;
        }
        return -1;
    }

public:
    void loadFromFile() {
        users.clear();
        ifstream file(filename);
        if (file) {
            string line;
            while (getline(file, line)) {
                if (line.empty()) continue;
                size_t pos1 = line.find(',');
                size_t pos2 = line.rfind(',');
                if (pos1 != string::npos && pos2 != string::npos && pos1 != pos2) {
                    string u = line.substr(0, pos1);
                    string p = line.substr(pos1 + 1, pos2 - pos1 - 1);
                    string r = line.substr(pos2 + 1);
                    users.push_back({u, p, r});
                }
            }
        }

        // Check if Admin exists. If not, create default.
        bool adminExists = false;
        for(const auto &u : users) {
            if(u.username == "admin" && u.role == "Admin") {
                adminExists = true;
                break;
            }
        }

        if (!adminExists) {
            users.push_back({"admin", "1234", "Admin"});
            saveToFile(); 
            cout << "[System] Default Admin created (User: admin, Pass: 1234)\n";
        }
    }

    void saveToFile() {
        ofstream file(filename);
        for (auto &user : users) {
            file << user.username << "," << user.password << "," << user.role << endl;
        }
    }

    bool addUser(const string &username, const string &password, const string &role) {
        if (findUserIndex(username) != -1) {
            cout << "Username exists in Login System!\n";
            return false;
        }
        users.push_back({username, password, role});
        saveToFile();
        // Removed the generic "created" message here to show the specific one in Manager classes
        return true;
    }

    void removeUser(const string &username) {
        int idx = findUserIndex(username);
        if (idx != -1) {
            users.erase(users.begin() + idx);
            cout << "User removed.\n";
        } else {
            cout << "User not found.\n";
        }
    }

    string authenticate(const string &username, const string &password) {
        int idx = findUserIndex(username);
        if (idx != -1 && users[idx].password == password) {
            return users[idx].role;
        }
        return "";
    }

    void displayAllCredentials() {
        cout << "\n--- SYSTEM CREDENTIALS (SENSITIVE DATA) ---\n";
        cout << "Role       | Username   | Password\n";
        cout << "--------------------------------------\n";
        for(const auto& u : users) {
            cout << u.role << " \t| " << u.username << " \t| " << u.password << endl;
        }
        cout << "--------------------------------------\n";
    }
};

// ==========================================
// 2. STUDENT MANAGER
// ==========================================
class StudentManager {
public:
    struct Student {
        int id;
        string name;
        string username;
        int age;
        string grade;
        string section;
        struct Course {
            string courseCode;
            double mark;
            Course* next = nullptr;
        };
        Course* courses = nullptr;
        Student* next = nullptr;
    };

private:
    Student* head = nullptr;
    string filename = "students.txt";

    string studentToString(Student* s) {
        stringstream ss;
        ss << s->id << "," << s->username << "," << s->name << "," << s->age << ","
           << s->grade << "," << s->section << ",";
        Student::Course* c = s->courses;
        bool first = true;
        while (c) {
            if (!first) ss << ";";
            ss << c->courseCode << "-" << c->mark;
            first = false;
            c = c->next;
        }
        return ss.str();
    }

    Student* findStudentById(int id) {
        Student* temp = head;
        while (temp) {
            if (temp->id == id) return temp;
            temp = temp->next;
        }
        return nullptr;
    }

public:
    void loadFromFile() {
        while(head) { Student* t = head; head=head->next; delete t; } 
        ifstream file(filename);
        if (!file) return;
        string line;
        while (getline(file, line)) {
            if (line.empty()) continue;
            stringstream ss(line);
            string idStr, username, name, ageStr, grade, section, coursesStr;

            getline(ss, idStr, ','); if(idStr.empty()) continue; 
            getline(ss, username, ',');
            getline(ss, name, ',');
            getline(ss, ageStr, ',');
            getline(ss, grade, ',');
            getline(ss, section, ',');
            getline(ss, coursesStr);

            try {
                Student* s = new Student{ stoi(idStr), name, username, stoi(ageStr), grade, section, nullptr, nullptr };
                stringstream cs(coursesStr);
                string pair;
                Student::Course* last = nullptr;
                while (getline(cs, pair, ';')) {
                    size_t pos = pair.find('-');
                    if (pos != string::npos) {
                        string courseCode = pair.substr(0, pos);
                        double mark = stod(pair.substr(pos + 1));
                        Student::Course* newCourse = new Student::Course{ courseCode, mark, nullptr };
                        if (!s->courses) s->courses = newCourse;
                        else last->next = newCourse;
                        last = newCourse;
                    }
                }
                s->next = head;
                head = s;
            } catch (...) { continue; }
        }
    }

    void saveToFile() {
        ofstream file(filename);
        Student* temp = head;
        while (temp) {
            file << studentToString(temp) << endl;
            temp = temp->next;
        }
    }

    void addStudent(LoginSystem& loginSys) {
        string username, password;
        cout << "Enter Username for Login: "; 
        cin >> username;
        cout << "Enter Password for Login: "; 
        cin >> password;

        if(!loginSys.addUser(username, password, "Student")) {
            cout << "Cannot add student. Username already taken.\n";
            return;
        }

        Student* s = new Student;
        s->username = username;
        cout << "Enter ID: ";
        cin >> s->id; cin.ignore();
        cout << "Enter Name: "; 
        getline(cin, s->name);
        cout << "Enter Age: "; 
        cin >> s->age; cin.ignore();
        cout << "Enter Grade: "; 
        getline(cin, s->grade);
        cout << "Enter Section: "; 
        getline(cin, s->section);
        s->courses = nullptr;
        s->next = head;
        head = s;
        
        saveToFile();

        // *** DISPLAY CREDENTIALS ***
        cout << "\n----------------------------------------\n";
        cout << " [SUCCESS] Student Added Successfully!\n";
        cout << "----------------------------------------\n";
        cout << " Name:     " << s->name << "\n";
        cout << " Username: " << username << "\n";
        cout << " Password: " << password << "\n";
        cout << "----------------------------------------\n";
        cout << " Please share these credentials with the Student.\n\n";
    }

    void updateStudent() {
        int id; cout << "Enter student ID to update: "; cin >> id; cin.ignore();
        Student* s = findStudentById(id);
        if (s) {
            cout << "Enter Name: "; 
            getline(cin, s->name);
            cout << "Enter Age: "; 
            cin >> s->age; cin.ignore();
            cout << "Enter Grade: "; 
            getline(cin, s->grade);
            cout << "Enter Section: "; 
            getline(cin, s->section);
            cout << "Student updated.\n";
        } else cout << "Student not found.\n";
    }

    void deleteStudent() {
        int id; 
        cout << "Enter student ID to delete: "; 
        cin >> id; 
        cin.ignore();
        Student* temp = head;
        Student* prev = nullptr;
        while (temp) {
            if (temp->id == id) {
                if (prev) prev->next = temp->next;
                else head = temp->next;
                // Delete courses
                Student::Course* c = temp->courses;
                while (c) {
                    Student::Course* nextC = c->next;
                    delete c;
                    c = nextC;
                }
                delete temp;
                cout << "Student deleted.\n";
                return;
            }
            prev = temp;
            temp = temp->next;
        }
        cout << "Student not found.\n";
    }

    void displayAll() {
        if (!head) 
        { 
            cout << "No students found.\n";
             return; 
            }
        Student* temp = head;
        while (temp) {
            cout << "ID:" << temp->id << ", User:" << temp->username << ", Name:" << temp->name
                 << ", Grade:" << temp->grade << endl;
            temp = temp->next;
        }
    }

    void displayStudentGrades(int id) {
        Student* s = findStudentById(id);
        if (!s) 
        { cout << "Student not found.\n";
             return; 
            }
        if (!s->courses) { 
            cout << "No grades found.\n";
             return; 
            }
        cout << "Grades for " << s->name << ":\n";
        Student::Course* c = s->courses;
        while (c) {
            cout << c->courseCode << ": " << c->mark << endl;
            c = c->next;
        }
    }

    void addOrUpdateGrade(int id, const string &course, double grade) {
        Student* s = findStudentById(id);
        if (!s) {
            
            cout << "Student not found.\n";
             return;
             }
        Student::Course* c = s->courses;
        Student::Course* last = nullptr;
        while (c) {
            if (c->courseCode == course)
             {
                 c->mark = grade; 
                 cout << "Grade updated.\n"; 
                 return; 
                }
            last = c;
            c = c->next;
        }
        // Add new course
        Student::Course* newCourse = new Student::Course{ course, grade, nullptr };
        if (!s->courses) 
        s->courses = newCourse;
        else 
        last->next = newCourse;
        cout << "Grade updated.\n";
    }

    string getStudentName(int id) {
        Student* s = findStudentById(id);
        if(s) 
        return s->name;
        return "Unknown";
    }

    Student* getAllStudents() { return head; }
};

// ==========================================
// 3. TEACHER MANAGER
// ==========================================
class TeacherManager {
public:
    struct Teacher {
        int id;
        string username;
        string name;
        struct CourseNode {
            string courseCode;
            CourseNode* next = nullptr;
        };
        CourseNode* courses = nullptr;
    };

private:
    stack<Teacher> teachers;
    string filename = "teachers.txt";

    string teacherToString(const Teacher &t) {
        stringstream ss;
        ss << t.id << "," << t.username << "," << t.name << ",";
        Teacher::CourseNode* c = t.courses;
        bool first = true;
        while (c) {
            if (!first) ss << ";";
            ss << c->courseCode;
            first = false;
            c = c->next;
        }
        return ss.str();
    }

public:
    void loadFromFile() {
        stack<Teacher> tempStack;
        ifstream file(filename);
        if (!file) return;

        string line;
        while (getline(file, line)) {
            if (line.empty()) continue;
            stringstream ss(line);
            string idStr, username, name, coursesStr;

            getline(ss, idStr, ','); if(idStr.empty()) continue;
            getline(ss, username, ','); 
            getline(ss, name, ',');
            getline(ss, coursesStr);

            try {
                Teacher t{ stoi(idStr), username, name, nullptr };
                stringstream cs(coursesStr);
                string course;
                while (getline(cs, course, ';')) {
                    if (!course.empty()) {
                        Teacher::CourseNode* cNode = new Teacher::CourseNode{ course, nullptr };
                        if (!t.courses) t.courses = cNode;
                        else {
                            Teacher::CourseNode* c = t.courses;
                            while (c->next) c = c->next;
                            c->next = cNode;
                        }
                    }
                }
                tempStack.push(t);
            } catch(...) { continue; }
        }
        teachers = tempStack; 
    }

    void saveToFile() {
        stack<Teacher> tempStack = teachers;
        ofstream file(filename);
        while (!tempStack.empty()) {
            file << teacherToString(tempStack.top()) << endl;
            tempStack.pop();
        }
    }

    void addTeacher(LoginSystem& loginSys) {
        string username, password;
        cout << "Enter Username for Login: "; 
        cin >> username;
        cout << "Enter Password for Login: "; 
        cin >> password;

        if(!loginSys.addUser(username, password, "Teacher")) {
            cout << "Cannot add teacher. Username taken.\n";
            return;
        }

        Teacher t;
        t.username = username;
        cout << "Enter ID: "; 
        cin >> t.id; 
        cin.ignore();
        cout << "Enter Name: "; 
        getline(cin, t.name);
        t.courses = nullptr;
        teachers.push(t);
        
        saveToFile();

        // *** DISPLAY CREDENTIALS FOR ADMIN ***
        cout << "\n----------------------------------------\n";
        cout << " [SUCCESS] Teacher Added Successfully!\n";
        cout << "----------------------------------------\n";
        cout << " ID:       " << t.id << "\n";
        cout << " Name:     " << t.name << "\n";
        cout << " Username: " << username << "\n";
        cout << " Password: " << password << "\n";
        cout << "----------------------------------------\n";
        cout << " Please share these credentials with the Teacher.\n\n";
    }

    void updateTeacher() {
        int id; cout << "Enter teacher ID to update: "; 
        cin >> id; 
        cin.ignore();
        stack<Teacher> tempStack;
        bool found = false;
        while (!teachers.empty()) {
            Teacher t = teachers.top(); 
            teachers.pop();
            if (t.id == id) {
                cout << "Enter Name: "; 
                getline(cin, t.name);
                found = true;
            }
            tempStack.push(t);
        }
        while (!tempStack.empty()) 
        { teachers.push(tempStack.top()); 
            tempStack.pop(); }
        if (found) 
        cout << "Teacher updated.\n"; 
        else cout << "Teacher not found.\n";
    }

    void deleteTeacher() {
        int id; 
        cout << "Enter teacher ID to delete: "; 
        cin >> id; 
        cin.ignore();
        stack<Teacher> tempStack;
        bool found = false;
        while (!teachers.empty()) {
            Teacher t = teachers.top(); 
            teachers.pop();
            if (t.id == id) { 
                found = true; 
                continue; } 
            tempStack.push(t);
        }
        while (!tempStack.empty()) 
        { 
            teachers.push(tempStack.top()); 
            tempStack.pop(); 
        }
        if (found) cout << "Teacher deleted.\n"; 
        else cout << "Teacher not found.\n";
    }

    void displayAll() {
        if (teachers.empty()) 
        { 
            cout << "No teachers found.\n";
             return; 
            }
        stack<Teacher> tempStack = teachers;
        while (!tempStack.empty()) {
            Teacher& t = tempStack.top();
            cout << "ID:" << t.id << ", User:" << t.username << ", Name:" << t.name << endl;
            tempStack.pop();
        }
    }

    int getTeacherID(const string &username) {
        stack<Teacher> tempStack = teachers;
        int foundID = -1;
        while (!tempStack.empty()) {
            if (tempStack.top().username == username) {
                foundID = tempStack.top().id;
                break;
            }
            tempStack.pop();
        }
        return foundID;
    }

    stack<Teacher>& getAllTeachers() {
         return teachers;
         }
};

// ==========================================
// 4. CLASSROOM MANAGER
// ==========================================
struct TeacherAssignment {
    string className;
    string sectionName;
    string subjectName;
};

class ClassRoomManager {
private:
    struct Subject { 
        string name;
         int teacherID;
         };
    struct Section { 
        string name; 
        queue<int> studentIDs; 
        queue<Subject> subjects;
    };
    struct ClassRoom { 
        string name; 
        queue<Section> sections; 
    };
    queue<ClassRoom> classes;
    string filename = "classrooms.txt";

    string subjectToString(const Subject &s) {
         return s.name + "-" + to_string(s.teacherID);
         }

    string sectionToString(Section sec) {
        stringstream ss; 
        ss << sec.name << ",";
        queue<int> tempStu = sec.studentIDs;
        bool first = true;
        while (!tempStu.empty()) {
             if (!first) ss << ";"; 
             ss << tempStu.front(); 
             tempStu.pop(); 
             first = false; }
        ss << ",";
        queue<Subject> tempSub = sec.subjects;
        first = true;
        while (!tempSub.empty()) {
             if (!first) ss << ";"; 
             ss << subjectToString(tempSub.front()); 
             tempSub.pop(); 
             first = false; }
        return ss.str();
    }

    string classToString(ClassRoom c) {
        stringstream ss; 
        ss << c.name << ",";
        queue<Section> tempSec = c.sections;
        bool first = true;
        while (!tempSec.empty()) {
             if (!first) ss << "|"; 
             ss << sectionToString(tempSec.front()); 
             tempSec.pop(); 
             first = false; 
            }
        return ss.str();
    }

public:
    void loadFromFile() {
        while(!classes.empty()) classes.pop();
        ifstream file(filename);
        if (!file) return;
        string line;
        while (getline(file, line)) {
            if (line.empty()) continue;
            stringstream ss(line);
            string className, sectionsStr;
            getline(ss, className, ','); 
            getline(ss, sectionsStr);
            ClassRoom c; 
            c.name = className;
            stringstream secStream(sectionsStr);
            string secPart;
            while (getline(secStream, secPart, '|')) {
                stringstream ssPart(secPart);
                string secName, stuStr, subStr;
                getline(ssPart, secName, ','); 
                getline(ssPart, stuStr, ','); 
                getline(ssPart, subStr, ',');
                Section sec; sec.name = secName;
                stringstream stID(stuStr);
                string sID;
                while (getline(stID, sID, ';')) 
                if (!sID.empty()) 
                sec.studentIDs.push(stoi(sID));
                stringstream subSS(subStr);
                string subP;
                while (getline(subSS, subP, ';')) {
                    size_t pos = subP.find('-');
                    if (pos != string::npos) 
                    sec.subjects.push({ subP.substr(0, pos), 
                        stoi(subP.substr(pos + 1)) });
                }
                c.sections.push(sec);
            }
            classes.push(c);
        }
    }

    void saveToFile() {
        ofstream file(filename);
        queue<ClassRoom> tempClasses = classes;
        while (!tempClasses.empty()) { 
            file << classToString(tempClasses.front()) << endl; 
            tempClasses.pop();
         }
    }

    void addClass(StudentManager &sm, TeacherManager &tm) {
        ClassRoom c; 
        cout << "Enter Class Name: "; 
        getline(cin, c.name);
        int secCount; 
        cout << "Number of Sections: "; 
        cin >> secCount; 
        cin.ignore();
        
        for (int i = 0; i < secCount; i++) {
            Section sec; 
            cout << "Section " << i+1 << " Name: "; 
            getline(cin, sec.name);
            
            // Add Students
            int stuCount; 
            cout << "Number of Students in Section: "; 
            cin >> stuCount; 
            cin.ignore();
            for (int j = 0; j < stuCount; j++) {
                int sid; 
                cout << "Student " << j+1 << " ID: "; 
                cin >> sid; 
                cin.ignore();
                StudentManager::Student* current = sm.getAllStudents();
                bool found = false;
                while (current != nullptr) { 
                    if (current->id == sid) 
                    { 
                        found = true; 
                        break; 
                    } 
                    current = current->next; 
                
                }
                if (found) 
                sec.studentIDs.push(sid);
                else 
                cout << "Student ID " << sid << " not found.\n";
            }

            // Add Subjects and Assign Teachers
            int subCount; cout << "Number of Subjects in Section: "; 
            cin >> subCount; 
            cin.ignore();
            for (int k = 0; k < subCount; k++) {
                Subject sub; 
                cout << "Subject " << k+1 << " Name: "; 
                getline(cin, sub.name);
                int tid; cout << "Assigned Teacher ID: "; 
                cin >> tid; cin.ignore();
                
                // Verify Teacher Exists
                bool found = false;
                stack<TeacherManager::Teacher> tStack = tm.getAllTeachers();
                while (!tStack.empty()) { 
                    if (tStack.top().id == tid) {
                         found = true; break; 
                        } 
                    tStack.pop(); 
                }

                if (found) {
                    sub.teacherID = tid; 
                    sec.subjects.push(sub);
                    cout << "Teacher ID " << tid << " assigned to " << sub.name << ".\n";
                } 
                else {
                    cout << "Error: Teacher ID " << tid << " does not exist. Subject not added.\n";
                }
            }
            c.sections.push(sec);
        }
        classes.push(c);
        cout << "Class added.\n";
    }

    void displayAll() {
        if (classes.empty()) { cout << "No classes.\n"; return; }
        queue<ClassRoom> tempClasses = classes;
        while (!tempClasses.empty()) {
            ClassRoom &c = tempClasses.front();
            cout << "Class: " << c.name << "\n";
            queue<Section> tempSections = c.sections;
            while (!tempSections.empty()) {
                Section &sec = tempSections.front();
                cout << " Section: " << sec.name << "\n  Students IDs: ";
                queue<int> tempStu = sec.studentIDs;
                while (!tempStu.empty()) 
                { 
                    cout << tempStu.front() << " "; 
                    tempStu.pop(); 
                }
                cout << "\n  Subjects: ";
                queue<Subject> tempSub = sec.subjects;
                while (!tempSub.empty()) { 
                    Subject &sub = tempSub.front();
                     cout << sub.name << "(TID:" << sub.teacherID << ") "; 
                     tempSub.pop(); 
                    }
                cout << endl;
                tempSections.pop();
            }
            tempClasses.pop();
        }
    }

    vector<TeacherAssignment> getTeacherAssignments(int tid) {
        vector<TeacherAssignment> assignments;
        queue<ClassRoom> tempC = classes;
        while(!tempC.empty()) {
            ClassRoom c = tempC.front(); 
            tempC.pop();
            queue<Section> tempS = c.sections;
            while(!tempS.empty()) {
                Section s = tempS.front(); 
                tempS.pop();
                queue<Subject> tempSub = s.subjects;
                while(!tempSub.empty()) {
                    Subject sub = tempSub.front(); 
                    tempSub.pop();
                    if(sub.teacherID == tid) {
                        assignments.push_back({c.name, s.name, sub.name});
                    }
                }
            }
        }
        return assignments;
    }

    vector<int> getStudentsInSpecificSection(string clsName, string secName) {
        vector<int> studentList;
        queue<ClassRoom> tempC = classes;
        while(!tempC.empty()) {
            ClassRoom c = tempC.front(); 
            tempC.pop();
            if(c.name == clsName) {
                queue<Section> tempS = c.sections;
                while(!tempS.empty()) {
                    Section s = tempS.front(); 
                    tempS.pop();
                    if(s.name == secName) {
                        queue<int> stuQ = s.studentIDs;
                        while(!stuQ.empty()) {
                            studentList.push_back(stuQ.front());
                            stuQ.pop();
                        }
                        return studentList;
                    }
                }
            }
        }
        return studentList;
    }
};

// ==========================================
// 5. ATTENDANCE MANAGER
// ==========================================
class AttendanceManager {
private:
    struct StudentAttendance { int studentID; vector<int> records; };
    vector<StudentAttendance> attendance;
    string filename = "attendance.txt";

    int findStudentIndex(int studentID) {
        int left = 0, right = attendance.size() - 1;
        while (left <= right) {
            int mid = left + (right - left) / 2;
            if (attendance[mid].studentID == studentID) 
            return mid;
            else if (attendance[mid].studentID < studentID) 
            left = mid + 1;
            else 
            right = mid - 1;
        }
        return -1;
    }

public:
    void loadFromFile() {
        attendance.clear();
        ifstream file(filename);
        if (!file) 
        return;
        string line;
        while (getline(file, line)) {
            if (line.empty()) 
            continue;
            size_t pos = line.find(':');
            int studentID = stoi(line.substr(0, pos));
            string data = line.substr(pos + 1);
            vector<int> records;
            size_t start = 0,
             end;
            while ((end = data.find(',', start)) != string::npos) {
                records.push_back(stoi(data.substr(start, end - start)));
                start = end + 1;
            }
            if (start < data.size())
            records.push_back(stoi(data.substr(start)));
            attendance.push_back({studentID, records});
        }
        sort(attendance.begin(), attendance.end(), [](const StudentAttendance &a, const StudentAttendance &b){
            return a.studentID < b.studentID;
        }
    );
    }

    void saveToFile() {
        ofstream file(filename);
        for (auto &s : attendance) {
            file << s.studentID << ":";
            for (size_t i = 0; i < s.records.size(); i++) {
                file << s.records[i];
                if (i != s.records.size() - 1)
                 file << ",";
            }
            file << endl;
        }
    }

    void markAttendance(int studentID, bool present) {
        int idx = findStudentIndex(studentID);
        if (idx != -1) {
            attendance[idx].records.push_back(present ? 1 : 0);
        } else {
            StudentAttendance s{studentID,
                 {present ? 1 : 0}
                };
            auto it = lower_bound(attendance.begin(), attendance.end(), s, [](const StudentAttendance &a, const StudentAttendance &b){ return a.studentID < b.studentID; });
            attendance.insert(it, s);
        }
        cout << "Attendance marked.\n";
    }

    void displayStudentAttendance(int studentID) {
        int idx = findStudentIndex(studentID);
        if (idx == -1) { 
            cout << "No attendance records.\n";
             return; 
            }
        cout << "Attendance for ID " << studentID << ": ";
        for (auto a : attendance[idx].records)
         cout << (a ? "P " : "A ");
        cout << endl;
    }
};

// ==========================================
// 6. LIBRARY MANAGER
// ==========================================
struct Book {
    int id; string title; string author; bool available;
    bool operator<(const Book &other) const { return id > other.id; }
};

class LibraryManager {
private:
    vector<Book> heap;
    string filename = "library.txt";

public:
    void loadFromFile() {
        heap.clear();
        ifstream file(filename);
        if (!file) return;
        Book b;
        while (file >> b.id) {
            file.ignore(); 
            getline(file, b.title); 
            getline(file, b.author);
            file >> b.available; 
            file.ignore(); 
            heap.push_back(b);
        }
        make_heap(heap.begin(), heap.end());
    }

    void saveToFile() {
        ofstream file(filename);
        for (auto &b : heap) {
            file << b.id << "\n" << b.title << "\n" << b.author << "\n" << b.available << "\n";
        }
    }

    void addBook() {
        Book b; 
        cout << "Enter Book ID: "; cin >> b.id; 
        cin.ignore();
        cout << "Enter Title: "; 
        getline(cin, b.title);
        cout << "Enter Author: "; 
        getline(cin, b.author);
        b.available = true;
        heap.push_back(b); 
        push_heap(heap.begin(), heap.end());
        cout << "Book added.\n";
        saveToFile();
    }

    void borrowBook(int id) {
        auto it = find_if(heap.begin(), heap.end(), [&](Book &b){ return b.id == id; });
        if (it != heap.end()) {
            if (!it->available) { cout << "Book borrowed.\n";
                 return;
                 }
            it->available = false; 
            cout << "Book borrowed.\n"; 
            saveToFile();
        } else 
        cout << "Book not found.\n";
    }

    void returnBook(int id) {
        auto it = find_if(heap.begin(), heap.end(), [&](Book &b){ return b.id == id; });
        if (it != heap.end()) {
            it->available = true; 
            cout << "Book returned.\n"; 
            saveToFile();
        } else 
        cout << "Book not found.\n";
    }

    void displayAllBooks() {
        if (heap.empty()) {
             cout << "No books.\n"; 
             return; 
            }
        vector<Book> temp = heap; 
        sort_heap(temp.begin(), temp.end());
        for (auto &b : temp) cout << "ID: " << b.id << " | " << b.title << " | " << (b.available ? "Avail" : "Borrowed") << endl;
    }
};

// ==========================================
// 7. FEE MANAGER
// ==========================================
class FeeManager {
private:
    struct FeeRecord {
        int studentID; 
        double amount; 
        bool paid;
        FeeRecord* left; 
        FeeRecord* right;
        FeeRecord(int id, double amt, bool p) : studentID(id), amount(amt), paid(p), left(nullptr), right(nullptr) {}
    };
    FeeRecord* root = nullptr;
    string filename = "fees.txt";

    FeeRecord* insert(FeeRecord* node, int id, double amt, bool paid) {
        if (!node)
         return new FeeRecord(id, amt, paid);
        if (id < node->studentID) 
        node->left = insert(node->left, id, amt, paid);
        else if (id > node->studentID)
         node->right = insert(node->right, id, amt, paid);
        else {
             node->amount = amt; 
             node->paid = paid;
             }
        return node;
    }
    FeeRecord* search(FeeRecord* node, int id) {
        if (!node) 
        return nullptr;
        if (node->studentID == id) 
        return node;
        if (id < node->studentID) 
        return search(node->left, id);
        return search(node->right, id);
    }
    void inOrderSave(FeeRecord* node, ofstream &file) {
        if (!node) return;
        inOrderSave(node->left, file);
        file << node->studentID << " " << node->amount << " " << node->paid << "\n";
        inOrderSave(node->right, file);
    }
    void deleteTree(FeeRecord* node) { if (!node) return; deleteTree(node->left); deleteTree(node->right); delete node; }

public:
    ~FeeManager() { deleteTree(root); }
    void loadFromFile() {
        deleteTree(root); 
        root = nullptr; 
        ifstream file(filename);
        if (!file) 
        return;
        int id; 
        double amt; 
        bool paid;
        while (file >> id >> amt >> paid) 
        root = insert(root, id, amt, paid);
    }
    void saveToFile() {
         ofstream file(filename); 
         inOrderSave(root, file); 
        }
    void addFeeRecord() {
        int id; 
        double amt; 
        cout << "Student ID: "; 
        cin >> id; cout << "Amount: "; 
        cin >> amt; 
        cin.ignore();
        root = insert(root, id, amt, false); 
        cout << "Fee added.\n"; saveToFile();
    }
    void payFee(int studentID) {
        FeeRecord* node = search(root, studentID);
        if (!node) {
            cout << "Record not found.\n"; 
            return; 
        }
        node->paid = true; 
        cout << "Fee paid.\n"; 
        saveToFile();
    }
    void displayFeeStatus(int studentID) {
        FeeRecord* node = search(root, studentID);
        if (!node) { 
            cout << "Record not found.\n";
             return; 
            }
        cout << "Amt: " << node->amount << ", Status: " << (node->paid ? "Paid" : "Pending") << endl;
    }
};

// ==========================================
// 8. ANNOUNCEMENT MANAGER
// ==========================================
class AnnouncementManager {
private:
    struct Node {
        string announcement;
         Node* left; 
         Node* right; 
         int height;
        Node(const string &ann) : announcement(ann), left(nullptr), right(nullptr), height(1) {}
    };
    Node* root = nullptr;
    string filename = "announcements.txt";

    int height(Node* n) { return n ? n->height : 0; }
    Node* insert(Node* node, const string &ann) {
        if (!node) 
        return new Node(ann);
        if (ann < node->announcement) 
        node->left = insert(node->left, ann);
        else if (ann > node->announcement) 
        node->right = insert(node->right, ann);
        return node; 
    }
    void inOrder(Node* node)
     { 
        if (!node) return; 
        inOrder(node->left); 
        cout << "- " << node->announcement << endl; 
        inOrder(node->right); 
    }
    void inOrderSave(Node* node, ofstream &file)
     { 
        if
        (!node) return; 
        inOrderSave(node->left, file); 
        file << node->announcement << endl; 
        inOrderSave(node->right, file);
     }
    void deleteTree(Node* node) 
    { if (!node) 
        return;
         deleteTree(node->left); 
         deleteTree(node->right); 
         delete node; }

public:
    ~AnnouncementManager() {
         deleteTree(root);
         }
    void loadFromFile() {
        deleteTree(root); 
        root = nullptr; 
        ifstream file(filename);
        string line; 
        while (getline(file, line)) 
        if (!line.empty()) root = insert(root, line);
    }
    void saveToFile() { 
        ofstream file(filename); 
        inOrderSave(root, file); 
    }
    void addAnnouncement() {
        string ann; 
        cin.ignore(); 
        cout << "Enter announcement: "; 
        getline(cin, ann);
        root = insert(root, ann); 
        cout << "Added.\n"; 
        saveToFile();
    }
    void displayAnnouncements() {
         cout << "=== Announcements ===\n";
          inOrder(root); 
        }
};

// ==========================================
// MAIN FUNCTION
// ==========================================
int main() {
    LoginSystem login;
    StudentManager studentManager;
    TeacherManager teacherManager;
    ClassRoomManager classManager;
    AttendanceManager attendanceManager;
    LibraryManager libraryManager;
    FeeManager feeManager;
    AnnouncementManager announcementManager;

    // Load all data once
    login.loadFromFile();
    studentManager.loadFromFile();
    teacherManager.loadFromFile();
    classManager.loadFromFile();
    attendanceManager.loadFromFile();
    libraryManager.loadFromFile();
    feeManager.loadFromFile();
    announcementManager.loadFromFile();

    cout << "====== SCHOOL MANAGEMENT SYSTEM ======\n";

    bool exitProgram = false;
    while(!exitProgram) {
        int userType;
        cout << "\nWho are you?\n1. Admin\n2. Teacher\n3. Student\n4. Librarian\n5. Exit Program\nChoice: ";
        cin >> userType; 
        cin.ignore();

        if(userType == 5) {
            exitProgram = true;
            break;
        }

        string expectedRole;
        switch(userType) {
            case 1: expectedRole = "Admin"; 
            break;
            case 2: expectedRole = "Teacher";
             break;
            case 3: expectedRole = "Student"; 
            break;
            case 4: expectedRole = "Librarian"; 
            break;
            default: cout << "Invalid choice!\n"; 
            continue;
        }

        string username, password;
        cout << "Username: "; 
        cin >> username;
        cout << "Password: "; 
        cin >> password;

        string role = login.authenticate(username, password);
        if(role.empty()) { 
            cout << "Invalid credentials!\n"; 
            continue; 
        }
        if(role != expectedRole) { 
            cout << "You are not authorized as " << expectedRole << "!\n"; 
            continue; 
        }

        cout << "Login successful! Welcome, " << username << " (" << role << ")\n";

        bool running = true;
        while(running) {
            if(role == "Admin") {
                cout << "\n=== Admin Menu ===\n";
                cout << "1. Add User\n2. Remove User\n3. Add Teacher\n4. Update Teacher\n5. Delete Teacher\n";
                cout << "6. Add Class\n7. Display All Classes\n8. Add Announcement\n9. Display Announcements\n";
                cout << "10. Add Student\n11. Update Student\n12. Delete Student\n13. Display All Students\n";
                cout << "14. Add Fee Record\n15. Display Fee Status\n16. Pay Fee\n17. View User Credentials\n";
                cout << "0. Logout\n";

                int choice; cout << "Choice: "; 
                cin >> choice; 
                cin.ignore();

                switch(choice) {
                    case 1: {
                        string u, p, r;
                        cout << "Username: "; 
                        cin >> u;
                        cout << "Password: "; 
                        cin >> p;
                        cout << "Role (Admin/Teacher/Student/Librarian): "; 
                        cin >> r;
                        login.addUser(u, p, r); 
                        login.saveToFile();
                        break;
                    }
                    case 2: {
                        string u; 
                        cout << "Username to remove: "; 
                        cin >> u;
                        login.removeUser(u); 
                        login.saveToFile();
                        break;
                    }
                    case 3: teacherManager.addTeacher(login); 
                    teacherManager.saveToFile(); 
                    break;
                    case 4: teacherManager.updateTeacher(); 
                    teacherManager.saveToFile(); 
                    break;
                    case 5: teacherManager.deleteTeacher(); 
                    teacherManager.saveToFile(); 
                    break;
                    case 6: classManager.addClass(studentManager, teacherManager); 
                    classManager.saveToFile(); 
                    break;
                    case 7: classManager.displayAll();
                    break;
                    case 8: announcementManager.addAnnouncement(); 
                    announcementManager.saveToFile(); 
                    break;
                    case 9: announcementManager.displayAnnouncements(); 
                    break;
                    case 10: studentManager.addStudent(login); 
                    studentManager.saveToFile(); break;
                    case 11: studentManager.updateStudent(); 
                    studentManager.saveToFile(); 
                    break;
                    case 12: studentManager.deleteStudent(); 
                    studentManager.saveToFile(); 
                    break;
                    case 13: studentManager.displayAll(); 
                    break;
                    case 14: feeManager.addFeeRecord(); 
                    feeManager.saveToFile(); 
                    break;
                    case 15: {
                        int sid; cout << "Enter Student ID: "; 
                        cin >> sid;
                        feeManager.displayFeeStatus(sid); 
                        break;
                    }
                    case 16: {
                        int sid; cout << "Enter Student ID: "; 
                        cin >> sid;
                        feeManager.payFee(sid); 
                        feeManager.saveToFile(); 
                        break;
                    }
                    case 17: login.displayAllCredentials(); 
                    break;
                    case 0: running = false; 
                    break;
                    default: cout << "Invalid choice!\n";
                }
            }
            else if(role == "Teacher") {
                // 1. Get Teacher ID
                int tid = teacherManager.getTeacherID(username);
                if(tid == -1) {
                    cout << "Error: Teacher profile not found.\n";
                    running = false; break;
                }

                // 2. Get Assigned Classes
                vector<TeacherAssignment> assignments = classManager.getTeacherAssignments(tid);
                
                // 3. Strict Check: If no assignments, block features
                if(assignments.empty()) {
                    cout << "\n[ALERT] You have not been assigned to any classes yet.\n";
                    cout << "Please contact the Administrator.\n";
                    cout << "1. Logout\nChoice: ";
                    int dummy; 
                    cin >> dummy; 
                    cin.ignore();
                    running = false;
                    break; 
                }

                cout << "\n=== Teacher Menu ===\n";
                cout << "1. View Assigned Classes\n2. Give Grades\n3. Mark Attendance\n4. Display All Classes\n0. Logout\n";
                int choice; 
                cout << "Choice: "; 
                cin >> choice; 
                cin.ignore();
                
                switch(choice) {
                    case 1: {
                        cout << "--- Your Assignments ---\n";
                        for(const auto& a : assignments) {
                            cout << "Class: " << a.className << " | Section: " << a.sectionName << " | Subject: " << a.subjectName << endl;
                        }
                        break;
                    }
                    case 2: { // Give Grades
                        cout << "Select Class/Section to Grade:\n";
                        for(size_t i=0; i<assignments.size(); ++i) {
                            cout << i+1 << ". " << assignments[i].className << " (" << assignments[i].sectionName << ") - " << assignments[i].subjectName << endl;
                        }
                        cout << "Enter Number: ";
                        int sel; cin >> sel; cin.ignore();
                        if(sel > 0 && sel <= (int)assignments.size()) {
                            TeacherAssignment ta = assignments[sel-1];
                            vector<int> sids = classManager.getStudentsInSpecificSection(ta.className, ta.sectionName);
                            if(sids.empty()) { cout << "No students in this section.\n"; break; }
                            
                            cout << "--- Grading for " << ta.subjectName << " ---\n";
                            for(int sid : sids) {
                                cout << "Enter Grade for Student ID " << sid << " (" << studentManager.getStudentName(sid) << "): ";
                                double gr; cin >> gr; cin.ignore();
                                studentManager.addOrUpdateGrade(sid, ta.subjectName, gr);
                            }
                            studentManager.saveToFile();
                            cout << "Grades saved.\n";
                        } else cout << "Invalid selection.\n";
                        break;
                    }
                    case 3: { // Mark Attendance
                        cout << "Select Class/Section for Attendance:\n";
                        for(size_t i=0; i<assignments.size(); ++i) {
                            cout << i+1 << ". " << assignments[i].className << " (" << assignments[i].sectionName << ") - " << assignments[i].subjectName << endl;
                        }
                        cout << "Enter Number: ";
                        int sel; cin >> sel; cin.ignore();
                        if(sel > 0 && sel <= (int)assignments.size()) {
                            TeacherAssignment ta = assignments[sel-1];
                            vector<int> sids = classManager.getStudentsInSpecificSection(ta.className, ta.sectionName);
                            if(sids.empty()) { cout << "No students in this section.\n"; break; }
                            
                            cout << "--- Attendance for " << ta.className << " " << ta.sectionName << " ---\n";
                            for(int sid : sids) {
                                char p;
                                cout << "Is " << studentManager.getStudentName(sid) << " (ID:" << sid << ") Present? (y/n): ";
                                cin >> p;
                                attendanceManager.markAttendance(sid, (p=='y'||p=='Y'));
                            }
                            attendanceManager.saveToFile();
                            cout << "Attendance saved.\n";
                        } else cout << "Invalid selection.\n";
                        break;
                    }
                    case 4: classManager.displayAll(); break;
                    case 0: running = false; break;
                    default: cout << "Invalid choice!\n";
                }
            }
            else if(role == "Student") {
                cout << "\n=== Student Menu ===\n";
                cout << "1. View Grades\n2. View Attendance\n3. View Announcements\n4. View Fee Status\n0. Logout\n";
                int choice; 
                cout << "Choice: "; 
                cin >> choice; 
                cin.ignore();

                int sid = -1;
                StudentManager::Student* current = studentManager.getAllStudents();
                while(current != nullptr) {
                    if(current->username == username) {
                        sid = current->id;
                        break;
                    }
                    current = current->next;
                }

                switch(choice) {
                    case 1:
                     if(sid!=-1) 
                    studentManager.displayStudentGrades(sid);
                     else cout << "Student ID not found for user.\n";
                      break;
                    case 2:
                     if(sid!=-1) attendanceManager.displayStudentAttendance(sid);
                      else cout << "Student ID not found for user.\n";
                       break;
                    case 3:
                     announcementManager.displayAnnouncements(); 
                        break;
                    case 4: 
                    if(sid!=-1)
                     feeManager.displayFeeStatus(sid); 
                    else 
                     cout << "Student ID not found for user.\n"; 
                    break;
                    case 0: running = false;
                     break;
                    default: cout << "Invalid choice!\n";
                }
            }
            else if(role == "Librarian") {
                cout << "\n=== Librarian Menu ===\n";
                cout << "1. Add Book\n2. Issue Book\n3. Return Book\n4. Display Books\n0. Logout\n";
                int choice; 
                cout << "Choice: "; 
                cin >> choice; 
                cin.ignore();

                switch(choice) {
                    case 1: libraryManager.addBook(); 
                    libraryManager.saveToFile(); 
                    break;
                    case 2: {
    
                                int sid;
                                cout << "Enter Student ID to issue book to: ";
                                cin >> sid;
                                cin.ignore();

                             
                                string sName = studentManager.getStudentName(sid);

                                if (sName == "Unknown") {
                                    cout << "\n[ERROR] Student with ID " << sid << " not found!\n";
                                    cout << "Cannot issue book.\n";
                                } 
                                else {
                                    // 3. Student found - Proceed to give book
                                    cout << "\n[VERIFIED] Student Name: " << sName << "\n";
                                    
                                    int bid;
                                    cout << "Enter Book ID to borrow: ";
                                    cin >> bid;
                                    cin.ignore();
                                    
                                   
                                    libraryManager.borrowBook(bid);
                                    libraryManager.saveToFile();
                                }
                                break;
                            }
                                                case 3: {
                        int bid; 
                        cout << "Book ID to return: "; 
                        cin >> bid; 
                        cin.ignore();
                        libraryManager.returnBook(bid); 
                        libraryManager.saveToFile(); 
                        
                        break;
                    }
                    case 4: libraryManager.displayAllBooks(); 
                    break;
                    case 0: running = false; 
                    break;
                    default: cout << "Invalid choice!\n";
                }
            }
        }
        cout << "Logged out.\n";
    }

    cout << "Exiting program. Goodbye!\n";
    return 0;
}