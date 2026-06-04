#include "desktop_icons/desktop_icons.h"

#include <stdbool.h>
#include <stddef.h>

#include "ext2/ext2.h"
#include "heap/heap.h"
#include "libc_kernel/string.h"
#include "libc_kernel/stdlib.h"

static void str_trim_inplace(char* s) {
    size_t n = kstrlen(s);
    size_t a = 0;

    while (a < n && (s[a] == ' ' || s[a] == '\t' || s[a] == '\r' || s[a] == '\n')) {
        a++;
    }

    size_t b = n;

    while (b > a && (s[b - 1] == ' ' || s[b - 1] == '\t' || s[b - 1] == '\r' || s[b - 1] == '\n')) {
        b--;
    }

    if (a > 0) {
        for (size_t i = 0; i < b - a; i++) {
            s[i] = s[a + i];
        }
        s[b - a] = 0;
    } else {
        s[b] = 0;
    }
}

static void str_unquote_inplace(char* s) {
    size_t n = kstrlen(s);
    if (n >= 2 && s[0] == '"' && s[n - 1] == '"') {
        for (size_t i = 0; i < n - 2; i++) {
            s[i] = s[i + 1];
        }
        s[n - 2] = 0;
    }
}

static int parse_icon_section(const char* line, char* out_id, size_t cap) {
    if (kstrlen(line) < 6) {
        return 0;
    }

    if (!(line[0] == '[' && line[1] == 'I' && line[2] == 'c' && line[3] == 'o' && line[4] == 'n')) {
        return 0;
    }

    const char* q = kstrchr(line, '"');
    if (q == NULL) {
        return 0;
    }

    q++;

    const char* e = kstrchr(q, '"');
    if (e == NULL) {
        return 0;
    }

    size_t len = (size_t)(e - q);
    if (len >= cap) {
        len = cap - 1;
    }

    for (size_t i = 0; i < len; i++) {
        out_id[i] = q[i];
    }
    out_id[len] = 0;

    return 1;
}

static void next_line_bounds(const char* buf, size_t size, size_t* cursor, size_t* out_off, size_t* out_len) {
    size_t i = *cursor;

    while (i < size && (buf[i] == '\r' || buf[i] == '\n')) {
        i++;
    }

    size_t start = i;

    while (i < size && buf[i] != '\r' && buf[i] != '\n') {
        i++;
    }

    *out_off = start;
    *out_len = i - start;

    while (i < size && (buf[i] == '\r' || buf[i] == '\n')) {
        i++;
    }

    *cursor = i;
}

static int count_icon_sections(const uint8_t* data, size_t size) {
    size_t cur = 0;
    int sections = 0;
    char line[512];

    while (cur < size) {
        size_t off = 0;
        size_t len = 0;
        next_line_bounds((const char*)data, size, &cur, &off, &len);

        if (len == 0 || len >= sizeof(line)) {
            continue;
        }

        for (size_t i = 0; i < len; i++) {
            line[i] = ((const char*)data)[off + i];
        }
        line[len] = 0;

        str_trim_inplace(line);

        char tmp_id[64];
        if (line[0] == '[') {
            if (parse_icon_section(line, tmp_id, sizeof(tmp_id))) {
                sections++;
            }
        }
    }

    return sections;
}

desktop_icon_t* icons_ini_load(const char* path, int* count) {
    bool ok = true;
    size_t sz = 0;
    uint8_t* file = ext2_read_file(root_fs, path, &sz, &ok);

    if (!ok || file == NULL || sz == 0) {
        if (count != NULL) {
            *count = 0;
        }
        
        return NULL;
    }

    int max_icons = count_icon_sections(file, sz);
    if (max_icons <= 0) {
        if (count != NULL) {
            *count = 0;
        }

        return NULL;
    }

    desktop_icon_t* arr = (desktop_icon_t*)kmalloc(sizeof(desktop_icon_t) * (size_t)max_icons);
    int n = 0;

    size_t cur = 0;
    int in_icon = 0;
    desktop_icon_t cur_icon;
    kmemset(&cur_icon, 0, sizeof(cur_icon));

    char line[512];

    while (cur < sz) {
        size_t off = 0;
        size_t len = 0;
        next_line_bounds((const char*)file, sz, &cur, &off, &len);

        if (len == 0 || len >= sizeof(line)) {
            continue;
        }

        for (size_t i = 0; i < len; i++) {
            line[i] = ((const char*)file)[off + i];
        }
        line[len] = 0;

        str_trim_inplace(line);
        if (line[0] == 0) {
            continue;
        }

        if (line[0] == ';' || line[0] == '#') {
            continue;
        }

        if (line[0] == '[') {
            if (in_icon) {
                if (cur_icon.title[0] != 0 && cur_icon.exec_path[0] != 0) {
                    arr[n] = cur_icon;
                    n++;
                }
                kmemset(&cur_icon, 0, sizeof(cur_icon));
            }

            in_icon = parse_icon_section(line, cur_icon.id, sizeof(cur_icon.id));
            continue;
        }

        if (!in_icon) {
            continue;
        }

        char* eq = kstrchr(line, '=');
        if (eq == NULL) {
            continue;
        }

        *eq = 0;

        char* k = line;
        char* v = eq + 1;

        str_trim_inplace(k);
        str_trim_inplace(v);
        str_unquote_inplace(v);

        if (kstrcmp(k, "title") == 0) {
            kstrncpy(cur_icon.title, v, sizeof(cur_icon.title) - 1);
        } else if (kstrcmp(k, "exec") == 0) {
            kstrncpy(cur_icon.exec_path, v, sizeof(cur_icon.exec_path) - 1);
        } else if (kstrcmp(k, "icon") == 0) {
            kstrncpy(cur_icon.icon_path, v, sizeof(cur_icon.icon_path) - 1);
        } else if (kstrcmp(k, "x") == 0) {
            cur_icon.x = katoi(v);
        } else if (kstrcmp(k, "y") == 0) {
            cur_icon.y = katoi(v);
        }
    }

    if (in_icon) {
        if (cur_icon.title[0] != 0 && cur_icon.exec_path[0] != 0) {
            arr[n] = cur_icon;
            n++;
        }
    }

    if (count != NULL) {
        *count = n;
    }

    kfree(file);

    return arr;
}