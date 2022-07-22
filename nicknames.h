// Credits: https://worthstart.com/random-nicknames/

#include <vector>
#include <string>

#define NICKNAME_MAX_CHARACTERS 50

namespace circle_server {

    std::string get_random_nickname();
    bool validate_nickname(const std::string &_nickname);

}