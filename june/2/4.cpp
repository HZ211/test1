#include <bits/stdc++.h>
using namespace std;

int main()
{
    string str = "hello";
    // while(cin >> str) {
    //     while(str.size() > 8) {
    //         cout << str.substr(0, 8) << endl;
    //         str = str.substr(8);
    //     }
    //     str.resize(8, '0');
    //     cout << str << endl;
    // }
    // return 0;
    // cout << str.substr(0,8) << endl;
    str.resize(8);
    cout << str << endl;

    char s[10];
    strcpy(s, "hex");
    string ptr = s;

    cout << ptr << endl;
}