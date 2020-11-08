#pragma once

#include "Error_Base.h"

enum class ERL_Type : uint8_t
{
    NONE,
    EXISTING_REG_INSERTION,         // ������� ������� � RegionsList ��� ������������� �������� RegionP<> (�.�. ��� ��� ���� ��������� �����)
    OVERLAPPED_REG_INSERTION,       // ������� ������� ���������������� ������� � P-List
    GRAB_FROM_EMPTY_LIST,           // ������ �������, ����� ������ ����
    CONSISTENT_REG_NOTFOUND,        // ������ ���������� ������ �� ������ � S-List
    REG_WITH_SUCH_START_NOTFOUND,   // ������ � ����� .start �� ������ � P-List
    SUCH_REGION_NOTFOUND,           // ������ � ����� .start � .size �� ������ � ������
    P_LIST_ALLOCATION,              // ��������� ������ ��� P-List ��������� ������
    S_LIST_ALLOCATION,              // ��������� ������ ��� S-List ��������� ������
};


class Error_RegionsList : public Error_Base
{
public:
    Error_RegionsList( ERL_Type err_type, void *start, size_t size, const char *file, const char *func, int line, const char *logFile = g_error_log_name );
    Error_RegionsList( ERL_Type err_type, void *start, const char *file, const char *func, int line, const char *logFile = g_error_log_name );
    Error_RegionsList( ERL_Type err_type, size_t size, const char *file, const char *func, int line, const char *logFile = g_error_log_name );
    Error_RegionsList( ERL_Type err_type, const char *file, const char *func, int line, const char *logFile = g_error_log_name );

    ERL_Type Elem() const { return m_errType; }

protected:
    virtual std::string Name() const override;

    virtual void PrintSelf() const override;

private:
    ERL_Type m_errType;
    size_t m_reg_start;
    size_t m_reg_size;
};


using Error_RegionsListPtr = std::shared_ptr<Error_RegionsList>;


#define ERR_REGIONSLIST(...)              { Error_RegionsListPtr( new Error_RegionsList( __VA_ARGS__, PLACE(), Error_Base::g_error_log_name)) }
#define ERR_REGIONSLIST_EX(log_name, ...) { Error_RegionsListPtr( new Error_RegionsList( __VA_ARGS__, PLACE(), log_name)) }

#define TRACE_REGIONSLIST(e, ...)                                { if(e) { Error_Base::Trace( e, ERR_REGIONSLIST(__VA_ARGS__)); }};
#define TRACE_REGIONSLIST_CNT(e, ...)                            { if(e) { Error_Base::Trace( e, ERR_REGIONSLIST(__VA_ARGS__)); continue; }};
#define TRACE_REGIONSLIST_BRK(e, ...)                            { if(e) { Error_Base::Trace( e, ERR_REGIONSLIST(__VA_ARGS__)); break; }};
#define TRACE_REGIONSLIST_RET(e, ...)                            { if(e) { Error_Base::Trace( e, ERR_REGIONSLIST(__VA_ARGS__)); return; }};
#define TRACE_REGIONSLIST_RET_VAL(e, ret, ...)                   { if(e) { Error_Base::Trace( e, ERR_REGIONSLIST(__VA_ARGS__)); return ret; }};
#define TRACE_REGIONSLIST_RET_ERR(e, ...)                        { if(e) { Error_Base::Trace( e, ERR_REGIONSLIST(__VA_ARGS__)); return e; }};
#define TRACE_REGIONSLIST_THR_VAL(e, thr, ...)                   { if(e) { Error_Base::Trace( e, ERR_REGIONSLIST(__VA_ARGS__)); throw thr; }};
#define TRACE_REGIONSLIST_THR_ERR(e, ...)                        { if(e) { Error_Base::Trace( e, ERR_REGIONSLIST(__VA_ARGS__)); throw e; }};
#define TRACE_REGIONSLIST_PRNT(e, ...)                           { if(e) { Error_Base::Trace( e, ERR_REGIONSLIST(__VA_ARGS__)); e->Print(); }};
#define TRACE_REGIONSLIST_PRNT_CNT(e, ...)                       { if(e) { Error_Base::Trace( e, ERR_REGIONSLIST(__VA_ARGS__)); e->Print(); continue; }};
#define TRACE_REGIONSLIST_PRNT_BRK(e, ...)                       { if(e) { Error_Base::Trace( e, ERR_REGIONSLIST(__VA_ARGS__)); e->Print(); break; }};
#define TRACE_REGIONSLIST_PRNT_RET(e, ...)                       { if(e) { Error_Base::Trace( e, ERR_REGIONSLIST(__VA_ARGS__)); e->Print(); return; }};
#define TRACE_REGIONSLIST_PRNT_RET_VAL(e, ret, ...)              { if(e) { Error_Base::Trace( e, ERR_REGIONSLIST(__VA_ARGS__)); e->Print(); return ret; }};
#define TRACE_REGIONSLIST_PRNT_RET_ERR(e, ...)                   { if(e) { Error_Base::Trace( e, ERR_REGIONSLIST(__VA_ARGS__)); e->Print(); return e; }};
#define TRACE_REGIONSLIST_PRNT_THR_VAL(e, thr, ...)              { if(e) { Error_Base::Trace( e, ERR_REGIONSLIST(__VA_ARGS__)); e->Print(); throw thr; }};
#define TRACE_REGIONSLIST_PRNT_THR_ERR(e, ...)                   { if(e) { Error_Base::Trace( e, ERR_REGIONSLIST(__VA_ARGS__)); e->Print(); throw e; }};

#define TRACE_REGIONSLIST_EX(e, log_name, ...)                   { if(e) { Error_Base::Trace( e, ERR_REGIONSLIST_EX(log_name, __VA_ARGS__)); }};
#define TRACE_REGIONSLIST_CNT_EX(e, log_name, ...)               { if(e) { Error_Base::Trace( e, ERR_REGIONSLIST_EX(log_name, __VA_ARGS__)); continue; }};
#define TRACE_REGIONSLIST_BRK_EX(e, log_name, ...)               { if(e) { Error_Base::Trace( e, ERR_REGIONSLIST_EX(log_name, __VA_ARGS__)); break; }};
#define TRACE_REGIONSLIST_RET_EX(e, log_name, ...)               { if(e) { Error_Base::Trace( e, ERR_REGIONSLIST_EX(log_name, __VA_ARGS__)); return; }};
#define TRACE_REGIONSLIST_RET_VAL_EX(e, log_name, ret, ...)      { if(e) { Error_Base::Trace( e, ERR_REGIONSLIST_EX(log_name, __VA_ARGS__)); return ret; }};
#define TRACE_REGIONSLIST_RET_ERR_EX(e, log_name, ...)           { if(e) { Error_Base::Trace( e, ERR_REGIONSLIST_EX(log_name, __VA_ARGS__)); return e; }};
#define TRACE_REGIONSLIST_THR_VAL_EX(e, log_name, thr, ...)      { if(e) { Error_Base::Trace( e, ERR_REGIONSLIST_EX(log_name, __VA_ARGS__)); throw thr; }};
#define TRACE_REGIONSLIST_THR_ERR_EX(e, log_name, ...)           { if(e) { Error_Base::Trace( e, ERR_REGIONSLIST_EX(log_name, __VA_ARGS__)); throw e; }};
#define TRACE_REGIONSLIST_PRNT_EX(e, log_name, ...)              { if(e) { Error_Base::Trace( e, ERR_REGIONSLIST_EX(log_name, __VA_ARGS__)); e->Print(); }};
#define TRACE_REGIONSLIST_PRNT_CNT_EX(e, log_name, ...)          { if(e) { Error_Base::Trace( e, ERR_REGIONSLIST_EX(log_name, __VA_ARGS__)); e->Print(); continue; }};
#define TRACE_REGIONSLIST_PRNT_BRK_EX(e, log_name, ...)          { if(e) { Error_Base::Trace( e, ERR_REGIONSLIST_EX(log_name, __VA_ARGS__)); e->Print(); break; }};
#define TRACE_REGIONSLIST_PRNT_RET_EX(e, log_name, ...)          { if(e) { Error_Base::Trace( e, ERR_REGIONSLIST_EX(log_name, __VA_ARGS__)); e->Print(); return; }};
#define TRACE_REGIONSLIST_PRNT_RET_VAL_EX(e, log_name, ret, ...) { if(e) { Error_Base::Trace( e, ERR_REGIONSLIST_EX(log_name, __VA_ARGS__)); e->Print(); return ret; }};
#define TRACE_REGIONSLIST_PRNT_RET_ERR_EX(e, log_name, ...)      { if(e) { Error_Base::Trace( e, ERR_REGIONSLIST_EX(log_name, __VA_ARGS__)); e->Print(); return e; }};
#define TRACE_REGIONSLIST_PRNT_THR_VA_EXL(e, log_name, thr, ...) { if(e) { Error_Base::Trace( e, ERR_REGIONSLIST_EX(log_name, __VA_ARGS__)); e->Print(); throw thr; }};
#define TRACE_REGIONSLIST_PRNT_THR_ERR_EX(e, log_name, ...)      { if(e) { Error_Base::Trace( e, ERR_REGIONSLIST_EX(log_name, __VA_ARGS__)); e->Print(); throw e; }};
