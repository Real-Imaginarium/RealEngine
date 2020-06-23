#pragma once

#include "Error_Base.h"

enum class RegList_ErrType
{
    ERR_EXISTING_REG_INSERTION,         // ѕопытка вставки в RegionsList уже существующего значени€ RegionP<> (т.е. оно уже было вставлено ранее)
    ERR_GRAB_FROM_EMPTY_LIST,           // «апрос региона, когда список пуст
    ERR_CONSISTENT_REG_NOTFOUND,        // –егион подход€щей ширины не найден в S-List
    ERR_REG_WITH_SUCH_START_NOTFOUND,   // –егион с таким .start не найден в P-List
    ERR_OVERLAPPED_REG_INSERTION,       // ѕопытка вставки перекрывающегос€ региона в P-List
};

class Error_RegionsList : public Error_Base
{
public:
    ERRORS_API Error_RegionsList( RegList_ErrType errType, size_t reg_start, size_t reg_size, const char* file_name, const char* func_name, int line, const char* logFile_name = "ErrorLog.txt" );
    ERRORS_API Error_RegionsList( RegList_ErrType errType, size_t reg_size, const char* file_name, const char* func_name, int line, const char* logFile_name = "ErrorLog.txt" );
    ERRORS_API Error_RegionsList( RegList_ErrType errType, const char* file_name, const char* func_name, int line, const char* logFile_name = "ErrorLog.txt" );

    RegList_ErrType Type() const { return m_errType; }
protected:
    virtual std::string Name() const override;

    virtual void PrintSelf() const override;

private:
    RegList_ErrType m_errType;
    size_t m_reg_start;
    size_t m_reg_size;
};