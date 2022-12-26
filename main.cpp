#include <sys/types.h>
#include <dirent.h>
#include<unistd.h>
#include <iostream>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <netinet/in.h>

using namespace std;

class SingletonProcess {
public:
    SingletonProcess(uint16_t port0) : socket_fd(-1), rc(1), port(port0) {}

    ~SingletonProcess() {
        if (socket_fd != -1) {
            close(socket_fd);
        }
    }

    bool operator()() {
        if (socket_fd == -1 || rc) {
            socket_fd = -1;
            rc = 1;

            if ((socket_fd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
                throw std::runtime_error(std::string("Could not create socket: ") + strerror(errno));
            } else {
                struct sockaddr_in name;
                name.sin_family = AF_INET;
                name.sin_port = htons (port);
                name.sin_addr.s_addr = htonl (INADDR_ANY);
                rc = bind(socket_fd, (struct sockaddr *) &name, sizeof(name));
            }
        }
        return (socket_fd != -1 && rc == 0);
    }

    std::string GetLockFileName() {
        return "port " + std::to_string(port);
    }

private:
    int socket_fd = -1;
    int rc;
    uint16_t port;
};

pid_t proc_find(const char *name) {
    DIR *dir;
    struct dirent *ent;
    char buf[512];

    long pid;
    char pname[100] = {0,};
    char state;
    FILE *fp = NULL;

    if (!(dir = opendir("/proc"))) {
        perror("can't open /proc");
        return -1;
    }

    while ((ent = readdir(dir)) != NULL) {
        long lpid = atol(ent->d_name);
        if (lpid < 0)
            continue;
        snprintf(buf, sizeof(buf), "/proc/%ld/stat", lpid);
        fp = fopen(buf, "r");

        if (fp) {
            if ((fscanf(fp, "%ld (%[^)]) %c", &pid, pname, &state)) != 3) {
                printf("fscanf failed \n");
                fclose(fp);
                closedir(dir);
                return -1;
            }
            if (!strcmp(pname, name)) {
                fclose(fp);
                closedir(dir);
                return (pid_t) lpid;
            }
            fclose(fp);
        }
    }


    closedir(dir);
    return -1;
}


int main(void) {
    SingletonProcess singleton(5555); // pick a port number to use that is specific to this app
    if (!singleton()) {
        return 1;
    }

    while (true) {
        pid_t pid = proc_find("nwBizibox");
        if (pid == -1) {
            system("/home/user/Desktop/nwBizibox/nwBizibox");
        }
        sleep(30);
    }
    return 0;
}
