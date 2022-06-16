#include <iostream>
#include "thread.h"
#include "tcpserver.h"

using namespace std;

#if 0
int main(int argc, char **argv)
{
   cout << "Hello World!" << endl;
   Thread *pt1 = new Thread(8);
   Thread *pt2 = new Thread(8);
   Thread *pt3 = new Thread(8);
   Thread *pt4 = new Thread(8);

   pt1->start();
   pt2->start();
   pt3->start();
   pt4->start();

   while (1)
   {
      printf("11111111111\n");
      sleep(1);
   }
   return 0;
}
#endif

#if 1
int main(int argc, char const *argv[])
{
   TcpServer s(8);
   s.listen(9999,"192.168.221.128");
   s.start();
   return 0;
}
#endif

