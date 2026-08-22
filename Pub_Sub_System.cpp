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
#include<list>

using namespace std;

/*

Design a Pub-Sub System


Requirements

The Pub-Sub system should allow publishers to publish messages to specific topics.
Subscribers should be able to subscribe to topics of interest and receive messages published to those topics.
The system should support multiple publishers and subscribers.
Messages should be delivered to all subscribers of a topic in real-time.
The system should handle concurrent access and ensure thread safety.
The Pub-Sub system should be scalable and efficient in terms of message delivery.

*/

/*

Functional Requirements : 
- Publishers publishing messages
- specific topics
- Subscribers subscribing
- Receive messages published to those topics
- Real time message delivery

Non Functional Requirements : 
- Thread Safety
- scalable
- Efficient
- Extendible


Workflow : 

1. Publish Messages on particular topics
2. Subscribers subscribe to particular topic
3. Subscribers gets real time message published to those topics

*/

class Topic{
private:
    string name;
public:
    Topic(string n){
        name = n;
    }
    string getTopicName(){
        return name;
    }
};

class Publisher {
private:
    int publisherID;
    string name;

public:
    Publisher(int id, string n) {
        publisherID = id;
        name = n;
    }

    int getPublisherID() const {
        return publisherID;
    }

    string getName() const {
        return name;
    }
};

class Subscriber {
private:
    int subscriberID;
    string name;

public:
    Subscriber(int id, string n) {
        subscriberID = id;
        name = n;
    }

    int getSubscriberID() const {
        return subscriberID;
    }

    string getName() const {
        return name;
    }

    void receiveMessage(string topic, string message) {
        cout << "[" << topic << "] "<< message << endl;
    }
};

class PubSubSystem {
private:
    unordered_map<string, vector<Subscriber*>> topicSubscribers;
    unordered_map<string, vector<string>> topicMessages;
    mutex mtx;

public:

    // Subscribe to a topic
    void subscribe(Subscriber* subscriber, Topic* topic) {
        lock_guard<mutex> lock(mtx);
        topicSubscribers[topic->getTopicName()].push_back(subscriber);
        cout << subscriber->getName() << " subscribed to " << topic->getTopicName() << endl;
    }

    // Publish a message
    void publish(Publisher* publisher, Topic* topic, string message) {
        lock_guard<mutex> lock(mtx);
        string topicName = topic->getTopicName();
        topicMessages[topicName].push_back(message);
        cout << publisher->getName() << " published to " << topicName << endl;
        // Real-time delivery
        for (auto subscriber : topicSubscribers[topicName]) {
            subscriber->receiveMessage(topicName, message);
        }
    }

    // Optional: Show message history
    void showMessages(Topic* topic) {
        string topicName = topic->getTopicName();
        cout << "Messages of " << topicName << ":\n";
        for (auto &msg : topicMessages[topicName]) {
            cout << msg << endl;
        }
    }
};

int main() {

    // Create Topics
    Topic sports("Sports");
    Topic technology("Technology");

    // Create Publishers
    Publisher p1(1, "ESPN");
    Publisher p2(2, "TechCrunch");

    // Create Subscribers
    Subscriber s1(101, "Prabal");
    Subscriber s2(102, "Rahul");
    Subscriber s3(103, "Aman");

    // Create Pub-Sub System
    PubSubSystem pubsub;

    // Subscribers subscribe to topics
    pubsub.subscribe(&s1, &sports);
    pubsub.subscribe(&s2, &sports);
    pubsub.subscribe(&s2, &technology);
    pubsub.subscribe(&s3, &technology);

    cout << endl;

    // Publishers publish messages
    pubsub.publish(&p1, &sports, "India won the cricket match!");

    cout << endl;

    pubsub.publish(&p2, &technology, "New AI model released today.");

    cout << endl;

    pubsub.publish(&p1, &sports, "Football World Cup starts next week.");

    cout << endl;

    // Show stored message history
    pubsub.showMessages(&sports);

    cout << endl;

    pubsub.showMessages(&technology);

    return 0;
}
