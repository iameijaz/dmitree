#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <sys/stat.h>
#include <unistd.h>
#include <ctype.h>

#define MAX_PATH 1024
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

void print_tree(const char* path, int level, int show_files) {
    DIR* dir;
    struct dirent* entry;
    struct stat file_stat;
    char full_path[MAX_PATH];
    char indent[256] = "";
    FileGroup groups[1000];
    int group_count = 0;

    // Create indentation
    for (int i = 0; i < level; i++) {
        strcat(indent, "  ");
    }

    if (level > 0) {
        printf("%s└── %s/\n", indent, strrchr(path, '/') ? strrchr(path, '/') + 1 : path);
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
                print_tree(full_path, level + 1, show_files);
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

int main(int argc, char* argv[]) {
    const char* start_path = ".";
    int show_files = 1;

    // Parse command line arguments
    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "-d") == 0) {
            show_files = 0;  // directories only
        } else if (strcmp(argv[i], "-h") == 0 || strcmp(argv[i], "--help") == 0) {
            printf("Dmitree v1.0.0 - Smart directory tree viewer\n");
            printf("Usage: %s [options] [directory]\n", argv[0]);
            printf("Options:\n");
            printf("  -d        Show directories only\n");
            printf("  -h        Show this help\n");
            printf("\nGroups numbered files like: file_1.txt, file_2.txt -> file_#.txt (2 files)\n");
            printf("Repository: https://github.com/yourusername/dmitree\n");
            return 0;
        } else {
            start_path = argv[i];
        }
    }

    printf("Tree structure for: %s\n", start_path);
    print_tree(start_path, 0, show_files);

    return 0;
}