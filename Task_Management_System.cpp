#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <chrono>
#include <format>
#include <ctime>
#include <iomanip>
#include <sstream>
#include <mutex>

using namespace std;


/*

Design a Task Management System

Requirements

The task management system should allow users to create, update, and delete tasks.
Each task should have a title, description, due date, priority, and status (e.g., pending, in progress, completed).
Users should be able to assign tasks to other users and set reminders for tasks.
The system should support searching and filtering tasks based on various criteria (e.g., priority, due date, assigned user).
Users should be able to mark tasks as completed and view their task history.
The system should handle concurrent access to tasks and ensure data consistency.
The system should be extensible to accommodate future enhancements and new features.

*/

/*

Functional Requirements

Create task.
Update task.
Delete task.
Each task has a title, description, due date, priority, and status.
Assign a task to another user.
Set reminders for tasks.
Search tasks.
Filter tasks by priority, due date, or assigned user.
Mark a task as completed.
View task history.


Non-Functional Requirements

Handle concurrent access.
Maintain data consistency.
Extensible.
Maintainable.

*/

/*

Workflow : 

User
   │
   ▼
Create Task
   │
   ▼
Enter Task Details
(Title, Description, Due Date, Priority)
   │
   ▼
Assign User (Optional)
   │
   ▼
Set Reminder (Optional)
   │
   ▼
Save Task
   │
   ▼
Search / Filter Tasks (Optional)
   │
   ▼
Update Task (Optional)
   │
   ▼
Mark Task Completed
   │
   ▼
Store in Task History

*/


class User{
private:
    int UserID;
    string name;
public:
    User(int uid , string n){
        UserID = uid;
        name = n;
    }
    int getUserID(){
        return UserID;
    }
    string getUserName(){
        return name;
    }
};

enum STATUS {
    PENDING,
    IN_PROGRESS,
    COMPLETED
};

enum PRIORITY {
    URGENT,
    NORMAL
};

class Task {
private:
    int taskID;
    string title;
    string description;
    string dueDate;       // DD/MM/YYYY
    string reminderTime;  // Optional

    PRIORITY priority;
    STATUS status;

    User* assignedUser;

public:
    Task(int tid, string t, string d, string dd) {
        taskID = tid;
        title = t;
        description = d;
        dueDate = dd;

        reminderTime = "";
        priority = NORMAL;
        status = PENDING;
        assignedUser = nullptr;
    }

    int getTaskID() const {
        return taskID;
    }

    string getTitle() const {
        return title;
    }

    string getDescription() const {
        return description;
    }

    string getDueDate() const {
        return dueDate;
    }

    string getReminderTime() const {
        return reminderTime;
    }

    PRIORITY getPriority() const {
        return priority;
    }

    STATUS getStatus() const {
        return status;
    }

    User* getAssignedUser() const {
        return assignedUser;
    }

    void setPriority(PRIORITY p) {
        priority = p;
    }

    void setStatus(STATUS s) {
        status = s;
    }

    void setReminder(string reminder) {
        reminderTime = reminder;
    }

    void assignUser(User* user) {
        assignedUser = user;
    }
};


class TaskManager {
private:
    unordered_map<int, deque<Task*>> tasks;   // UserID -> Tasks
    vector<Task*> completedTasks;
    mutex mtx;

public:
    TaskManager() {}

    void assignTask(int userID, Task* task) {

        lock_guard<mutex> lock(mtx);

        if (task->getPriority() == URGENT)
            tasks[userID].push_front(task);
        else
            tasks[userID].push_back(task);

        task->assignUser(new User(userID, "Assigned User"));
    }

    void removeTask(int userID, int taskID) {
        lock_guard<mutex> lock(mtx);
        auto &userTasks = tasks[userID];
        for (auto it = userTasks.begin(); it != userTasks.end(); ++it) {
            if ((*it)->getTaskID() == taskID) {
                userTasks.erase(it);
                return;
            }
        }
    }

    void markCompleted(int userID, int taskID) {
        lock_guard<mutex> lock(mtx);
        auto &userTasks = tasks[userID];
        for (auto it = userTasks.begin(); it != userTasks.end(); ++it) {
            if ((*it)->getTaskID() == taskID) {
                (*it)->setStatus(COMPLETED);
                completedTasks.push_back(*it);
                userTasks.erase(it);
                return;
            }
        }
    }

    void showTasks(int userID) {
        lock_guard<mutex> lock(mtx);
        for (auto task : tasks[userID]) {
            cout << task->getTaskID() << " " << task->getTitle() << endl;
        }
    }

    void showCompletedTasks() {
        lock_guard<mutex> lock(mtx);
        for (auto task : completedTasks) {
            cout << task->getTaskID() << " " << task->getTitle() << endl;
        }
    }
};

int main(){
    TaskManager tm;

    User U1(1 , "Golu");
    User U2(2 , "PAPA");

    Task* t1 = new Task(1, "Complete LLD", "Finish Task Management System", "30/06/2026");
    t1->setPriority(URGENT);

    Task* t2 = new Task(2, "Practice DSA", "Solve 5 LeetCode problems", "01/07/2026");
    t2->setPriority(NORMAL);

    Task* t3 = new Task(3, "Prepare Resume", "Update resume for placements", "02/07/2026");
    t3->setPriority(URGENT);

    Task* t4 = new Task(4, "Read OS Notes", "Revise process synchronization", "03/07/2026");
    t4->setPriority(NORMAL);

    tm.assignTask(1 , t2);
    tm.assignTask(1 , t1);

    tm.showTasks(1);

    tm.markCompleted(1 , t1->getTaskID());
    tm.removeTask(1 , t1->getTaskID());

    tm.showTasks(1);

    tm.showCompletedTasks();

}