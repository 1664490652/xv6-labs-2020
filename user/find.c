#include "kernel/types.h"
#include "kernel/stat.h"

#include "user/user.h"
#include "kernel/fs.h"

/**
 * 检查文件名是否以目标字符串结尾
 */
int ends_with(const char* filepath, const char* target) {
    int path_len = strlen(filepath);
    int target_len = strlen(target);
    
    if (path_len < target_len) {
        return 0;
    }
    
    return strcmp(filepath + path_len - target_len, target) == 0;
}

/**
 * 构建完整的文件路径
 */
void build_path(char* buffer, const char* dir_path, const char* filename) {
    strcpy(buffer, dir_path);
    char* end = buffer + strlen(buffer);
    *end++ = '/';
    memmove(end, filename, DIRSIZ);
    end[DIRSIZ] = '\0';
}

/**
 * 检查是否应该跳过的目录项
 */
int should_skip_entry(struct dirent* entry) {
    return entry->inum == 0 || 
           strcmp(entry->name, ".") == 0 || 
           strcmp(entry->name, "..") == 0;
}

/**
 * 处理文件：检查是否匹配目标
 */
void handle_file(const char* filepath, const char* target) {
    if (ends_with(filepath, target)) {
        printf("%s\n", filepath);
    }
}

/**
 * 处理目录：遍历其中的所有项目
 */
void handle_directory(const char* dir_path, const char* target, int fd) {
    char full_path[512];
    struct dirent dir_entry;
    struct stat file_stat;
    
    // 检查路径长度是否会溢出
    if (strlen(dir_path) + 1 + DIRSIZ + 1 > sizeof(full_path)) {
        printf("find: path too long\n");
        return;
    }
    
    // 遍历目录中的每个条目
    while (read(fd, &dir_entry, sizeof(dir_entry)) == sizeof(dir_entry)) {
        // 跳过无效条目和特殊目录
        if (should_skip_entry(&dir_entry)) {
            continue;
        }
        
        // 构建完整路径
        build_path(full_path, dir_path, dir_entry.name);
        
        // 获取文件状态
        if (stat(full_path, &file_stat) < 0) {
            printf("find: cannot stat %s\n", full_path);
            continue;
        }
        
        // 递归查找
        find(full_path, target);
    }
}

/**
 * 在指定路径中查找目标文件
 * @param path 要搜索的路径
 * @param target 要查找的目标文件名
 */
void find(const char* path, const char* target) {
    struct stat path_stat;
    int fd;
    
    // 打开路径
    fd = open(path, 0);
    if (fd < 0) {
        fprintf(2, "find: cannot open %s\n", path);
        return;
    }
    
    // 获取路径状态信息
    if (fstat(fd, &path_stat) < 0) {
        fprintf(2, "find: cannot stat %s\n", path);
        close(fd);
        return;
    }
    
    // 根据路径类型进行不同处理
    switch (path_stat.type) {
        case T_FILE:
            handle_file(path, target);
            break;
            
        case T_DIR:
            handle_directory(path, target, fd);
            break;
            
        default:
            // 未知文件类型，可以选择忽略或报错
            break;
    }
    
    close(fd);
}

int main(int argc, char **argv) {
    if(argc != 3){
        fprintf(2, "Usage: find <path> <filename>\n");
        exit(1);
    }

    find(argv[1], argv[2]);
    exit(0);
}