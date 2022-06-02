#include<bits/stdc++.h>
using namespace std;
 
int main() {
    string str;
    while(cin >> str) {
        while(str.size() > 8) {
            cout << str.substr(0, 8) << endl;
            str = str.substr(8);
        }
        str.resize(8, '0');
        cout << str << endl;
    }
    return 0;
}