#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#ifdef _WIN32
  #include <io.h>
  #include <sys/stat.h>
  #define STAT_STRUCT    struct _stat
  #define STAT_FUNC(p, buf)  _stat((p), (buf))
  #define DIRSEP        "\\"
  #define FIND_HANDLE   intptr_t
  #define FIND_DATA     struct _finddata_t
  #define FIND_FIRST(p, d)   _findfirst((p), (d))
  #define FIND_NEXT(h, d)    _findnext((h), (d))
  #define FIND_CLOSE(h)      _findclose(h)
#else
  #include <dirent.h>
  #include <sys/stat.h>
  #define STAT_STRUCT    struct stat
  #define STAT_FUNC(p, buf)  stat((p), (buf))
  #define DIRSEP        "/"
  #define FIND_HANDLE   DIR*
  #define FIND_DATA     struct dirent
#endif

#define MAX_PATH    1024
#define MAX_PATTERN 256
#define MAX_COLOR_RULES 100
#define MAX_IGNORE_RULES 100

typedef struct {
    char pattern[MAX_PATTERN];
    int  count;
} FileGroup;

// color rule: extension or "folder"
typedef struct {
    char key[MAX_PATTERN];    // ".c" or "folder"
    char color_code[8];       // ANSI code like "33" or "34"
} ColorRule;

static ColorRule color_rules[MAX_COLOR_RULES];
static int       color_rule_count = 0;
static char      ignore_patterns[MAX_IGNORE_RULES][MAX_PATTERN];
static int       ignore_count = 0;

static int max_depth = -1;  // no limit by default
static int use_color = 0;

// Load config: ~/.dmitreerc or dmitree.conf in CWD
void load_config() {
    const char *paths[] = {"./dmitree.conf", "~/.dmitreerc"};
    char buf[512];
    for (int p = 0; p < 2; p++) {
        FILE *f = fopen(paths[p], "r");
        if (!f) continue;
        while (fgets(buf, sizeof(buf), f)) {
            if (buf[0] == '#' || buf[0] == '\n') continue;
            char *eq = strchr(buf, '=');
            if (!eq) continue;
            *eq = '\0';
            char *key = buf;
            char *val = eq + 1;
            // trim newline
            char *nl = strchr(val, '\n'); if (nl) *nl = '\0';
            if (strcmp(key, "color") == 0) {
                // format: ext:key:code or folder:code
                char *k = strtok(val, ":");
                char *c = strtok(NULL, ":");
                if (k && c && color_rule_count < MAX_COLOR_RULES) {
                    strcpy(color_rules[color_rule_count].key, k);
                    strcpy(color_rules[color_rule_count].color_code, c);
                    color_rule_count++;
                }
            } else if (strcmp(key, "ignore") == 0) {
                if (ignore_count < MAX_IGNORE_RULES) {
                    strncpy(ignore_patterns[ignore_count++], val, MAX_PATTERN-1);
                }
            }
        }
        fclose(f);
    }
}

// Check if name matches any ignore pattern (simple substring)
int is_ignored(const char *name) {
    for (int i = 0; i < ignore_count; i++) {
        if (strstr(name, ignore_patterns[i])) return 1;
    }
    return 0;
}

// Get ANSI wrap for key (extension or "folder")
char* get_color(const char *key) {
    for (int i = 0; i < color_rule_count; i++) {
        if (strcmp(color_rules[i].key, key) == 0) {
            static char seq[16];
            snprintf(seq, sizeof(seq), "\x1b[%sm", color_rules[i].color_code);
            return seq;
        }
    }
    return "";
}

// Reset ANSI
const char *COLOR_RESET = "\x1b[0m";

void extract_pattern(const char* filename, char* pattern) {
    int i, j = 0;
    int in_number = 0;
    for (i = 0; filename[i] && j < MAX_PATTERN - 1; i++) {
        if (isdigit((unsigned char)filename[i])) {
            if (!in_number) { pattern[j++] = '#'; in_number = 1; }
        } else { pattern[j++] = filename[i]; in_number = 0; }
    }
    pattern[j] = '\0';
}

int find_or_create_group(FileGroup* groups, int* group_count, const char* pattern) {
    for (int i = 0; i < *group_count; i++) {
        if (strcmp(groups[i].pattern, pattern) == 0) {
            groups[i].count++;
            return i;
        }
    }
    strcpy(groups[*group_count].pattern, pattern);
    groups[*group_count].count = 1;
    (*group_count)++;
    return *group_count - 1;
}

int has_numbers(const char* str) {
    for (int i = 0; str[i]; i++) if (isdigit((unsigned char)str[i])) return 1;
    return 0;
}

void print_tree(const char* path, int level, int show_files) {
    if (max_depth >= 0 && level > max_depth) return;

    FIND_HANDLE hFind;
    FIND_DATA fd;
    STAT_STRUCT st;
    char full_path[MAX_PATH];
    char indent[256] = "";
    FileGroup groups[1000];
    int group_count = 0;

    for (int i = 0; i < level; i++) strcat(indent, "  ");
    if (level > 0) {
        const char* leaf = strrchr(path, DIRSEP[0]);
        char *color = "";
        if (use_color) color = get_color("folder");
        printf("%s%s+-- %s/%s\n", indent, color, leaf ? leaf+1 : path,
               use_color ? COLOR_RESET : "");
        strcat(indent, "  ");
    }

    char search[MAX_PATH];
    snprintf(search, MAX_PATH, "%s%s*", path, DIRSEP);
    hFind = FIND_FIRST(search, &fd);
    if (hFind == -1) return;

    do {
        if (fd.name[0] == '.' || is_ignored(fd.name)) continue;
        snprintf(full_path, MAX_PATH, "%s%s%s", path, DIRSEP, fd.name);
        if (STAT_FUNC(full_path, &st) == 0) {
            int is_dir = (st.st_mode & S_IFDIR) != 0;
            if (is_dir) {
                print_tree(full_path, level+1, show_files);
            } else if (show_files) {
                char ext[MAX_PATTERN] = "";
                char *color = "";
                // get extension
                char *dot = strrchr(fd.name, '.');
                if (dot) { strncpy(ext, dot, MAX_PATTERN-1); }
                if (use_color && ext[0]) color = get_color(ext);
                // print
                printf("%s%s|-- %s%s\n", indent, color, fd.name,
                       use_color ? COLOR_RESET : "");
            }
        }
    } while (FIND_NEXT(hFind, &fd) == 0);
    FIND_CLOSE(hFind);

    if (show_files) {
        for (int i = 0; i < group_count; i++) {
            if (groups[i].count > 1) {
                printf("%s|-- %s (%d files)\n", indent,
                       groups[i].pattern, groups[i].count);
            }
        }
    }
}

int main(int argc, char* argv[]) {
    const char* start_path = ".";
    int show_files = 1;

    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "-d") == 0) {
            show_files = 0;
        } else if (strcmp(argv[i], "--color") == 0) {
            use_color = 1;
        } else if ((strcmp(argv[i], "-L") == 0 || strcmp(argv[i], "--depth") == 0) && i+1<argc) {
            max_depth = atoi(argv[++i]);
        } else if (strcmp(argv[i], "-h") == 0 || strcmp(argv[i], "--help") == 0) {
            printf("Usage: %s [options] [directory]\n", argv[0]);
            printf("Options:\n");
            printf("  -d            Show directories only\n");
            printf("  -L, --depth N Limit depth to N levels\n");
            printf("  --color       Enable ANSI colors per config\n");
            printf("  -h, --help    Show this help\n");
            return 0;
        } else {
            start_path = argv[i];
        }
    }

    load_config();
    printf("Tree structure for: %s\n", start_path);
    print_tree(start_path, 0, show_files);
    return 0;
}
