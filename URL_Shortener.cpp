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

class URLshortner{
private:
    unordered_map<string,string> url_to_short;
    unordered_map<string,string> short_to_url;
    unordered_map<string,int> counter; // For short URL
    int cnt;
    string default_short_URL;
    mutex mtx;
public:
    URLshortner(){
        cnt = 1;
        default_short_URL = "https://tinyurl.com/";
    }
    void Shorten_URL(string URL){
        lock_guard<mutex> lock(mtx);
        if(url_to_short.count(URL)){
            cout<<url_to_short[URL]<<endl;
            return;
        }
        string short_URL = default_short_URL + to_string(cnt);
        cnt++;
        url_to_short[URL] = short_URL;
        short_to_url[short_URL] = URL;
        cout<<short_URL<<endl;
    }
    void delete_URL(string short_URL){
        lock_guard<mutex> lock(mtx);
        if(!short_to_url.count(short_URL)){
            cout<<"Invalid URL"<<endl;
            return;
        }
        string long_URL = short_to_url[short_URL];
        cout<<"URL : " << long_URL <<"  Deleted"<<endl;
        short_to_url.erase(short_URL);
        url_to_short.erase(long_URL);
        if(counter.count(short_URL)){
            counter.erase(short_URL);
        }
    }
    void visit(string short_URL){
        lock_guard<mutex> lock(mtx);
        if(!short_to_url.count(short_URL)){
            cout<<"Invalid URL"<<endl;
            return;
        }
        cout<<"Redirecting to : " <<short_to_url[short_URL]<<endl;
        counter[short_URL]++;
    }
    void check_count(string URL){
        lock_guard<mutex> lock(mtx);
        if(!short_to_url.count(URL)){
            cout<<"Invalid URL"<<endl;
            return;
        }
        if(!counter.count(URL)){
            cout<<"Not accessed yet."<<endl;
            return;
        }
        cout<<"Accessed times : "<<counter[URL]<<endl;
    }
};

int main(){
    URLshortner u;
    u.Shorten_URL("https://leetcode.com/problems/lru-cache");
    u.Shorten_URL("https://github.com/openai");
    u.Shorten_URL("https://www.chess.com/user433");

    cout<<endl;

    u.visit("https://tinyurl.com/2");
    u.visit("https://tinyurl.com/2");
    u.visit("https://tinyurl.com/2");
    u.visit("https://tinyurl.com/1");

    cout<<endl;

    u.check_count("https://tinyurl.com/2");
    u.check_count("https://tinyurl.com/1");

    cout<<endl;

    u.delete_URL("https://tinyurl.com/1");

    cout<<endl;

    u.check_count("https://tinyurl.com/1");

    return 0;
}