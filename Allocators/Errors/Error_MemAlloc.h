#pragma once

#include "Error_Base.h"


enum class Method : uint8_t
{
    MALLOC,
    CALLOC,
    REALLOC,
    OTHER
};


class Error_MemAlloc : public Error_Base
{
public:
    Error_MemAlloc( size_t alloc_size_bytes, const char *file_name, const char *func_name, int line, const char *logFile_name = g_error_log_name );

    Error_MemAlloc( size_t elem_count, size_t elem_size_bytes, const char *file_name, const char *func_name, int line, const char *logFile_name = g_error_log_name );

    Error_MemAlloc( void *memory_block, size_t new_size_bytes, const char *file_name, const char *func_name, int line, const char *logFile_name = g_error_log_name );

    Error_MemAlloc( size_t alloc_size_bytes, const std::string mess, const char *file_name, const char *func_name, int line, const char *logFile_name = g_error_log_name );

    Error_MemAlloc( size_t elem_count, size_t elem_size_bytes, const std::string mess, const char *file_name, const char *func_name, int line, const char *logFile_name = g_error_log_name );

    Error_MemAlloc( void *memory_block, size_t new_size_bytes, const std::string mess, const char *file_name, const char *func_name, int line, const char *logFile_name = g_error_log_name );

protected:
    virtual std::string Name() const override;

    virtual void PrintSelf() const override;

private:
    Method m_alloc_type;
    size_t m_meaning_1;             // Размер запрошенной памяти (malloc) или количество элементов (calloc) или стартовый адрес изменяемого блока памяти (realloc)
    size_t m_meaning_2;             // Размер элемента (calloc) или новая ширина блока памяти (realloc)
    std::string m_mess;             // Доп. сообщение
};


using Error_MemAllocPtr = std::shared_ptr<Error_MemAlloc>;


#define ERR_MEMALLOC(...)              { Error_MemAllocPtr( new Error_MemAlloc(__VA_ARGS__, PLACE(), Error_Base::g_error_log_name)) }
#define ERR_MEMALLOC_EX(log_name, ...) { Error_MemAllocPtr( new Error_MemAlloc(__VA_ARGS__, PLACE(), log_name)) }

#define TRACE_MEMALLOC(e, ...)                                { if(e) { Error_Base::Trace( e, ERR_MEMALLOC(__VA_ARGS__)); }};
#define TRACE_MEMALLOC_CNT(e, ...)                            { if(e) { Error_Base::Trace( e, ERR_MEMALLOC(__VA_ARGS__)); continue; }};
#define TRACE_MEMALLOC_BRK(e, ...)                            { if(e) { Error_Base::Trace( e, ERR_MEMALLOC(__VA_ARGS__)); break; }};
#define TRACE_MEMALLOC_RET(e, ...)                            { if(e) { Error_Base::Trace( e, ERR_MEMALLOC(__VA_ARGS__)); return; }};
#define TRACE_MEMALLOC_RET_VAL(e, ret, ...)                   { if(e) { Error_Base::Trace( e, ERR_MEMALLOC(__VA_ARGS__)); return ret; }};
#define TRACE_MEMALLOC_RET_ERR(e, ...)                        { if(e) { Error_Base::Trace( e, ERR_MEMALLOC(__VA_ARGS__)); return e; }};
#define TRACE_MEMALLOC_THR_VAL(e, thr, ...)                   { if(e) { Error_Base::Trace( e, ERR_MEMALLOC(__VA_ARGS__)); throw thr; }};
#define TRACE_MEMALLOC_THR_ERR(e, ...)                        { if(e) { Error_Base::Trace( e, ERR_MEMALLOC(__VA_ARGS__)); throw e; }};
#define TRACE_MEMALLOC_PRNT(e, ...)                           { if(e) { Error_Base::Trace( e, ERR_MEMALLOC(__VA_ARGS__)); e->Print(); }};
#define TRACE_MEMALLOC_PRNT_CNT(e, ...)                       { if(e) { Error_Base::Trace( e, ERR_MEMALLOC(__VA_ARGS__)); e->Print(); continue; }};
#define TRACE_MEMALLOC_PRNT_BRK(e, ...)                       { if(e) { Error_Base::Trace( e, ERR_MEMALLOC(__VA_ARGS__)); e->Print(); break; }};
#define TRACE_MEMALLOC_PRNT_RET(e, ...)                       { if(e) { Error_Base::Trace( e, ERR_MEMALLOC(__VA_ARGS__)); e->Print(); return; }};
#define TRACE_MEMALLOC_PRNT_RET_VAL(e, ret, ...)              { if(e) { Error_Base::Trace( e, ERR_MEMALLOC(__VA_ARGS__)); e->Print(); return ret; }};
#define TRACE_MEMALLOC_PRNT_RET_ERR(e, ...)                   { if(e) { Error_Base::Trace( e, ERR_MEMALLOC(__VA_ARGS__)); e->Print(); return e; }};
#define TRACE_MEMALLOC_PRNT_THR_VAL(e, thr, ...)              { if(e) { Error_Base::Trace( e, ERR_MEMALLOC(__VA_ARGS__)); e->Print(); throw thr; }};
#define TRACE_MEMALLOC_PRNT_THR_ERR(e, ...)                   { if(e) { Error_Base::Trace( e, ERR_MEMALLOC(__VA_ARGS__)); e->Print(); throw e; }};

#define TRACE_MEMALLOC_EX(e, log_name, ...)                   { if(e) { Error_Base::Trace( e, ERR_MEMALLOC_EX(log_name, __VA_ARGS__)); }};
#define TRACE_MEMALLOC_CNT_EX(e, log_name, ...)               { if(e) { Error_Base::Trace( e, ERR_MEMALLOC_EX(log_name, __VA_ARGS__)); continue; }};
#define TRACE_MEMALLOC_BRK_EX(e, log_name, ...)               { if(e) { Error_Base::Trace( e, ERR_MEMALLOC_EX(log_name, __VA_ARGS__)); break; }};
#define TRACE_MEMALLOC_RET_EX(e, log_name, ...)               { if(e) { Error_Base::Trace( e, ERR_MEMALLOC_EX(log_name, __VA_ARGS__)); return; }};
#define TRACE_MEMALLOC_RET_VAL_EX(e, log_name, ret, ...)      { if(e) { Error_Base::Trace( e, ERR_MEMALLOC_EX(log_name, __VA_ARGS__)); return ret; }};
#define TRACE_MEMALLOC_RET_ERR_EX(e, log_name, ...)           { if(e) { Error_Base::Trace( e, ERR_MEMALLOC_EX(log_name, __VA_ARGS__)); return e; }};
#define TRACE_MEMALLOC_THR_VAL_EX(e, log_name, thr, ...)      { if(e) { Error_Base::Trace( e, ERR_MEMALLOC_EX(log_name, __VA_ARGS__)); throw thr; }};
#define TRACE_MEMALLOC_THR_ERR_EX(e, log_name, ...)           { if(e) { Error_Base::Trace( e, ERR_MEMALLOC_EX(log_name, __VA_ARGS__)); throw e; }};
#define TRACE_MEMALLOC_PRNT_EX(e, log_name, ...)              { if(e) { Error_Base::Trace( e, ERR_MEMALLOC_EX(log_name, __VA_ARGS__)); e->Print(); }};
#define TRACE_MEMALLOC_PRNT_CNT_EX(e, log_name, ...)          { if(e) { Error_Base::Trace( e, ERR_MEMALLOC_EX(log_name, __VA_ARGS__)); e->Print(); continue; }};
#define TRACE_MEMALLOC_PRNT_BRK_EX(e, log_name, ...)          { if(e) { Error_Base::Trace( e, ERR_MEMALLOC_EX(log_name, __VA_ARGS__)); e->Print(); break; }};
#define TRACE_MEMALLOC_PRNT_RET_EX(e, log_name, ...)          { if(e) { Error_Base::Trace( e, ERR_MEMALLOC_EX(log_name, __VA_ARGS__)); e->Print(); return; }};
#define TRACE_MEMALLOC_PRNT_RET_VAL_EX(e, log_name, ret, ...) { if(e) { Error_Base::Trace( e, ERR_MEMALLOC_EX(log_name, __VA_ARGS__)); e->Print(); return ret; }};
#define TRACE_MEMALLOC_PRNT_RET_ERR_EX(e, log_name, ...)      { if(e) { Error_Base::Trace( e, ERR_MEMALLOC_EX(log_name, __VA_ARGS__)); e->Print(); return e; }};
#define TRACE_MEMALLOC_PRNT_THR_VA_EXL(e, log_name, thr, ...) { if(e) { Error_Base::Trace( e, ERR_MEMALLOC_EX(log_name, __VA_ARGS__)); e->Print(); throw thr; }};
#define TRACE_MEMALLOC_PRNT_THR_ERR_EX(e, log_name, ...)      { if(e) { Error_Base::Trace( e, ERR_MEMALLOC_EX(log_name, __VA_ARGS__)); e->Print(); throw e; }};
