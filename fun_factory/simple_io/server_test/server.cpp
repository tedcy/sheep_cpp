#include <boost/smart_ptr.hpp>
#include "server.h"
#include "real_message_factory.h"
#include "message_factory.h"
using namespace boost;

int main() {
    MessageFactory *mfactoryP(new RealMessageFactory);
    scoped_ptr<Server> p(new Server(mfactoryP));
    if (p) {
        if (p->Init((char*)"127.0.0.1", 12345) < 0) {
            return -1;
        }
        return p->Serve();
    }
    return 0;
}
