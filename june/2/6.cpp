#include <iostream>
#include <cstring>
#include <string>

using namespace std;
char str[1024] = {0};   

int main(int argc,char **argv)
{
    char ch;
    int count = 0;
    
    cin.getline(str,1024);
    cin >> ch;
    
    for(int i = 0;str[i] != '\0';++i)
    {
        if(str[i] == ch || str[i] == ch + 32 || str[i] == ch - 32)
        {
            ++count;
        }
    }
    
    cout << count << endl;
    
    return 0;
}