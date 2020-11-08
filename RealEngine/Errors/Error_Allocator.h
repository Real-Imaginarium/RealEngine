#pragma once

#include "Error_Base.h"

enum class EA_Type : uint8_t
{
    NONE,
    DEALLOC_REGION_OUT_OF_BOUNDS,
    ALLOC_SIZE_TOO_BIG,
    MANAGED_BLOCK_CREATION_ERROR,
    REGIONS_LIST_CREATION_ERROR
};


class Error_Allocator : public Error_Base
{
public:
    Error_Allocator( EA_Type err_type, void *dealloc_start, size_t dealloc_size, void *managed_start, size_t managed_size, const char *file, const char *func, int line, const char *logFile = g_error_log_name );
    Error_Allocator( EA_Type err_type, void *managed_start, size_t managed_size, const char *file, const char *func, int line, const char *logFile = g_error_log_name );
    Error_Allocator( EA_Type err_type, size_t size, const char *file, const char *func, int line, const char *logFile = g_error_log_name );

    EA_Type Elem() const { return m_errType; }

protected:
    virtual std::string Name() const override;

    virtual void PrintSelf() const override;

private:
    EA_Type m_errType;
    size_t m_reg_start_1;
    size_t m_reg_start_2;
    size_t m_reg_size_1;
    size_t m_reg_size_2;
};


using Error_AllocatorPtr = std::shared_ptr<Error_Allocator>;


#define ERR_ALLOCATOR(...)              { Error_AllocatorPtr( new Error_Allocator( __VA_ARGS__, PLACE(), Error_Base::g_error_log_name)) }
#define ERR_ALLOCATOR_EX(log_name, ...) { Error_AllocatorPtr( new Error_Allocator( __VA_ARGS__, PLACE(), log_name)) }

#define TRACE_ALLOCATOR(e, ...)                                { if(e) { Error_Base::Trace( e, ERR_ALLOCATOR(__VA_ARGS__)); }};
#define TRACE_ALLOCATOR_CNT(e, ...)                            { if(e) { Error_Base::Trace( e, ERR_ALLOCATOR(__VA_ARGS__)); continue; }};
#define TRACE_ALLOCATOR_BRK(e, ...)                            { if(e) { Error_Base::Trace( e, ERR_ALLOCATOR(__VA_ARGS__)); break; }};
#define TRACE_ALLOCATOR_RET(e, ...)                            { if(e) { Error_Base::Trace( e, ERR_ALLOCATOR(__VA_ARGS__)); return; }};
#define TRACE_ALLOCATOR_RET_VAL(e, ret, ...)                   { if(e) { Error_Base::Trace( e, ERR_ALLOCATOR(__VA_ARGS__)); return ret; }};
#define TRACE_ALLOCATOR_RET_ERR(e, ...)                        { if(e) { Error_Base::Trace( e, ERR_ALLOCATOR(__VA_ARGS__)); return e; }};
#define TRACE_ALLOCATOR_THR_VAL(e, thr, ...)                   { if(e) { Error_Base::Trace( e, ERR_ALLOCATOR(__VA_ARGS__)); throw thr; }};
#define TRACE_ALLOCATOR_THR_ERR(e, ...)                        { if(e) { Error_Base::Trace( e, ERR_ALLOCATOR(__VA_ARGS__)); throw e; }};
#define TRACE_ALLOCATOR_PRNT(e, ...)                           { if(e) { Error_Base::Trace( e, ERR_ALLOCATOR(__VA_ARGS__)); e->Print(); }};
#define TRACE_ALLOCATOR_PRNT_CNT(e, ...)                       { if(e) { Error_Base::Trace( e, ERR_ALLOCATOR(__VA_ARGS__)); e->Print(); continue; }};
#define TRACE_ALLOCATOR_PRNT_BRK(e, ...)                       { if(e) { Error_Base::Trace( e, ERR_ALLOCATOR(__VA_ARGS__)); e->Print(); break; }};
#define TRACE_ALLOCATOR_PRNT_RET(e, ...)                       { if(e) { Error_Base::Trace( e, ERR_ALLOCATOR(__VA_ARGS__)); e->Print(); return; }};
#define TRACE_ALLOCATOR_PRNT_RET_VAL(e, ret, ...)              { if(e) { Error_Base::Trace( e, ERR_ALLOCATOR(__VA_ARGS__)); e->Print(); return ret; }};
#define TRACE_ALLOCATOR_PRNT_RET_ERR(e, ...)                   { if(e) { Error_Base::Trace( e, ERR_ALLOCATOR(__VA_ARGS__)); e->Print(); return e; }};
#define TRACE_ALLOCATOR_PRNT_THR_VAL(e, thr, ...)              { if(e) { Error_Base::Trace( e, ERR_ALLOCATOR(__VA_ARGS__)); e->Print(); throw thr; }};
#define TRACE_ALLOCATOR_PRNT_THR_ERR(e, ...)                   { if(e) { Error_Base::Trace( e, ERR_ALLOCATOR(__VA_ARGS__)); e->Print(); throw e; }};

#define TRACE_ALLOCATOR_EX(e, log_name, ...)                   { if(e) { Error_Base::Trace( e, ERR_ALLOCATOR_EX(log_name, __VA_ARGS__)); }};
#define TRACE_ALLOCATOR_CNT_EX(e, log_name, ...)               { if(e) { Error_Base::Trace( e, ERR_ALLOCATOR_EX(log_name, __VA_ARGS__)); continue; }};
#define TRACE_ALLOCATOR_BRK_EX(e, log_name, ...)               { if(e) { Error_Base::Trace( e, ERR_ALLOCATOR_EX(log_name, __VA_ARGS__)); break; }};
#define TRACE_ALLOCATOR_RET_EX(e, log_name, ...)               { if(e) { Error_Base::Trace( e, ERR_ALLOCATOR_EX(log_name, __VA_ARGS__)); return; }};
#define TRACE_ALLOCATOR_RET_VAL_EX(e, log_name, ret, ...)      { if(e) { Error_Base::Trace( e, ERR_ALLOCATOR_EX(log_name, __VA_ARGS__)); return ret; }};
#define TRACE_ALLOCATOR_RET_ERR_EX(e, log_name, ...)           { if(e) { Error_Base::Trace( e, ERR_ALLOCATOR_EX(log_name, __VA_ARGS__)); return e; }};
#define TRACE_ALLOCATOR_THR_VAL_EX(e, log_name, thr, ...)      { if(e) { Error_Base::Trace( e, ERR_ALLOCATOR_EX(log_name, __VA_ARGS__)); throw thr; }};
#define TRACE_ALLOCATOR_THR_ERR_EX(e, log_name, ...)           { if(e) { Error_Base::Trace( e, ERR_ALLOCATOR_EX(log_name, __VA_ARGS__)); throw e; }};
#define TRACE_ALLOCATOR_PRNT_EX(e, log_name, ...)              { if(e) { Error_Base::Trace( e, ERR_ALLOCATOR_EX(log_name, __VA_ARGS__)); e->Print(); }};
#define TRACE_ALLOCATOR_PRNT_CNT_EX(e, log_name, ...)          { if(e) { Error_Base::Trace( e, ERR_ALLOCATOR_EX(log_name, __VA_ARGS__)); e->Print(); continue; }};
#define TRACE_ALLOCATOR_PRNT_BRK_EX(e, log_name, ...)          { if(e) { Error_Base::Trace( e, ERR_ALLOCATOR_EX(log_name, __VA_ARGS__)); e->Print(); break; }};
#define TRACE_ALLOCATOR_PRNT_RET_EX(e, log_name, ...)          { if(e) { Error_Base::Trace( e, ERR_ALLOCATOR_EX(log_name, __VA_ARGS__)); e->Print(); return; }};
#define TRACE_ALLOCATOR_PRNT_RET_VAL_EX(e, log_name, ret, ...) { if(e) { Error_Base::Trace( e, ERR_ALLOCATOR_EX(log_name, __VA_ARGS__)); e->Print(); return ret; }};
#define TRACE_ALLOCATOR_PRNT_RET_ERR_EX(e, log_name, ...)      { if(e) { Error_Base::Trace( e, ERR_ALLOCATOR_EX(log_name, __VA_ARGS__)); e->Print(); return e; }};
#define TRACE_ALLOCATOR_PRNT_THR_VA_EXL(e, log_name, thr, ...) { if(e) { Error_Base::Trace( e, ERR_ALLOCATOR_EX(log_name, __VA_ARGS__)); e->Print(); throw thr; }};
#define TRACE_ALLOCATOR_PRNT_THR_ERR_EX(e, log_name, ...)      { if(e) { Error_Base::Trace( e, ERR_ALLOCATOR_EX(log_name, __VA_ARGS__)); e->Print(); throw e; }};
