//
// Created by Administrator on 2019/9/25 0025.
//

#include "public.h"
#include "Server.h"
#include "WebsocketServer.h"
#include "EchoServer.h"
#include "Config.h"
#include "Log.h"
#include "Daemon.h"

#define  PIPE_BLOCK_SIZE 1024
#define  APP_ERROR_EXIT_CODE -1
#define  APP_NORMAL_EXIT_CODE 0

#define CloseProcess(code)  exit(code)
//following is global variables
Config global_config;
Log global_log;
EchoServer *server;


//used for manage child process
struct Process {
    bool ready;
    pid_t pid_no;//process id
    int pipes[2]; //pipe to communicate between parent and child process,pipe[0] for writing,pipe[1] for reading
};

//package protocol used for communicating between  master and child process
struct Package {
    size_t len;//package length
    char type;//package type
    void *data;//package overload, it is maybe empty
};

std::vector<Process *> child_process;
pid_t parent_pid;
pid_t self_pid;
bool scheduled_close_server = false;
bool server_should_restart = false;
bool has_notify_child_process_close = false;
int self_related_pipes[2];//current process related pipe
char block[PIPE_BLOCK_SIZE] = {0};//pipe read buffer
int last_block_write_pos = 0;//position for writing next time
int response_count_from_client = 0;//used for close or restart server
EventLoop global_event_loop; //global event system
string config_file; //config file


/**
 * dispatch process according to config file
 * **/
void DispatchProcess();


/**
 * register signal handlers
 * **/
void ChildProcessRegisterSignalHandlers();

/**
 * register signal handlers
 * **/
void MasterProcessRegisterSignalHandlers();

/**
 * handler to process signal
 * **/
void ChildProcessSignalHandler(int, siginfo_t *, void *);

/**
 * handler to process signal
 * **/
void MasterProcessSignalHandler(int, siginfo_t *, void *);

/**
 * bootstrap  child process
 * **/
void ChildProcessBootstrap();

/**
 * bootstrap  master process
 * **/
void MasterProcessBootstrap();

/**
 * restart server
 * **/
void ServerRestart();

/**
 * restart child process
 * **/
void RestartChildProcess(pid_t child_pid = 0);

/**
 * close server
 * **/
void CloseServer();

void NotifyChildProcessClose();

void ParseConfig();

void BootstrapServer();

/**
 * set process title
 * **/
int SetProcessTitle(int argc, char **argv, char *title, int len);

inline bool AllChildProcessClosed() {
    return child_process.size() == response_count_from_client;
}

inline bool NewChildProcessClosed() {
    ++response_count_from_client;
}

inline void SetChildProcessStatus(pid_t pid) {
    auto iterator = child_process.begin();
    while (iterator != child_process.end()) {
        if ((*iterator)->pid_no == pid) {
            (*iterator)->ready = false;
            break;
        }
        ++iterator;
    }
}

struct : public EventCallback {
    bool Handle(bool socket_should_close, void *event_loop) override {
        int ret;
        Package *pkg = nullptr;
        while (true) {
            try_again:
            if ((ret = read(self_related_pipes[1], block, (PIPE_BLOCK_SIZE - last_block_write_pos))) < 0) {
                if (errno == EINTR) {
                    goto try_again;
                } else {
                    //quit directly
                    global_log.Info({"Child Process ", to_string(self_pid).c_str(), " quit unusual"});
                    CloseProcess(APP_ERROR_EXIT_CODE);
                }
            } else {
                last_block_write_pos += ret;
                //no enough data
                if (last_block_write_pos < sizeof(Package)) {
                    return true;
                } else {
                    pkg = (Package *) block;
                    if (pkg->type == 1) {
                        CloseProcess(APP_NORMAL_EXIT_CODE);
                    } else {
                        //do nothing now,reserved for future
                    }
                }
            }
        }
    }
} child_pipe_event_handler;


int main(int argc, char *argv[]) {
    int i = 0;
    bool config_file_specified = false;
    for (; i < argc; i++) {
        if (strncmp(argv[i], "-c", 2) == 0 && i != (argc - 1)) {
            config_file_specified = true;
            config_file = argv[i + 1];
            break;
        }
    }
    if (config_file_specified) {
        BootstrapServer();
    } else {
        cout << "-c option is required" << endl;
        exit(1);
    }
}

void DispatchProcess() {

    MasterProcessRegisterSignalHandlers();

    int i = 0;
    for (; i < global_config.GetProcessNum(); i++) {
        RestartChildProcess();
    }
    MasterProcessBootstrap();
}

/**
 * register signal handlers
 * **/
void ChildProcessRegisterSignalHandlers() {
    struct sigaction sig;
    sigemptyset(&sig.sa_mask);
    sig.sa_flags = SA_SIGINFO;
    sig.sa_sigaction = ChildProcessSignalHandler;
    sigaction(SIGINT, &sig, nullptr);
    sigaction(SIGTERM, &sig, nullptr);
    signal(SIGHUP, SIG_IGN);
    signal(SIGALRM, SIG_IGN);
}

/**
 * register signal handlers
 * **/
void MasterProcessRegisterSignalHandlers() {
    struct sigaction sig;
    sigemptyset(&sig.sa_mask);
    sig.sa_flags = SA_SIGINFO;
    sig.sa_sigaction = MasterProcessSignalHandler;
    sigaction(SIGINT, &sig, nullptr);
    sigaction(SIGTERM, &sig, nullptr);
    sigaction(SIGCHLD, &sig, nullptr);
    sigaction(SIGHUP, &sig, nullptr);
    sigaction(SIGALRM, &sig, nullptr);
}

/**
 * handler to process signal
 * **/
void ChildProcessSignalHandler(int sig_no, siginfo_t *info, void *context) {
    switch (sig_no) {
        case SIGINT:
        case SIGTERM:
            global_log.Info({"Child Process ", to_string(self_pid).c_str(), " receive close signal,so quit normally"});
            CloseProcess(APP_NORMAL_EXIT_CODE);
            break;
    }
}

/**
 * handler to process signal
 * **/
void MasterProcessSignalHandler(int sig_no, siginfo_t *info, void *context) {
    switch (sig_no) {
        case SIGINT:
            if (!scheduled_close_server) {
                scheduled_close_server = true;
                global_log.Info({"Received SIGINT signal,server will close"});
            }
            break;
        case SIGTERM:
            if (!scheduled_close_server) {
                scheduled_close_server = true;
                global_log.Info({"Received SIGTERM signal,server will close"});
            }
            break;
        case SIGHUP:
            //received SIGTERM or SIGINT signal before SIGHUP
            if (!scheduled_close_server) {
                if (!server_should_restart) {
                    server_should_restart = true;
                    global_log.Info({"Received SIGHUP signal,server will restart"});
                }
            }
        case SIGCHLD:
            global_log.Info({"Process ", to_string(info->si_pid).c_str(), " Quit"});
        case SIGALRM:
            global_log.Info({"Received SIGALRM signal"});
        default:
            break;
    }
}

void ChildProcessBootstrap() {
    char process_title[100] = {0};
    sprintf(process_title, "websocket_worker_process_%d", self_pid);
    SetProcessTitle(__arg_type_tag())
    ChildProcessRegisterSignalHandlers();
    global_event_loop.AddEvent(self_related_pipes[1], EVENT_READABLE, &child_pipe_event_handler);
    server->Loop();
    server->WaitClient();
}

void ServerRestart() {
    //notify child process closing itself
    if (!has_notify_child_process_close) {
        NotifyChildProcessClose();
    } else {
        //check response count from child process
        if (AllChildProcessClosed()) {
            //all child process closed
            child_process.clear();
            for (int i = 0; i < global_config.GetProcessNum(); i++) {
                RestartChildProcess();
            }
            //reset global flags
            server_should_restart = false;
            has_notify_child_process_close = false;
            response_count_from_client = 0;
        }
    }
}


void MasterProcessBootstrap() {
    int status;
    pid_t child_pid;
    self_pid = getpid();
    parent_pid = getppid();
    //close listen socket,because master process do not receive remote client connection
    server->CloseListenSocket();
    while (true) {
        if ((child_pid = wait(&status)) < 0) {
            if (errno == EINTR) {
                //wait was interrupted,so we check global flag
                if (server_should_restart) {
                    server_should_restart = false;
                    ServerRestart();
                } else if (scheduled_close_server) {
                    scheduled_close_server = false;
                    CloseServer();
                }
                continue;
            } else {
                CloseServer();
            }
        } else {
            //check  whether child process quit normal
            if (!WIFEXITED(status)) {
                //child process quit unusual
                RestartChildProcess(child_pid);
            } else {
                //child process was killed by master process
                global_log.Info({"Process", to_string(child_pid).c_str(), " Quit normal"});
                //statics closed process count
                NewChildProcessClosed();
                SetChildProcessStatus(child_pid);
            }
        }
    }
}

void RestartChildProcess(pid_t child_pid) {
    auto iterator = child_process.begin();
    Process *n_process = new Process;
    Process *cp = *iterator;
    pid_t new_child_pid;

    if (child_pid > 0) {
        //erase old
        while (iterator != child_process.end()) {
            if (cp->pid_no == child_pid) {
                close(cp->pipes[0]);
                child_process.erase(iterator);
                break;
            }
            ++iterator;
        }
    }

    if (pipe(n_process->pipes) < 0) {
        global_log.Info({"Pipe called failed,Stop restart ", to_string(child_pid).c_str()});
        free(n_process);
        exit(1);
    } else {
        if ((new_child_pid = fork()) < 0) {
            global_log.Info({"Fork called failed,Stop restart ", to_string(child_pid).c_str()});
            free(n_process);
            exit(1);
        } else if (new_child_pid > 0) {
            close(n_process->pipes[1]);
            n_process->pid_no = new_child_pid;
            n_process->ready = true;
            child_process.push_back(n_process);//only parent process  required
        } else {
            memcpy(self_related_pipes, n_process->pipes, 2);
            self_pid = getpid();
            parent_pid = getppid();
            for (int i = 0; i < child_process.size(); ++i) {
                close(child_process[i]->pipes[1]);//inherit from parent process
            }
            ChildProcessBootstrap();
        }
    }
}

void CloseServer() {
    //firstly,we notify child process closing itself
    if (!has_notify_child_process_close) {
        NotifyChildProcessClose();
    } else {
        if (AllChildProcessClosed()) {
            global_log.Info({"Master quit usual"});
            CloseProcess(APP_NORMAL_EXIT_CODE);
        }
    }
}

void NotifyChildProcessClose() {
    Package *pkg = new Package;
    pkg->type = 1;//type=1,close
    pkg->len = sizeof(Package);
    size_t len = pkg->len;
    for (int i = 0; i < child_process.size(); ++i) {
        write(child_process[i]->pipes[0], pkg, len);
    }
    has_notify_child_process_close = true;
}

/**
 * parse config file
 * **/
void ParseConfig() {
    global_config.Parse(config_file);
    global_log.Open(global_config.GetLogPath().c_str());
}

/**
 * read config again
 * **/
void BootstrapServer() {
    try {
        ParseConfig();
        if (global_config.CheckRunAsDaemon()) {
            RunAsDaemon();
        }
        server = new EchoServer(global_config.GetPort(), INADDR_ANY, 10);
        server->Loop();
        DispatchProcess();
    } catch (const char *error) {
        cout << error << endl;
        exit(1);
    } catch (std::runtime_error error) {
        cout << error.what() << endl;
        exit(1);
    } catch (SystemCallException &exception) {
        cout << exception.what() << endl;
        exit(1);
    }
}


int SetProcessTitle(int argc, char **argv, char *title, int len) {
    //linux system,argv and environ  stored one after another
    //so set process title is complicated
    char **p = argv;
    size_t al = strlen(p[0]);
    if (al >= len) {
        //argv[0] has enough memory to store title,then just set argv[0]
        memcpy(p[0], title, len);
        //left space  reset to 0
        memset(p[0] + len, 0, (al - len));
        return 1;
    } else {
        //firstly allocate memory to store environment variables
        size_t tl = 0;
        size_t til;
        char **pe = environ;
        char *envBuf;
        char *next;
        int i = 0;
        size_t nl;
        while (*pe) {
            tl += (strlen(*pe) + 1);//tail 0
            pe++;
        }
        envBuf = static_cast<char *>(malloc(tl));
        if (!envBuf) {
            return -1;
        }
        pe = environ;//reset again
        next = envBuf;
        while (*pe) {
            til = strlen(*pe);
            memcpy(next, *pe, til);
            environ[i++] = next;//reset global environ again
            next += (til + 1);//include tail 0
            pe++;
        }

        //args move backward,firstly calculate total memory length to store all  application args
        tl = 0;
        while (*p) {
            tl += (strlen(*p) + 1);//include tail 0
            p++;
        }
        p = argv;//reset again
        nl = tl - al + len;//new args length
        i = argc - 1;//copy from the last element
        next = p[0] + nl;//last storage position
        while (i >= 0) {
            if (!i) {
                til = len;
            } else {
                til = strlen(p[i]);
            }
            next -= (til + 1);
            memcpy(next, !i ? title : p[i], til);
            argv[i] = next;//reset argv
            next[til] = 0;//Required,Very important
            i--;
        }
        return 1;
    }
}