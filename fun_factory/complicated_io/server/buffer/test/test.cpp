#include "../buffer.h"
#include <iostream>
using namespace std;

int main() {
    BufferWrapper buffer(1);
    cout << buffer.Capacity() << endl;
    cout << buffer.Size() << endl;
    cout << buffer.LeftWrite() << endl;
    return 0;
}
