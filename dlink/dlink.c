#include <unistd.h>
#include <dlfcn.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <errno.h>

int main(int argc, char* argv[])
{
    if (argc != 4) {
        printf("Usage: %s [function] [lower bound] [upper bound]\n", argv[0]);
        return 1;
    }
    int gcc_pipe[2];
    if (pipe(gcc_pipe)) {
        perror("pipe");
        return 1;
    }
    pid_t child_pid = fork();
    if (child_pid < 0) {
        perror("fork");
        return 2;
    } else if (child_pid == 0) {
        close(gcc_pipe[1]);
        int gcc_inputfd = dup2(gcc_pipe[0], STDIN_FILENO);
        if (gcc_inputfd < 0) {
            perror("dup2");
            close(gcc_pipe[0]);
            return 1;
        }
        close(gcc_pipe[0]);
        execlp(
            "gcc", "gcc", "-Wall", "-Wextra", "-O3",
            "-fPIC", "-fPIE", "-shared",
            "-xc", "-", "-lm", "-o" "tmp.so",
            NULL
        );
        perror("Exec GCC");
        return 1;
    }
    close(gcc_pipe[0]);
    FILE* gcc_istream = fdopen(gcc_pipe[1], "w");
    fprintf(
        gcc_istream, 
        "#include <math.h>\n"
        "double f(double x) {\n"
        "   return %s;\n"
        "}\n",
        argv[1]
    );
    if (fclose(gcc_istream)) {
        perror("fclose");
        close(gcc_pipe[1]);
        return 1;
    }
    close(gcc_pipe[1]);
    int status;
    waitpid(child_pid, &status, 0);
    if (!WIFEXITED(status) || WEXITSTATUS(status)) {
        printf("Error, gcc exited with code %d\n", WEXITSTATUS(status));
        return 1;
    }

    void* lib = dlopen("./tmp.so", RTLD_LAZY);
    char* dlerrstr;
    if(lib == NULL) {
        perror(dlerror());
        return 4;
    }

    double (*fun)(double);
    *(void **) (&fun) = dlsym(lib, "f");
    if ((dlerrstr = dlerror()) != NULL) {
        puts(dlerrstr);
        dlclose(lib);
        return 1;
    }

    errno = 0;
    double lb = strtod(argv[2], NULL);
    if (errno == ERANGE) {
        printf("Incorrect lower bound value.\n");
        return 1;
    }
    double ub = strtod(argv[3], NULL);
    if (errno == ERANGE) {
        printf("Incorrect upper bound value.\n");
        return 1;
    }
    double res = 0.0;
    double pas = 0.001;
    unsigned long iter = 1;
    double x = lb;
    while (x < ub) {
        res += pas * (*fun)(x);
        x = lb + (iter * pas);
        iter++;
    }
    printf("%lf\n", res);
    return 0;
}