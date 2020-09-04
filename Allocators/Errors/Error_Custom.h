#pragma once

#include "Error_Base.h"


class Error_Custom : public Error_Base
{
public:
    ERRORS_API Error_Custom( const std::string &desc, const char *file, const char *func, int line, const char *logFile );

    ERRORS_API ~Error_Custom() override;

protected:
    virtual std::string Name() const override;

    virtual void PrintSelf() const override;

private:
    std::string m_description;
};


using Error_CustomPtr = std::shared_ptr<Error_Custom>;


#define ERR_CUSTOM(...) { Error_CustomPtr( new Error_Custom( __VA_ARGS__, PLACE(), Error_Base::g_error_log_name)) }
#define ERR_CUSTOM_EX(log_name, ...) { Error_CustomPtr( new Error_Custom( __VA_ARGS__, PLACE(), log_name)) }

#define TRACE_CUSTOM(e, ...)                                { if(e) { Error_Base::Trace( e, ERR_CUSTOM(__VA_ARGS__)); }};
#define TRACE_CUSTOM_CNT(e, ...)                            { if(e) { Error_Base::Trace( e, ERR_CUSTOM(__VA_ARGS__)); continue; }};
#define TRACE_CUSTOM_BRK(e, ...)                            { if(e) { Error_Base::Trace( e, ERR_CUSTOM(__VA_ARGS__)); break; }};
#define TRACE_CUSTOM_RET(e, ...)                            { if(e) { Error_Base::Trace( e, ERR_CUSTOM(__VA_ARGS__)); return; }};
#define TRACE_CUSTOM_RET_VAL(e, ret, ...)                   { if(e) { Error_Base::Trace( e, ERR_CUSTOM(__VA_ARGS__)); return ret; }};
#define TRACE_CUSTOM_RET_ERR(e, ...)                        { if(e) { Error_Base::Trace( e, ERR_CUSTOM(__VA_ARGS__)); return e; }};
#define TRACE_CUSTOM_THR_VAL(e, thr, ...)                   { if(e) { Error_Base::Trace( e, ERR_CUSTOM(__VA_ARGS__)); throw thr; }};
#define TRACE_CUSTOM_THR_ERR(e, ...)                        { if(e) { Error_Base::Trace( e, ERR_CUSTOM(__VA_ARGS__)); throw e; }};
#define TRACE_CUSTOM_PRNT(e, ...)                           { if(e) { Error_Base::Trace( e, ERR_CUSTOM(__VA_ARGS__)); e->Print(); }};
#define TRACE_CUSTOM_PRNT_CNT(e, ...)                       { if(e) { Error_Base::Trace( e, ERR_CUSTOM(__VA_ARGS__)); e->Print(); continue; }};
#define TRACE_CUSTOM_PRNT_BRK(e, ...)                       { if(e) { Error_Base::Trace( e, ERR_CUSTOM(__VA_ARGS__)); e->Print(); break; }};
#define TRACE_CUSTOM_PRNT_RET(e, ...)                       { if(e) { Error_Base::Trace( e, ERR_CUSTOM(__VA_ARGS__)); e->Print(); return; }};
#define TRACE_CUSTOM_PRNT_RET_VAL(e, ret, ...)              { if(e) { Error_Base::Trace( e, ERR_CUSTOM(__VA_ARGS__)); e->Print(); return ret; }};
#define TRACE_CUSTOM_PRNT_RET_ERR(e, ...)                   { if(e) { Error_Base::Trace( e, ERR_CUSTOM(__VA_ARGS__)); e->Print(); return e; }};
#define TRACE_CUSTOM_PRNT_THR_VAL(e, thr, ...)              { if(e) { Error_Base::Trace( e, ERR_CUSTOM(__VA_ARGS__)); e->Print(); throw thr; }};
#define TRACE_CUSTOM_PRNT_THR_ERR(e, ...)                   { if(e) { Error_Base::Trace( e, ERR_CUSTOM(__VA_ARGS__)); e->Print(); throw e; }};

#define TRACE_CUSTOM_EX(e, log_name, ...)                   { if(e) { Error_Base::Trace( e, ERR_CUSTOM_EX(log_name, __VA_ARGS__)); }};
#define TRACE_CUSTOM_CNT_EX(e, log_name, ...)               { if(e) { Error_Base::Trace( e, ERR_CUSTOM_EX(log_name, __VA_ARGS__)); continue; }};
#define TRACE_CUSTOM_BRK_EX(e, log_name, ...)               { if(e) { Error_Base::Trace( e, ERR_CUSTOM_EX(log_name, __VA_ARGS__)); break; }};
#define TRACE_CUSTOM_RET_EX(e, log_name, ...)               { if(e) { Error_Base::Trace( e, ERR_CUSTOM_EX(log_name, __VA_ARGS__)); return; }};
#define TRACE_CUSTOM_RET_VAL_EX(e, log_name, ret, ...)      { if(e) { Error_Base::Trace( e, ERR_CUSTOM_EX(log_name, __VA_ARGS__)); return ret; }};
#define TRACE_CUSTOM_RET_ERR_EX(e, log_name, ...)           { if(e) { Error_Base::Trace( e, ERR_CUSTOM_EX(log_name, __VA_ARGS__)); return e; }};
#define TRACE_CUSTOM_THR_VAL_EX(e, log_name, thr, ...)      { if(e) { Error_Base::Trace( e, ERR_CUSTOM_EX(log_name, __VA_ARGS__)); throw thr; }};
#define TRACE_CUSTOM_THR_ERR_EX(e, log_name, ...)           { if(e) { Error_Base::Trace( e, ERR_CUSTOM_EX(log_name, __VA_ARGS__)); throw e; }};
#define TRACE_CUSTOM_PRNT_EX(e, log_name, ...)              { if(e) { Error_Base::Trace( e, ERR_CUSTOM_EX(log_name, __VA_ARGS__)); e->Print(); }};
#define TRACE_CUSTOM_PRNT_CNT_EX(e, log_name, ...)          { if(e) { Error_Base::Trace( e, ERR_CUSTOM_EX(log_name, __VA_ARGS__)); e->Print(); continue; }};
#define TRACE_CUSTOM_PRNT_BRK_EX(e, log_name, ...)          { if(e) { Error_Base::Trace( e, ERR_CUSTOM_EX(log_name, __VA_ARGS__)); e->Print(); break; }};
#define TRACE_CUSTOM_PRNT_RET_EX(e, log_name, ...)          { if(e) { Error_Base::Trace( e, ERR_CUSTOM_EX(log_name, __VA_ARGS__)); e->Print(); return; }};
#define TRACE_CUSTOM_PRNT_RET_VAL_EX(e, log_name, ret, ...) { if(e) { Error_Base::Trace( e, ERR_CUSTOM_EX(log_name, __VA_ARGS__)); e->Print(); return ret; }};
#define TRACE_CUSTOM_PRNT_RET_ERR_EX(e, log_name, ...)      { if(e) { Error_Base::Trace( e, ERR_CUSTOM_EX(log_name, __VA_ARGS__)); e->Print(); return e; }};
#define TRACE_CUSTOM_PRNT_THR_VA_EXL(e, log_name, thr, ...) { if(e) { Error_Base::Trace( e, ERR_CUSTOM_EX(log_name, __VA_ARGS__)); e->Print(); throw thr; }};
#define TRACE_CUSTOM_PRNT_THR_ERR_EX(e, log_name, ...)      { if(e) { Error_Base::Trace( e, ERR_CUSTOM_EX(log_name, __VA_ARGS__)); e->Print(); throw e; }};