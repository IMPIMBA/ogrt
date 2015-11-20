       #include <sys/types.h>
       #include <sys/stat.h>
       #include <stdio.h>
       #include <stdlib.h>
       #include <unistd.h>

       int
       main(int argc, char *argv[])
       {
           struct stat sb;
           char *linkname;
           ssize_t r;

           if (argc != 2) {
               fprintf(stderr, "Usage: %s <pathname>\n", argv[0]);
               exit(EXIT_FAILURE);
           }

           if (lstat(argv[1], &sb) == -1) {
               perror("lstat");
               exit(EXIT_FAILURE);
           }

           linkname = malloc(sb.st_size + 1);
           if (linkname == NULL) {
               fprintf(stderr, "insufficient memory\n");
               exit(EXIT_FAILURE);
           }

           r = readlink(argv[1], linkname, sb.st_size + 1);

           if (r == -1) {
               perror("lstat");
               exit(EXIT_FAILURE);
           }

           if (r > sb.st_size) {
               fprintf(stderr, "symlink increased in size "
                               "between lstat() and readlink()\n");
               exit(EXIT_FAILURE);
           }

           linkname[r] = '\0';

           printf("'%s' points to '%s'\n", argv[1], linkname);

           exit(EXIT_SUCCESS);
       }

