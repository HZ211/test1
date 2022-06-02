#include <iostream>
#include <algorithm>
#include <vector>
#include <ctime>

using namespace std;

int main(int argc, char **argv)
{
    vector<int> v(1000);
    srand(time(NULL));

    for (int i = 0; i < 100; ++i)
    {
        v[i] = rand() % 500 + 1;
    }
    sort(v.begin(), v.end());
    
    v.erase(unique(v.begin(), v.end()), v.end());

    for (vector<int>::iterator it = v.begin(); it != v.end(); ++it)
    {
        cout << *it << endl;
    }
}