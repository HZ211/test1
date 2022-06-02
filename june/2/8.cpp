#include <iostream>
#include <map>

using namespace std;

int main(int argc, char **argv)
{
    map<int, unsigned int> m;
    map<int, unsigned int>::iterator it;
    int n;
    cin >> n;
    for (int i = 0; i < n; ++i)
    {
        int a, b;
        cin >> a >> b;
        it = m.find(a);
        if (it != m.end())
        {
            m[a] += b;
        }
        else
        {
            m[a] = b;
        }
    }

    for (it = m.begin(); it != m.end(); ++it)
    {
        cout << it->first << " " << it->second << endl;
    }

    return 0;
}