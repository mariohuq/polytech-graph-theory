#include <istream>
#include <ostream>

void encode(std::istream& is, std::ostream& os) {
    char buffer;
    char prev;
    if (!is.get(prev)) {
        return;
    }
    int count = 1;
    while (is.get(buffer)) {
        if (buffer == prev) {
            count++;
            continue;
        }

        prev = buffer;
    }
}

void decode(std::istream& is, std::ostream& os) {

}