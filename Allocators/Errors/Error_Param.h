#pragma once

#include "Error_Base.h"


class Error_Param : public Error_Base
{
public:
    ERRORS_API Error_Param( int arg_pos, std::string arg_name, std::string expected, std::string gained, const char* file, const char* func, int line, const char* logFile = g_error_log_name );

protected:
    virtual std::string Name() const override;

    virtual void PrintSelf() const override;

private:
    int m_arg_number;
    std::string m_arg_name;
    std::string m_expected;
    std::string m_gained;
};


using Error_ParamPtr = std::shared_ptr<Error_Param>;


#define ERR_PARAM(...)              { Error_ParamPtr( new Error_Param( __VA_ARGS__, PLACE(), Error_Base::g_error_log_name)) }
#define ERR_PARAM_EX(log_name, ...) { Error_ParamPtr( new Error_Param( __VA_ARGS__, PLACE(), log_name)) }

#define TRACE_PARAM(e, ...)                                { if(e) { Error_Base::Trace( e, ERR_PARAM(__VA_ARGS__)); }};
#define TRACE_PARAM_CNT(e, ...)                            { if(e) { Error_Base::Trace( e, ERR_PARAM(__VA_ARGS__)); continue; }};
#define TRACE_PARAM_BRK(e, ...)                            { if(e) { Error_Base::Trace( e, ERR_PARAM(__VA_ARGS__)); break; }};
#define TRACE_PARAM_RET(e, ...)                            { if(e) { Error_Base::Trace( e, ERR_PARAM(__VA_ARGS__)); return; }};
#define TRACE_PARAM_RET_VAL(e, ret, ...)                   { if(e) { Error_Base::Trace( e, ERR_PARAM(__VA_ARGS__)); return ret; }};
#define TRACE_PARAM_RET_ERR(e, ...)                        { if(e) { Error_Base::Trace( e, ERR_PARAM(__VA_ARGS__)); return e; }};
#define TRACE_PARAM_THR_VAL(e, thr, ...)                   { if(e) { Error_Base::Trace( e, ERR_PARAM(__VA_ARGS__)); throw thr; }};
#define TRACE_PARAM_THR_ERR(e, ...)                        { if(e) { Error_Base::Trace( e, ERR_PARAM(__VA_ARGS__)); throw e; }};
#define TRACE_PARAM_PRNT(e, ...)                           { if(e) { Error_Base::Trace( e, ERR_PARAM(__VA_ARGS__)); e->Print(); }};
#define TRACE_PARAM_PRNT_CNT(e, ...)                       { if(e) { Error_Base::Trace( e, ERR_PARAM(__VA_ARGS__)); e->Print(); continue; }};
#define TRACE_PARAM_PRNT_BRK(e, ...)                       { if(e) { Error_Base::Trace( e, ERR_PARAM(__VA_ARGS__)); e->Print(); break; }};
#define TRACE_PARAM_PRNT_RET(e, ...)                       { if(e) { Error_Base::Trace( e, ERR_PARAM(__VA_ARGS__)); e->Print(); return; }};
#define TRACE_PARAM_PRNT_RET_VAL(e, ret, ...)              { if(e) { Error_Base::Trace( e, ERR_PARAM(__VA_ARGS__)); e->Print(); return ret; }};
#define TRACE_PARAM_PRNT_RET_ERR(e, ...)                   { if(e) { Error_Base::Trace( e, ERR_PARAM(__VA_ARGS__)); e->Print(); return e; }};
#define TRACE_PARAM_PRNT_THR_VAL(e, thr, ...)              { if(e) { Error_Base::Trace( e, ERR_PARAM(__VA_ARGS__)); e->Print(); throw thr; }};
#define TRACE_PARAM_PRNT_THR_ERR(e, ...)                   { if(e) { Error_Base::Trace( e, ERR_PARAM(__VA_ARGS__)); e->Print(); throw e; }};

#define TRACE_PARAM_EX(e, log_name, ...)                   { if(e) { Error_Base::Trace( e, ERR_PARAM_EX(log_name, __VA_ARGS__)); }};
#define TRACE_PARAM_CNT_EX(e, log_name, ...)               { if(e) { Error_Base::Trace( e, ERR_PARAM_EX(log_name, __VA_ARGS__)); continue; }};
#define TRACE_PARAM_BRK_EX(e, log_name, ...)               { if(e) { Error_Base::Trace( e, ERR_PARAM_EX(log_name, __VA_ARGS__)); break; }};
#define TRACE_PARAM_RET_EX(e, log_name, ...)               { if(e) { Error_Base::Trace( e, ERR_PARAM_EX(log_name, __VA_ARGS__)); return; }};
#define TRACE_PARAM_RET_VAL_EX(e, log_name, ret, ...)      { if(e) { Error_Base::Trace( e, ERR_PARAM_EX(log_name, __VA_ARGS__)); return ret; }};
#define TRACE_PARAM_RET_ERR_EX(e, log_name, ...)           { if(e) { Error_Base::Trace( e, ERR_PARAM_EX(log_name, __VA_ARGS__)); return e; }};
#define TRACE_PARAM_THR_VAL_EX(e, log_name, thr, ...)      { if(e) { Error_Base::Trace( e, ERR_PARAM_EX(log_name, __VA_ARGS__)); throw thr; }};
#define TRACE_PARAM_THR_ERR_EX(e, log_name, ...)           { if(e) { Error_Base::Trace( e, ERR_PARAM_EX(log_name, __VA_ARGS__)); throw e; }};
#define TRACE_PARAM_PRNT_EX(e, log_name, ...)              { if(e) { Error_Base::Trace( e, ERR_PARAM_EX(log_name, __VA_ARGS__)); e->Print(); }};
#define TRACE_PARAM_PRNT_CNT_EX(e, log_name, ...)          { if(e) { Error_Base::Trace( e, ERR_PARAM_EX(log_name, __VA_ARGS__)); e->Print(); continue; }};
#define TRACE_PARAM_PRNT_BRK_EX(e, log_name, ...)          { if(e) { Error_Base::Trace( e, ERR_PARAM_EX(log_name, __VA_ARGS__)); e->Print(); break; }};
#define TRACE_PARAM_PRNT_RET_EX(e, log_name, ...)          { if(e) { Error_Base::Trace( e, ERR_PARAM_EX(log_name, __VA_ARGS__)); e->Print(); return; }};
#define TRACE_PARAM_PRNT_RET_VAL_EX(e, log_name, ret, ...) { if(e) { Error_Base::Trace( e, ERR_PARAM_EX(log_name, __VA_ARGS__)); e->Print(); return ret; }};
#define TRACE_PARAM_PRNT_RET_ERR_EX(e, log_name, ...)      { if(e) { Error_Base::Trace( e, ERR_PARAM_EX(log_name, __VA_ARGS__)); e->Print(); return e; }};
#define TRACE_PARAM_PRNT_THR_VA_EXL(e, log_name, thr, ...) { if(e) { Error_Base::Trace( e, ERR_PARAM_EX(log_name, __VA_ARGS__)); e->Print(); throw thr; }};
#define TRACE_PARAM_PRNT_THR_ERR_EX(e, log_name, ...)      { if(e) { Error_Base::Trace( e, ERR_PARAM_EX(log_name, __VA_ARGS__)); e->Print(); throw e; }};
