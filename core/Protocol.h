#ifndef PROTOCOL_H
#define PROTOCOL_H

// Константи
const int MAX_IDEAS = 100;
const int TEXT_SIZE = 256;
const int UUID_SIZE = 40; 

// Імена ресурсів
const char* const SHM_NAME = "Local\\MyLabSharedMem";
const char* const MUTEX_NAME = "Local\\MyLabMutex";
const char* const PIPE_NAME = "\\\\.\\pipe\\MyLabPipe";

// Структура ідеї
struct Idea {
    char uuid[UUID_SIZE];      // Унікальний ID
    char text[TEXT_SIZE];      // Текст
    int votes;                 // Голоси
    int worker_id;             // Автор
};

// Структура дошки (Shared Memory)
struct BoardLayout {
    Idea ideas[MAX_IDEAS];
    int count;
    bool is_stopped;
};

// Команди пайпів
enum PipeCmd {
    CMD_ADD_IDEA = 1,
    CMD_GET_ALL = 2,
    CMD_VOTE = 3,
    CMD_CHECK_STOP = 4
};

// Заголовок повідомлення пайпа
struct PipeMessageHeader {
    int command;
    int data_size;
};

#endif // PROTOCOL_H