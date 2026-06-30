#include <iostream>
#include <string>
#include <vector>
#include <map>

using namespace std;

/*
Design Stack Overflow

Requirements :

Users can post questions, answer questions, and comment on questions and answers.
Users can vote on questions and answers.
Questions should have tags associated with them.
Users can search for questions based on keywords, tags, or user profiles.
The system should assign reputation score to users based on their activity and the quality of their contributions.
The system should handle concurrent access and ensure data consistency.

*/

/*
Step 1: The Core Scope

Must-Have Features:

- Users can post a Question (must include 1 to 5 Tags).
- Users can post an Answer to a Question.
- Users can post a Comment on either a Question or an Answer.
- Users can Upvote or Downvote a Question or Answer.
- System calculates User Reputation (+10 for receiving an upvote, -2 for a downvote).

Limits :

- 1 Question -> Many Answers, Many Comments.
- 1 Answer -> Many Comments.
- A user cannot upvote or downvote their own post.
- A user cannot vote on the exact same post twice.

Ignoring:

- Search/Keyword parsing.
- Concurrency and thread safety.
- Feed generation and pagination.

*/

// 1. Enums for strict typing
enum VoteType {
    UPVOTE,
    DOWNVOTE
};

// 2. Core Entities
class User {
private:
    int id;
    string name;
    int reputationScore; 

public:
    // Constructor initializes the default state
    User(int uid, string uname) {
        id = uid;
        name = uname;
        reputationScore = 0;
    }

    // Getters RETURN data so other classes can use it
    int getId() const { return id; }
    string getName() const { return name; }
    int getReputation() const { return reputationScore; }

    // Flexible method to add (+10) or subtract (-2) points
    void updateReputation(int points) {
        reputationScore += points;
    }
};

class Tag {
    int id;
    string name;
};

// 3. Content Entities
class Comment {
    int id;
    int authorId;   // Who wrote it?
    string body;    // The actual text
};

class Answer {
private:
    int id;
    int authorId;
    string body;    // The actual text
    
    vector<Comment> comments; // An Answer HAS Comments
    
    // Voting State Management
    int voteCount; 
    map<int, VoteType> userVotes; // Maps UserID -> VoteType. Prevents voting twice!
public:
    // Constructor
    Answer(int ans_id , int auth_id, string abody){
        id = ans_id;
        authorId = auth_id;
        body = abody;
        voteCount = 0;
    }
    // getters
    int getID() const { return id; }
    int getAuthorId() const { return authorId; }
    int getVoteCount() const { return voteCount; }

    // --- The Core Behaviors (Verbs) ---

    // 1. Add Comments
    void addComments(Comment cmt){
        comments.push_back(cmt);
    }
    // 3. Process a Vote
    bool castVote(int userId, VoteType type) {
        // 1. The Author Check: You cannot vote on your own post
        if (userId == authorId) {
            return false; 
        }

        // 2. The Duplicate Check: You cannot vote twice
        if (userVotes.count(userId)) {
            return false;
        }

        // 3. State Update: +1 for Upvote, -1 for Downvote
        if (type == UPVOTE) {
            voteCount += 1;
        } else {
            voteCount -= 1;
        }

        // Save the record so they can't vote again
        userVotes[userId] = type;
        return true; 
    }

};

class Question {
private:
    int id;
    int authorId;
    string title;   
    string body;    
    
    vector<Tag> tags;         
    vector<Answer> answers;   
    vector<Comment> comments; 
    
    int voteCount; 
    map<int, VoteType> userVotes; 

public:
    // Constructor
    Question(int qid, int aid, string qTitle, string qBody) {
        id = qid;
        authorId = aid;
        title = qTitle;
        body = qBody;
        voteCount = 0;
    }
    
    // Getters
    int getId() const { return id; }
    int getAuthorId() const { return authorId; }
    int getVoteCount() const { return voteCount; }
    
    // --- The Core Behaviors (Verbs) ---
    
    // 1. Add an Answer
    void addAnswer(Answer answer) {
        answers.push_back(answer);
    }
    
    // 2. Add a Comment
    void addComment(Comment comment) {
        comments.push_back(comment);
    }
    
    // 3. Process a Vote
    // Returns true if successful, false if they already voted
    bool castVote(int userId, VoteType type) {
        // 1. The Author Check: You cannot vote on your own post
        if (userId == authorId) {
            return false; 
        }

        // 2. The Duplicate Check: You cannot vote twice
        if (userVotes.count(userId)) {
            return false;
        }

        // 3. State Update: +1 for Upvote, -1 for Downvote
        if (type == UPVOTE) {
            voteCount += 1;
        } else {
            voteCount -= 1;
        }

        // Save the record so they can't vote again
        userVotes[userId] = type;
        return true; 
    }
};

class StackOverflowSystem {
private:
    unordered_map<int, User> users;       // Maps UserID -> User Object
    unordered_map<int, Question> questions; // Maps QuestionID -> Question Object

public:
    // 1. Register a new user
    void registerUser(User u) {
        users.insert({u.getId(), u});
    }

    // 2. Post a new question
    void postQuestion(Question q) {
        questions.insert({q.getId(), q});
    }

    // 3. The Boss Fight: Processing the Vote
    void voteOnQuestion(int votingUserId, int questionId, VoteType type) {
        
        // Step 1: Check if the question exists in our database
        auto qIterator = questions.find(questionId);
        if (qIterator == questions.end()) {
            cout << "System Error: Question " << questionId << " not found." << endl;
            return;
        }

        // Step 2: Grab the actual question by REFERENCE (&) so we don't modify a copy
        Question& targetQuestion = qIterator->second;

        // Step 3: Attempt to cast the vote using the logic we wrote earlier
        bool voteSuccess = targetQuestion.castVote(votingUserId, type);

        // Step 4: If the vote went through, update the author's reputation
        if (voteSuccess) {
            int authorId = targetQuestion.getAuthorId();
            
            // Make sure the author actually exists in our system
            auto uIterator = users.find(authorId);
            if (uIterator != users.end()) {
                
                // Grab the author by REFERENCE (&)
                User& author = uIterator->second;
                
                // Apply the business rules (+10 for upvote, -2 for downvote)
                if (type == UPVOTE) {
                    author.updateReputation(10);
                    cout << "Success: Vote cast. Author reputation +10." << endl;
                } else {
                    author.updateReputation(-2);
                    cout << "Success: Vote cast. Author reputation -2." << endl;
                }
            }
        } else {
            // Vote failed (they are the author, or they already voted)
            cout << "Declined: User " << votingUserId << " cannot vote on this question." << endl;
        }
    }
};

int main() {
    cout << "--- System Initializing ---" << endl;
    StackOverflowSystem system;

    // 1. Create and Register Users
    User u1(1, "Prabal Pratik"); // The Author
    User u2(2, "Senior Engineer"); // The Voter
    
    system.registerUser(u1);
    system.registerUser(u2);

    // 2. Create and Register Content
    Question q1(101, 1, "CS", "How many MB are there in a GB?");
    Answer a1(201, 2, "There are 1024 MB in a GB.");
    
    q1.addAnswer(a1);
    system.postQuestion(q1); // Save it to the System database

    cout << "\n--- Testing Voting Logic ---" << endl;

    // TEST 1: The Author Check
    // Prabal (User 1) tries to upvote his own question (Question 101)
    cout << "\n[Test 1] Author tries to vote:" << endl;
    system.voteOnQuestion(1, 101, UPVOTE); 

    // TEST 2: The Successful Vote
    // User 2 upvotes Prabal's question. Prabal should get +10 reputation.
    cout << "\n[Test 2] Valid user casts an upvote:" << endl;
    system.voteOnQuestion(2, 101, UPVOTE); 

    // TEST 3: The Duplicate Check
    // User 2 tries to vote on the exact same question again.
    cout << "\n[Test 3] Valid user tries to vote AGAIN:" << endl;
    system.voteOnQuestion(2, 101, DOWNVOTE); 

    cout << "\n--- System Execution Complete ---" << endl;
    return 0;
} 