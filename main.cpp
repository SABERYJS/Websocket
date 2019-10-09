//
// Created by Administrator on 2019/9/25 0025.
//

#include "public.h"
#include "Server.h"
#include "WebsocketServer.h"
#include "EchoServer.h"
#include "Config.h"
#include "Log.h"

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
bool server_read_config_again = false;

/**
 * dispatch process according to config file
 * **/
void DispatchProcess();

/**
 * kill all child process
 * **/
void KillAllProcess();

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
void RestartChildProcess(pid_t child_pid);

/**
 * close server
 * **/
void CloseServer();

int main(int argc, char *argv[]) {
    int i = 0;
    bool config_file_specified = false;
    string config_file;
    for (; i < argc; i++) {
        if (strncmp(argv[i], "-c", 2) == 0 && i != (argc - 1)) {
            config_file_specified = true;
            config_file = argv[i + 1];
            break;
        }
    }
    if (config_file_specified) {
        try {
            global_config.Parse(config_file);
            global_log.Open(global_config.GetLogPath().c_str());
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

    } else {
        cout << "-c option is required" << endl;
        exit(1);
    }
}

void DispatchProcess() {
    if (global_config.CheckRunAsDaemon()) {
        RunAsDaemon();
    }

    MasterProcessRegisterSignalHandlers();

    int i = 0, j;
    int child_pid;
    Process *tc_process = nullptr;
    //init process list and pipes
    for (; i < global_config.GetProcessNum(); i++) {
        tc_process = new Process;
        tc_process->ready = false;
        if (pipe(tc_process->pipes) < 0) {
            global_log.Info({"Called Pipe Failed"});
            exit(0);
        }
        child_process.push_back(tc_process);
    }
    i = 0;//reset
    for (; i < global_config.GetProcessNum(); i++) {
        if ((child_pid = fork()) < 0) {
            KillAllProcess();
        } else if (child_pid == 0) {
            close(tc_process->pipes[0]);//child process only read from pipe
            //for child process ,we only care about pipe related with the process,so close others
            for (j = 0; j < i; ++j) {
                close(child_process[j]->pipes[1]);
            }
            ChildProcessBootstrap();
        } else {
            tc_process->ready = true;
            tc_process->pid_no = child_pid;
            close(tc_process->pipes[1]);//parent process only write to pipe
        }
    }
    MasterProcessBootstrap();
}

/**
 * register signal handlers
 * **/
void ChildProcessRegisterSignalHandlers() {
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
void ChildProcessSignalHandler(int, siginfo_t *, void *) {}

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
            if (!server_should_restart) {
                server_should_restart = true;
                global_log.Info({"Received SIGHUP signal,server will restart"});
            }
        case SIGCHLD:
            global_log.Info({"Process ", to_string(info->si_pid).c_str(), " Quit"});
        case SIGALRM:
            global_log.Info({"Received SIGALRM signal"});
        default:
            break;
    }
}

void KillAllProcess() {
}

void ChildProcessBootstrap(int idx) {

}

void ServerRestart() {}


void MasterProcessBootstrap() {
    int status;
    pid_t child_pid;
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
            }
        }
    }
}

void RestartChildProcess(pid_t child_pid) {
    auto iterator = child_process.begin();
    Process *n_process = new Process;
    Process *cp = *iterator;
    pid_t new_child_pid;

    //erase old
    while (iterator != child_process.end()) {
        if (cp->pid_no == child_pid) {
            close(cp->pipes[0]);
            child_process.erase(iterator);
            break;
        }
        ++iterator;
    }

    if (pipe(n_process->pipes) < 0) {
        global_log.Info({"Pipe called failed,Stop restart ", to_string(child_pid).c_str()});
        free(n_process);
        return;
    } else {
        close(n_process->pipes[1]);
        if ((new_child_pid = fork()) < 0) {
            global_log.Info({"Fork called failed,Stop restart ", to_string(child_pid).c_str()});
            free(n_process);
            return;
        } else if (new_child_pid > 0) {
            n_process->pid_no = new_child_pid;
            n_process->ready = true;
            child_process.push_back(n_process);
        } else {
            for (int i = 0; i < child_process.size(); ++i) {
                close(child_process[i]->pipes[1]);
            }
            ChildProcessBootstrap();
        }
    }
}

void CloseServer() {

}