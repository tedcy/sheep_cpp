#include "client.h"
#include <boost/smart_ptr.hpp>
#include <iostream>
using namespace std;
using namespace boost;

int main() {
    scoped_ptr<Client> p(new Client);
    if (p) {
        cout << p->Echo() << endl;
    }
    return 0;
}
