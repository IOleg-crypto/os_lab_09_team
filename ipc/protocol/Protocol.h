#ifndef PROTOCOL_H
#define PROTOCOL_H

// Константи
constexpr int MAX_IDEAS = 100;
constexpr int TEXT_SIZE = 256;
constexpr int UUID_SIZE = 40;

// Імена ресурсів
const char* const SHM_NAME = "Local\\MyLabSharedMem";
const char* const MUTEX_NAME = "Local\\MyLabMutex";
const char* const PIPE_NAME = "\\\\.\\pipe\\MyLabPipe";

// Idea struct
struct Idea {
    char uuid[UUID_SIZE];      // UUID
    char text[TEXT_SIZE];      // Text
    int votes;                 // Votes
    int worker_id;             // Author
};

// Board struct (Shared Memory)
struct BoardLayout {
    Idea ideas[MAX_IDEAS];
    int count;
    bool is_stopped;
};

// Pipes command
enum PipeCmd {
    CMD_ADD_IDEA = 1,
    CMD_GET_ALL = 2,
    CMD_VOTE = 3,
    CMD_CHECK_STOP = 4
};

// Header of pipes message
struct PipeMessageHeader {
    int command;
    int data_size;
};

#endif // PROTOCOL_H
