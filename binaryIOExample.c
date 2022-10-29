#define _GNU_SOURCE

#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <errno.h>

#define VERBOSE 2
#define BYTES_IN_MB 1048576
#define MAX_MB_IN_MEMORY 1

#if VERBOSE >= 2
#include <sys/time.h>
#endif

int read_file(char *, size_t, void **);
int write_file(char *, size_t, void *);
int iterative_copy(char *, char *, size_t);
int file_check(char *, unsigned, size_t *);
void evaluate_time(struct timeval, struct timeval);
void usage (char *);
void my_log (unsigned, char *);

int main(int argc, char **argv){

    void *buffer;
    size_t src_size;

#if VERBOSE >= 2
    struct timeval t1, t2;
#endif

    if (argc < 3)
        usage(*argv);

    if (file_check(argv[1], 0, &src_size)) {
#if VERBOSE
        my_log(1, "Error opening the src file.");
#endif
        exit(EXIT_FAILURE);
    }
    
    if (file_check(argv[2], 1, NULL)) {
#if VERBOSE
        my_log(1, "Error opening the dst file.");
#endif
        exit(EXIT_FAILURE);
    }

    if (src_size > MAX_MB_IN_MEMORY * BYTES_IN_MB) {
#if VERBOSE >= 2
        gettimeofday(&t1, NULL);
#endif
        if (iterative_copy(argv[1], argv[2], src_size)){
#if VERBOSE
            my_log(1, "Error coping the file.");
#endif
            exit(EXIT_FAILURE);
        }
#if VERBOSE >= 2
        gettimeofday(&t2, NULL);
        evaluate_time(t1, t2);
#endif
        exit(EXIT_SUCCESS);
    }
    
#if VERBOSE >= 2
        gettimeofday(&t1, NULL);
#endif
    if (read_file(argv[1], src_size, &buffer)) {
#if VERBOSE
        my_log(1, "Error reading the src.");
#endif
        exit(EXIT_FAILURE);
    }

    if (write_file(argv[2], src_size, buffer)) {
#if VERBOSE
        my_log(1, "Error writing to the dst.");
#endif
        free(buffer);
        exit(EXIT_FAILURE);
    }
#if VERBOSE >= 2
        gettimeofday(&t2, NULL);
        evaluate_time(t1, t2);
#endif

    free(buffer);
    exit(EXIT_SUCCESS);

}

int read_file(char *path, size_t size, void **buffer) {

    FILE *fp;

    if ((fp = fopen(path, "rb")) == NULL)
        return -1;

    if ((*buffer = malloc(size)) == NULL)
        return -2;

    if (fread(*buffer, 1, size, fp) != size)
        return -3;

    fclose(fp);
    return 0;

}

int write_file(char *path, size_t size, void *buffer) {

    FILE *fp;

    if ((fp = fopen(path, "wb")) == NULL)
        return -1;

    if (fwrite(buffer, 1, size, fp) != size)
        return -2;
    
    fclose(fp);
    return 0;

}

int iterative_copy(char *src, char *dst, size_t size){

    void *buffer;
    FILE *fp_r, *fp_w;
    size_t remaining_size, copied_size;

    if ((fp_r = fopen(src, "rb")) == NULL)
        return -1;

    if ((fp_w = fopen(dst, "wb")) == NULL)
        return -2;

    if ((buffer = malloc(MAX_MB_IN_MEMORY * BYTES_IN_MB)) == NULL)
        return -3;

    for (remaining_size = size; remaining_size > 0; remaining_size -= copied_size) {
        if (remaining_size < MAX_MB_IN_MEMORY * BYTES_IN_MB){
            if (fread(buffer, 1, remaining_size, fp_r) != remaining_size) {
                free(buffer);
                return -4;
            }
            if ((copied_size = fwrite(buffer, 1, remaining_size, fp_w)) != remaining_size) {
                free(buffer);
                return -4;
            }
        } else {
            if (fread(buffer, 1, MAX_MB_IN_MEMORY * BYTES_IN_MB, fp_r) != MAX_MB_IN_MEMORY * BYTES_IN_MB) {
                free(buffer);
                return -4;
            }
            if ((copied_size = fwrite(buffer, 1, MAX_MB_IN_MEMORY * BYTES_IN_MB, fp_w)) != MAX_MB_IN_MEMORY * BYTES_IN_MB) {
                free(buffer);
                return -4;
            }
        }
    }
    
    fclose(fp_r);
    fclose(fp_w);
    free(buffer);
    return 0;

}

int file_check (char *path, unsigned write, size_t *size) {

    FILE *fp;
    unsigned exists;
    struct stat sb;

    if (!(exists = stat(path, &sb) == 0) && !write)
        return errno;

    if (!exists) {
        if (size != NULL)
            *size = 0;
        // Check the write permission on the parent folder
        if ((fp = fopen(path, "wb")) == NULL)
            return -1;
        fclose(fp);
        return remove(path);
    }

    switch (sb.st_mode & S_IFMT) {
        case S_IFREG:   break;
        default:        return -1;
    }

    if (size != NULL)
        *size = sb.st_size;

    if ((getuid() == sb.st_uid || geteuid() == sb.st_uid) ) {
        if (write && (sb.st_mode & S_IWUSR))
            return 0;
        else if (!write && (sb.st_mode & S_IRUSR))
            return 0;
    }

    if ((getgid() == sb.st_gid || getegid() == sb.st_gid) && (sb.st_mode & S_IRUSR)) {
        if (write && (sb.st_mode & S_IWGRP))
            return 0;
        else if (!write && (sb.st_mode & S_IRGRP))
            return 0;
    }

    if (write && (sb.st_mode & S_IWOTH))
        return 0;
    else if (!write && (sb.st_mode & S_IROTH))
        return 0; 

    return -1;
}

#if VERBOSE >= 2
void evaluate_time(struct timeval t1, struct timeval t2) {
    
    char *log_string;
    double elapsed_time;

    elapsed_time = (t2.tv_sec - t1.tv_sec) * 1000.0;
    elapsed_time += (t2.tv_usec - t1.tv_usec) / 1000.0;

    if(asprintf(&log_string, "Elapsed time: %f ms", elapsed_time) < 0){
        my_log(1, "Can't format the log string.");
        return;
    }

    my_log(0, log_string);
    free(log_string);

}
#endif

void usage (char *name) {

    char *log_string;

    if(asprintf(&log_string, "Usage: %s src dst", name) < 0){
        my_log(1, "Can't format the log string.");
    } else {
        my_log(0, log_string);
        free(log_string);
    }

    exit(EXIT_FAILURE);

}

void my_log (unsigned error, char *msg) {

    switch (error)
    {
    case 0:
        fprintf(stdout, "[*] %s\n", msg);
        break;
    case 1:
        fprintf(stderr, "[!] %s\n", msg);
        break;
    default:
        break;
    }

}