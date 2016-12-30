#include <sys/stat.h>
#include <dirent.h>
#include <iostream>
#include <vector>

int addDirectory(std::vector<std::string> *list, std::string root)
{
    DIR *dir = opendir(root.c_str());
    struct dirent *entry;

    while ((entry = readdir(dir)))
    {
        std::string entry_name = std::string(entry->d_name);
        if (entry_name == "." || entry_name == "..") continue;
        if (entry_name == ".DS_Store") continue;

        std::string path = root + "/" + entry_name;
        struct stat stats; lstat(path.c_str(), &stats);

        if (access(path.c_str(), F_OK | R_OK))
        {
            std::cerr << std::endl << "Error: Could not access file '" << path << "'" << std::endl;

            closedir(dir);
            return 1;
        }

        if (S_ISDIR(stats.st_mode)) {
            int rval = addDirectory(list, path);

            if (rval != 0)
            {
                closedir(dir);
                return 1;
            }
        } else if (S_ISREG(stats.st_mode)) {
            list->push_back(path);
        } else if (S_ISLNK(stats.st_mode)) {
            // Ignore links... they will always have the same checksums
        } else {
            std::cerr << std::endl << "Warning: Ignoring file '" << path << "' of invalid type";
            continue;
        }
    }

    closedir(dir);
    return 0;
}

std::vector<std::string> listFiles(std::string directory)
{
    std::vector<std::string> list;
    int rval = addDirectory(&list, directory);
    return (rval ? std::vector<std::string>() : list);
}
