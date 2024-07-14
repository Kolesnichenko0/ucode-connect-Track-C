#ifndef ULS_UTILS_H
#define ULS_UTILS_H

#include "libmx.h"

#include <sys/ioctl.h>
#include <time.h>

#include <stdbool.h>
#include <errno.h>
#include <string.h>
#include <dirent.h>
#include <grp.h>
#include <pwd.h>
#include <sys/acl.h>
#include <sys/stat.h>
#include <sys/syslimits.h>
#include <sys/types.h>
#include <sys/xattr.h>
#include <unistd.h>

//Configs

typedef enum e_time_type {
    LAST_MODIFICATION,    // default
    LAST_METADATA_CHANGE, // -c
    LAST_ACCESS,          // -u
    LAST_CREATION,        // -U
} t_time_type;

typedef enum e_sort_type {
    BY_NAME,  // default
    BY_TIME,  // -t
    BY_SIZE,  // -S
    UNSORTED, // -f
} t_sort_type;

typedef enum e_format{
    ONE_COLUMN,   // -1
    MULTI_COLUMN, // -C
    STREAM,       // -m
    DETAILED,     // -l
} t_format;

typedef enum e_ignore_type{
    HIDDEN,        // default
    DOTS,          // -A
    NOT_IGNORED,   // -a
} t_ignore_type;

typedef struct s_configs {
    t_time_type time_type;
    t_sort_type sort_type;
    t_format format;
    t_ignore_type ignore_type;
    bool use_recursion;                     // -R
    bool use_colors;                        // -G
    bool classify;                          // -F
    bool add_only_slash_to_directories;     // -p
    bool sort_reverse;                      // -r
    bool sort_horizontally;                 // -x
    bool display_human_readable_size;       // -h
    bool display_numeric_IDs;               // -n
    bool display_non_printable_characters;  // -q
    bool acl;                               // -e
    bool full_time_info;                    // -T
    bool extended_attributes;               // -@
    bool hide_owner_information;            // -g
    bool hide_group_information;            // -o
    bool follow_symbolic_links;             // -H
} t_configs;

t_configs *parse_configs(int argc, char *argv[]);

//File details
typedef struct s_file_details {
    char *path;
    char *name;
    char *user;
    char *link;
    char *group;
    char **xattr_keys;
    acl_t acl;
    struct stat stat;
    struct timespec timespec;
} t_file_details;

t_file_details *get_file_details(const char *dir, const char *name, t_configs *configs);
void free_file_details(t_file_details *file_details);
void free_files_details(t_list *files_details);

//Dirs
bool sort_print_dirs(t_list *dirs, t_configs *configs, bool must_print_names);
bool sort_print_dir(t_file_details *file_details, t_configs *configs, bool must_print_name);
blkcnt_t calculate_count_blocks(t_list *files);
bool get_dir_entries(t_list **entries, const char *name, t_configs *configs);
bool is_ignored(const char *name, t_ignore_type ignore_type);

//Sorting
void sort_filenames(t_list *filenames, t_sort_type sort_type);
void sort_files_details(t_list *files_details, t_sort_type sort_type, bool isReversed);

//Output
typedef struct s_width {
    int links;
    int user;
    int group;
    int size;
} t_width;

int print_file_details(t_file_details *files_details, t_configs *configs);
void print_one_column(t_list *files_details, t_configs *configs);
void print_multi_column(t_list *files_details, t_configs *configs);
void print_detailed(t_list *files_details, t_configs *configs);
void print_stream(t_list *files_details, t_configs *configs);
void print_files_details(t_list *files_details, t_configs *configs);

#endif
