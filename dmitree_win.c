#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#ifdef _WIN32
    #include <windows.h>
    #include <direct.h>
    #define PATH_SEPARATOR '\\'
    #define getcwd _getcwd
#else
    #include <dirent.h>
    #include <sys/stat.h>
    #include <unistd.h>
    #define PATH_SEPARATOR '/'
#endif

#define MAX_PATH_LEN 1024
#define MAX_PATTERN 256

typedef struct {
    char pattern[MAX_PATTERN];
    int count;
} FileGroup;

// Function to extract base pattern from filename
void extract_pattern(const char* filename, char* pattern) {
    int i, j = 0;
    int in_number = 0;

    for (i = 0; filename[i] && j < MAX_PATTERN - 1; i++) {
        if (isdigit(filename[i])) {
            if (!in_number) {
                pattern[j++] = '#';
                in_number = 1;
            }
        } else {
            pattern[j++] = filename[i];
            in_number = 0;
        }
    }
    pattern[j] = '\0';
}

// Function to find or create a file group
int find_or_create_group(FileGroup* groups, int* group_count, const char* pattern) {
    for (int i = 0; i < *group_count; i++) {
        if (strcmp(groups[i].pattern, pattern) == 0) {
            groups[i].count++;
            return i;
        }
    }

    // Create new group
    strcpy(groups[*group_count].pattern, pattern);
    groups[*group_count].count = 1;
    (*group_count)++;
    return *group_count - 1;
}

// Function to check if a string contains digits
int has_numbers(const char* str) {
    for (int i = 0; str[i]; i++) {
        if (isdigit(str[i])) return 1;
    }
    return 0;
}

#ifdef _WIN32
void print_tree_windows(const char* path, int level, int show_files) {
    WIN32_FIND_DATA findFileData;
    HANDLE hFind;
    char searchPath[MAX_PATH_LEN];
    char fullPath[MAX_PATH_LEN];
    char indent[256] = "";
    FileGroup groups[1000];
    int group_count = 0;

    // Create indentation
    for (int i = 0; i < level; i++) {
        strcat(indent, "  ");
    }

    if (level > 0) {
        const char* dirName = strrchr(path, PATH_SEPARATOR);
        printf("%s+--- %s/\n", indent, dirName ? dirName + 1 : path);
        strcat(indent, "  ");
    }

    // Prepare search path
    snprintf(searchPath, sizeof(searchPath), "%s\\*", path);

    hFind = FindFirstFile(searchPath, &findFileData);
    if (hFind == INVALID_HANDLE_VALUE) {
        printf("Error accessing directory: %s\n", path);
        return;
    }

    do {
        if (strcmp(findFileData.cFileName, ".") == 0 || 
            strcmp(findFileData.cFileName, "..") == 0) {
            continue;
        }

        snprintf(fullPath, sizeof(fullPath), "%s\\%s", path, findFileData.cFileName);

        if (findFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
            print_tree_windows(fullPath, level + 1, show_files);
        } else if (show_files) {
            if (has_numbers(findFileData.cFileName)) {
                char pattern[MAX_PATTERN];
                extract_pattern(findFileData.cFileName, pattern);
                find_or_create_group(groups, &group_count, pattern);
            } else {
                printf("%s|-- %s\n", indent, findFileData.cFileName);
            }
        }
    } while (FindNextFile(hFind, &findFileData) != 0);

    FindClose(hFind);

    // Print grouped files
    if (show_files) {
        for (int i = 0; i < group_count; i++) {
            if (groups[i].count > 1) {
                printf("%s├── %s (%d files)\n", indent, groups[i].pattern, groups[i].count);
            } else {
                // Find and print the original filename
                hFind = FindFirstFile(searchPath, &findFileData);
                if (hFind != INVALID_HANDLE_VALUE) {
                    do {
                        if (strcmp(findFileData.cFileName, ".") == 0 || 
                            strcmp(findFileData.cFileName, "..") == 0) {
                            continue;
                        }
                        if (!(findFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)) {
                            char pattern[MAX_PATTERN];
                            extract_pattern(findFileData.cFileName, pattern);
                            if (strcmp(pattern, groups[i].pattern) == 0) {
                                printf("%s├── %s\n", indent, findFileData.cFileName);
                                break;
                            }
                        }
                    } while (FindNextFile(hFind, &findFileData) != 0);
                    FindClose(hFind);
                }
            }
        }
    }
}
#else
void print_tree_unix(const char* path, int level, int show_files) {
    DIR* dir;
    struct dirent* entry;
    struct stat file_stat;
    char full_path[MAX_PATH_LEN];
    char indent[256] = "";
    FileGroup groups[1000];
    int group_count = 0;

    // Create indentation
    for (int i = 0; i < level; i++) {
        strcat(indent, "  ");
    }

    if (level > 0) {
        printf("%s+--- %s/\n", indent, strrchr(path, '/') ? strrchr(path, '/') + 1 : path);
        strcat(indent, "  ");
    }

    dir = opendir(path);
    if (!dir) {
        perror("opendir");
        return;
    }

    // First pass: collect directories and group files
    while ((entry = readdir(dir)) != NULL) {
        if (entry->d_name[0] == '.') continue;

        snprintf(full_path, sizeof(full_path), "%s/%s", path, entry->d_name);

        if (stat(full_path, &file_stat) == 0) {
            if (S_ISDIR(file_stat.st_mode)) {
                print_tree_unix(full_path, level + 1, show_files);
            } else if (show_files) {
                if (has_numbers(entry->d_name)) {
                    char pattern[MAX_PATTERN];
                    extract_pattern(entry->d_name, pattern);
                    find_or_create_group(groups, &group_count, pattern);
                } else {
                    printf("%s├── %s\n", indent, entry->d_name);
                }
            }
        }
    }

    // Print grouped files
    if (show_files) {
        for (int i = 0; i < group_count; i++) {
            if (groups[i].count > 1) {
                printf("%s├── %s (%d files)\n", indent, groups[i].pattern, groups[i].count);
            } else {
                // If only one file matches pattern, show original name
                rewinddir(dir);
                while ((entry = readdir(dir)) != NULL) {
                    if (entry->d_name[0] == '.') continue;
                    char pattern[MAX_PATTERN];
                    extract_pattern(entry->d_name, pattern);
                    if (strcmp(pattern, groups[i].pattern) == 0) {
                        printf("%s├── %s\n", indent, entry->d_name);
                        break;
                    }
                }
            }
        }
    }

    closedir(dir);
}
#endif

void print_tree(const char* path, int level, int show_files) {
#ifdef _WIN32
    print_tree_windows(path, level, show_files);
#else
    print_tree_unix(path, level, show_files);
#endif
}

int main(int argc, char* argv[]) {
    const char* start_path = ".";
    int show_files = 1;

    // Parse command line arguments
    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "-d") == 0) {
            show_files = 0;  // directories only
        } else if (strcmp(argv[i], "-h") == 0 || strcmp(argv[i], "--help") == 0) {
            printf("Dmitree v0.2.0 - Smart directory tree viewer from VBT\n");
            printf("Usage: %s [options] [directory]\n", argv[0]);
            printf("Options:\n");
            printf("  -d        Show directories only\n");
            printf("  -h        Show this help\n");
            printf("\nGroups numbered files like: file_1.txt, file_2.txt -> file_#.txt (2 files)\n");
            printf("Cross-platform compatible (Windows/Linux)\n");
            return 0;
        } else {
            start_path = argv[i];
        }
    }

    printf("Tree structure for: %s\n", start_path);
    print_tree(start_path, 0, show_files);

    return 0;
}