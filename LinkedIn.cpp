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
#include<set>

using namespace std;

/*
Design a Professional Networking Platform like LinkedIn


Requirements :- 


User Registration and Authentication:

Users should be able to create an account with their professional information, such as name, email, and password.
Users should be able to log in and log out of their accounts securely.


User Profiles:

Each user should have a profile with their professional information, such as profile picture, headline, summary, experience, education, and skills.
Users should be able to update their profile information.


Connections:

Users should be able to send connection requests to other users.
Users should be able to accept or decline connection requests.
Users should be able to view their list of connections.


Messaging:

Users should be able to send messages to their connections.
Users should be able to view their inbox and sent messages.


Job Postings:

Employers should be able to post job listings with details such as title, description, requirements, and location.
Users should be able to view and apply for job postings.


Search Functionality:

Users should be able to search for other users, companies, and job postings based on relevant criteria.
Search results should be ranked based on relevance and user preferences.


Notifications:

Users should receive notifications for events such as connection requests, messages, and job postings.
Notifications should be delivered in real-time.


Scalability and Performance:

The system should be designed to handle a large number of concurrent users and high traffic load.
The system should be scalable and efficient in terms of resource utilization.

*/

/*

Functional Requirements

User Registration.
Login / Logout.
View and Update Profile.
Send Connection Request.
Accept / Decline Connection Request.
View Connections.
Send Messages.
Receive Messages.
Post Jobs.
View Jobs.
Apply for Jobs.
Search Users, Companies and Jobs.
Receive Real-Time Notifications.


Non-Functional Requirements

Handle Concurrent Users.
Maintain Data Consistency.
Scalable.
Efficient Resource Utilization.
Extensible.

*/

/*

Workflow : 

1. User

2. Create Account / Login

3. Complete Profile / Update Profile (Optional)

4. Send Connection Request
   → Receive Connection Request
   → Accept / Decline Request
   → View Connections

5. Send Messages
   → Receive Messages
   → View Inbox / Sent Messages

6. Post Jobs (Only if registered as Employer/Company)

7. View Job Postings

8. Apply for Job (Optional)

9. Search Users, Companies and Job Postings

10. Receive Real-Time Notifications

*/


class User{
private:
    string userID;
    string password;
    string name;
    string profilePicture;
    string headline;
    string summary;
    unordered_map<string,pair<string,string>> Education; // Educational Institution -> S Date to E Date
    unordered_map<string,pair<string,string>> Experience; // Educational Institution -> S Date to E Date
    vector<string> skills; //Multiple Skills
public:
    // Manadatory for Registration
    User(string un , string pass , string n){
        userID = un;
        password = pass;
        name = n;
        //Default
        profilePicture = "";
        headline = "";
        summary = "";
        Education = {};
        Experience = {};
        skills = {};
    };  
    // setters
    void setPFP(string p){
        profilePicture = p;
    }
    void setHeadline(string h){
        headline = h;
    }
    void setSummary(string s){
        summary = s;
    }
    void addEducation(string inst, pair<string,string> dates){
        Education[inst] = dates;
    }
    void deleteEducation(string inst){
        auto it = Education.find(inst);
        if(it != Education.end()){
            Education.erase(inst);
        }else{
            cout<<"Record Not found"<<endl;
        }
    }
    void addExperience(string comp, pair<string,string> dates){
        Experience[comp] = dates;
    }
    void deleteExperience(string comp){
        auto it = Experience.find(comp);
        if(it != Experience.end()){
            Experience.erase(comp);
        }else{
            cout<<"Company Not found"<<endl;
        }
    }
    void addSkills(string s){
        skills.push_back(s);
    }
    //getters
    string getUserID() const{
        return userID;
    }
    string getUserName() const{
        return name;
    }
};


class Connections {
private:
    unordered_map<string, vector<User*>> connections;
    unordered_map<string, queue<User*>> pending;
    mutex mtx;
public:

    void addUser(User* user) {
        connections[user->getUserID()] = {};
        pending[user->getUserID()] = {};
    }

    // Send Connection Request
    void sendRequest(User* sender, User* receiver) {
        lock_guard<mutex> lock(mtx);
        pending[receiver->getUserID()].push(sender);
        cout << "Connection Request Sent Successfully." << endl;
    }

    // View and Accept/Decline Requests
    void viewRequests(User* user) {
        lock_guard<mutex> lock(mtx);
        auto &q = pending[user->getUserID()];
        while (!q.empty()) {
            User* sender = q.front();
            q.pop();
            cout << sender->getUserName()<< " sent you a request." << endl;

            cout << "ACCEPT / DECLINE : ";
            string choice;
            cin >> choice;
            if (choice == "ACCEPT") {
                connections[user->getUserID()].push_back(sender);
                connections[sender->getUserID()].push_back(user);
                cout << "Connected with " << sender->getUserName() << endl;
            }
            else {
                cout << "Request Declined." << endl;
            }
        }
    }

    // View Connections
    void viewConnections(User* user) {
        lock_guard<mutex> lock(mtx);
        cout << "Connections :" << endl;
        for (auto connection : connections[user->getUserID()]) {
            cout << connection->getUserName() << endl;
        }
    }
    
    bool checkConnection(User *u , User *con){
        lock_guard<mutex> lock(mtx);
        for(auto c : connections[u->getUserID()]){
            if(c->getUserID() == con->getUserID()){
                return true;
            }
        }
        return false;
    }
};

enum MessageStatus{
    SEEN,
    NEW
};

class Messaging {
private:
    Connections* connections;
    unordered_map<string, queue<pair<User*, string>>> sentMessages;
    unordered_map<string, queue<pair<User*, pair<string, MessageStatus>>>> inbox;
    mutex mtx;
public:

    Messaging(Connections* con){
        connections = con;
    }

    void sendMessage(User* sender, User* receiver, string message){
        lock_guard<mutex> lock(mtx);
        if(!connections->checkConnection(sender, receiver)){
            cout << "Can't send message. Not a connection." << endl;
            return;
        }
        sentMessages[sender->getUserID()].push({receiver, message});
        inbox[receiver->getUserID()].push({sender, {message, NEW}});
        cout << "Message sent successfully." << endl;
    }

    void viewNewMessages(User* user){
        lock_guard<mutex> lock(mtx);
        auto &q = inbox[user->getUserID()];
        int sz = q.size();
        if(sz == 0){
            cout << "No Messages." << endl;
            return;
        }

        while(sz--){
            auto [sender, msg] = q.front();
            q.pop();
            string senderName = sender->getUserName();
            string message = msg.first;
            MessageStatus status = msg.second;
            if(status == NEW){
                cout << "Sender : " << senderName << endl;
                cout << "Message : " << message << endl;
                q.push({sender, {message, SEEN}});
            }
            else{
                q.push({sender, {message, SEEN}});
            }
        }
    }

    void viewAllMessages(User* user){
        lock_guard<mutex> lock(mtx);
        auto q = inbox[user->getUserID()];
        if(q.empty()){
            cout << "No Messages." << endl;
            return;
        }

        while(!q.empty()){
            auto [sender, msg] = q.front();
            q.pop();
            cout << "Sender : " << sender->getUserName() << endl;
            cout << "Message : " << msg.first << endl;
        }
    }
};

class JobPosting {
private:
    int jobID;
    string CompanyName;
    string JobTitle;
    string JobDiscription;
public:
    JobPosting(int jid , string cn , string jt, string jd){
        jobID = jid;
        CompanyName = cn;
        JobTitle = jt;
        JobDiscription = jd;
    }
    int getjobID() const{
        return jobID;
    }
    string getCompanyName() const {
        return CompanyName;
    }
    string getJobTitle() const {
        return JobTitle;
    }
    string getJobDiscription() const {
        return JobDiscription;
    }
};

class JobApplication {
private:
    vector<JobPosting*> Jobs;
    unordered_map<int , vector<User*>> application; // JobID -> User applied (Company)
    unordered_map<string , vector<JobPosting*>> applied; // UserID -> Applied Job (User)
    mutex mtx;
public:
    //Add Job
    void addJob(JobPosting* j){
        lock_guard<mutex> lock(mtx);
        Jobs.push_back(j);
        cout<<"New Job Posting : "<<endl;
        cout<<j->getjobID()<<j->getCompanyName() << j->getJobTitle() << j->getJobDiscription()<<endl;
        application[j->getjobID()] = {};
    }
    //Search Job
    void serchJob(int byID , string byCName , string byTitle){
        lock_guard<mutex> lock(mtx);
        // by ID
        set<JobPosting*> results;
        cout<<"Jobs based on Job ID : "<<endl;
        for(auto j : Jobs){
            if(j->getjobID() == byID){
                results.insert(j);
            }
        }
        // by Company Name
        cout<<"Jobs based on Company Name : "<<endl;
        for(auto j : Jobs){
            if(j->getCompanyName() == byCName){
                results.insert(j);
            }
        }
        //by Job Title
        cout<<"Jobs based on Job Title : "<<endl;
        for(auto j : Jobs){
            if(j->getJobTitle() == byTitle){
                results.insert(j);
            }
        }
        for(auto j : results)
            cout<<j->getjobID()<<j->getCompanyName() << j->getJobTitle() << j->getJobDiscription()<<endl;
   
    }
    //Apply for job
    void apply(JobPosting* j , User* u){
        lock_guard<mutex> lock(mtx);
        auto jid = j->getjobID();
        auto uid = u->getUserID();
        application[jid].push_back(u);
        applied[uid].push_back(j);
        cout<<"Applied Sucessfully"<<endl;
    }
};

int main() {

    // Create Users
    User* u1 = new User("prabal", "1234", "Prabal");
    User* u2 = new User("rahul", "5678", "Rahul");
    User* u3 = new User("amit", "9999", "Amit");

    // ================= Connections =================
    Connections connections;

    connections.addUser(u1);
    connections.addUser(u2);
    connections.addUser(u3);

    // Send Request
    connections.sendRequest(u1, u2);

    // Rahul accepts/declines from console
    connections.viewRequests(u2);

    cout << "\nRahul's Connections:\n";
    connections.viewConnections(u2);

    cout << "\nPrabal's Connections:\n";
    connections.viewConnections(u1);

    // ================= Messaging =================
    Messaging messaging(&connections);

    messaging.sendMessage(u1, u2, "Hello Rahul!");
    messaging.sendMessage(u2, u1, "Hi Prabal!");

    cout << "\nNew Messages of Prabal\n";
    messaging.viewNewMessages(u1);

    cout << "\nAll Messages of Prabal\n";
    messaging.viewAllMessages(u1);

    // ================= Jobs =================
    JobApplication jobs;

    JobPosting* j1 = new JobPosting(
        1,
        "Google",
        "Software Engineer",
        "C++ Backend Developer"
    );

    JobPosting* j2 = new JobPosting(
        2,
        "Microsoft",
        "SDE Intern",
        "Strong DSA Required"
    );

    jobs.addJob(j1);
    jobs.addJob(j2);

    cout << "\nSearching Jobs...\n";
    jobs.serchJob(1, "", "");

    cout << "\nApplying for Job...\n";
    jobs.apply(j1, u1);

    return 0;
}