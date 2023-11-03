#include <system_error>
#include <fcntl.h>
#include <assert.h>
#include <errno.h>
#include <iostream>
#include <string>

int main(void)
{
    std::string file_name = "bad_file_name.txt";
    int fd = open(file_name.data(), O_RDWR);
    if (fd < 0)
    {
        throw std::system_error(errno, std::generic_category(), file_name);
    }
}

// hoe kan ik ervoor zorgen dat de errno word geprint, en mijn message, en het nummer word meegegeven?

https: // stackoverflow.com/questions/12171377/how-to-convert-errno-to-exception-using-system-error