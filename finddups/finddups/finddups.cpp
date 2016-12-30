#include <iostream>
#include <vector>

#include <unistd.h>
#include <libgen.h>
#include <stdlib.h>
#include <stdio.h>

using std::string;
using std::vector;

typedef struct {
    uint32_t checksum;
    vector<string> matches;
} ilist_entry;

typedef struct {
    uint32_t checksum;
    string file;
} list_entry;

extern vector<ilist_entry *> findMatches(list_entry **list, size_t size);
extern vector<string> listFiles(string directory);
extern "C" uint32_t doChecksum(const char *file);
extern "C" bool testEQ(const char *path1, const char *path2);

void relocate_file(std::string file)
{
    if (rename(file.c_str(), basename((char *)file.c_str())))
        fprintf(stderr, "Error moving file '%s'\n", file.c_str());
}

void dellist(void **list, size_t size)
{
    for (size_t i = 0; i < size; i++)
        delete ((uintptr_t **)list)[i];

    delete [] list;
}

int main(int argc, char **argv)
{
    if (argc < 2 || argc > 4)
    {
        std::clog << "Usage: " << argv[0] << " <directory> [-Fg] [-o path]" << std::endl;
        std::clog << "       -F: Compare direct byte streams as well as CRC-32 checksums" << std::endl;
        std::clog << "       -g: Move matching files to current directory" << std::endl;
        std::clog << "       -o: Create an output file at the specified path to list duplicates" << std::endl;

        exit(1);
    }

    if (access(argv[1], F_OK | R_OK))
    {
        std::clog << "Error Accessing directory '" << argv[1] << "'" << std::endl;
        exit(1);
    }

    std::string outputFile = "";
    bool compareFully = false;
    bool moveFiles = false;
    
    char option = 0;
    optind = 2;
    
    while ((option = getopt(argc, argv, "Fgo:")) != -1)
    {
        switch (option)
        {
            case 'F': compareFully = true; break;
            case 'g': moveFiles = true; break;
            case 'o': outputFile = string(optarg); break;
            case ':': std::clog << "Missing Argument!" << std::endl;
            case '?': std::clog << "Unknown Option!" << std::endl;
            default: break;
        }
    }

    std::cout << "Building file list..."; std::cout.flush();
    vector<string> filelist = listFiles(string(argv[1]));
    if (filelist.size() == 0) exit(1);
    size_t filecount = filelist.size();

    std::cout << " (" << filecount << ")" << std::endl;
    std::cout << "Calculating checksums... "; std::cout.flush();
    list_entry **entrylist = new list_entry *[filecount];
    char *ls; asprintf(&ls, "%lu", filecount);
    string lstr(ls);
    free(ls);

    for (size_t i = 0; i < filecount; i++)
    {
        char *istr; asprintf(&istr, "%lu", i);
        string pstring = "(" + string(istr) + "/" + lstr + ")";
        free(istr);

        char *bstring = new char[pstring.length() + 1];
        memset((void *)bstring, '\b', pstring.length() * sizeof(char));
        bstring[pstring.length()] = '\0';
        std::cout << pstring;
        std::cout.flush();

        list_entry *entry = new list_entry;
        entry->checksum = doChecksum(filelist[i].c_str());
        entry->file = filelist[i];
        entrylist[i] = entry;
        std::cout << bstring;
        delete [] bstring;
    }

    std::cout << "(" << lstr << "/" << lstr << ")" << std::endl;
    std::cout << "Looking for matching checksums..." << std::endl;

    vector<ilist_entry *> dupchklist = findMatches(entrylist, filecount);
    dellist((void **)entrylist, filecount);

    if (compareFully) {
        std::cout << "Found " << dupchklist.size() << " matching checksums." << std::endl;
        std::cout << "Comparing files with matching checksums fully..." << std::endl;
        vector<vector<string>> matches;

        for (size_t i = 0; i < dupchklist.size(); i++)
        {
            vector<string> possible_matches = dupchklist[i]->matches;
            bool actualMatch = true;

            for (size_t j = 1; j < possible_matches.size(); j++)
            {
                if (possible_matches[0] == possible_matches[j])
                {
                    std::cout << "Warning: Found matching file names: '" << possible_matches[0]
                              << "' and '" << possible_matches[j] << "'" << std::endl;

                    actualMatch = false;
                    break;
                }

                actualMatch = testEQ(possible_matches[0].c_str(), possible_matches[j].c_str());
                if (!actualMatch) break;
            }

            if (actualMatch) {
                matches.push_back(possible_matches);
            } else {
                std::cout << "Warning: Removed possible match [";

                for (size_t j = 0; j < possible_matches.size(); j++)
                {
                    std::cout << possible_matches[j];
                    if (j != (possible_matches.size() - 1)) std::cout << ", ";
                }

                std::cout << "]" << std::endl;
            }
        }

        std::cout << "Found " << matches.size() << " matches." << std::endl;
        std::cout << "Matches:" << std::endl;
        FILE *fp = NULL;

        if (outputFile != "")
        {
            fp = fopen(outputFile.c_str(), "w");
            if (!fp) std::cout << "Warning: could not open output file '" << outputFile << "'" << std::endl;
        }

        for (size_t i = 0; i < matches.size(); i++)
        {
            std::cout << (i + 1) << ' ';

            for (size_t j = 0; j < matches[i].size(); j++)
            {
                if (fp) fprintf(fp, "%s", matches[i][j].c_str());
                if (moveFiles) relocate_file(matches[i][j]);
                std::cout << matches[i][j];

                if (j != (matches[i].size() - 1))
                {
                    if (fp) fputc(' ', fp);
                    std::cout << ' ';
                }
            }

            if (fp) fputc('\n', fp);
            std::cout << std::endl;
        }

        if (fp) fclose(fp);
    } else {
        std::cout << "Found " << dupchklist.size() << " matching checksums." << std::endl;
        FILE *fp = NULL;
        
        if (outputFile != "")
        {
            fp = fopen(outputFile.c_str(), "w");
            if (!fp) std::cout << "Warning: could not open output file '" << outputFile << "'" << std::endl;
        }

        for (size_t i = 0; i < dupchklist.size(); i++)
        {
            std::cout << (i + 1) << ' ' << std::hex << dupchklist[i]->checksum << std::dec << ' ';

            for (size_t j = 0; j < dupchklist[i]->matches.size(); j++)
            {
                if (fp) fprintf(fp, "%s", dupchklist[i]->matches[j].c_str());
                if (moveFiles) relocate_file(dupchklist[i]->matches[j]);
                std::cout << dupchklist[i]->matches[j];

                if (j != (dupchklist[i]->matches.size() - 1))
                {
                    if (fp) fputc(' ', fp);
                    std::cout << ' ';
                }
            }

            if (fp) fputc('\n', fp);
            std::cout << std::endl;
        }

        if (fp) fclose(fp);
    }

    return(0);
}
