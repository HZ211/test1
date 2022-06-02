#include <iostream>
#include <string>

using namespace std;

int main(int argc,char **argv)
{
    char str[100] = {0};
    cin.getline(str,100);
    char temp[9] = {0};
    int i = 0,j = 0;
    
    while(str[i] != '\0')
    {
        temp[j++] = str[i];
        ++i;
        if(i % 8 == 0)
        {
            temp[j+1] = '\0';
            cout << temp << endl;
            j = 0;
        }
    }    
    for(;j < 8;++j)
    {
        temp[j] = '0';
    }
    temp[8] = '\0';
    cout << temp << endl;
    
    return 0;
}