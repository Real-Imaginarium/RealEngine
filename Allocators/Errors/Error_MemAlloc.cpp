#include "Error_MemAlloc.h"
#include "LogError.h"




Error_MemAlloc::Error_MemAlloc( size_t alloc_size_bytes, const char *file_name, const char *func_name, int line, const char *logFile_name )
    : Error_Base( file_name, func_name, line, logFile_name )
    , m_alloc_type( Method::MALLOC )
    , m_meaning_1( alloc_size_bytes )
    , m_meaning_2( 0 )
    , m_mess("")
{}

Error_MemAlloc::Error_MemAlloc( size_t elem_count, size_t elem_size_bytes, const char *file_name, const char *func_name, int line, const char *logFile_name )
    : Error_Base( file_name, func_name, line, logFile_name )
    , m_alloc_type( Method::CALLOC )
    , m_meaning_1( elem_count )
    , m_meaning_2( elem_size_bytes )
    , m_mess( "" )
{}

Error_MemAlloc::Error_MemAlloc( void *memory_block, size_t new_size_bytes, const char *file_name, const char *func_name, int line, const char *logFile_name )
    : Error_Base( file_name, func_name, line, logFile_name )
    , m_alloc_type( Method::REALLOC )
    , m_meaning_1( (size_t)memory_block )
    , m_meaning_2( new_size_bytes )
    , m_mess( "" )
{}

Error_MemAlloc::Error_MemAlloc( size_t alloc_size_bytes, const std::string mess, const char *file_name, const char *func_name, int line, const char *logFile_name )
    : Error_Base( file_name, func_name, line, logFile_name )
    , m_alloc_type( Method::MALLOC )
    , m_meaning_1( alloc_size_bytes )
    , m_meaning_2( 0 )
    , m_mess( mess )
{}

Error_MemAlloc::Error_MemAlloc( size_t elem_count, size_t elem_size_bytes, const std::string mess, const char *file_name, const char *func_name, int line, const char *logFile_name )
    : Error_Base( file_name, func_name, line, logFile_name )
    , m_alloc_type( Method::CALLOC )
    , m_meaning_1( elem_count )
    , m_meaning_2( elem_size_bytes )
    , m_mess( mess )
{}

Error_MemAlloc::Error_MemAlloc( void *memory_block, size_t new_size_bytes, const std::string mess, const char *file_name, const char *func_name, int line, const char *logFile_name )
    : Error_Base( file_name, func_name, line, logFile_name )
    , m_alloc_type( Method::REALLOC )
    , m_meaning_1( (size_t)memory_block )
    , m_meaning_2( new_size_bytes )
    , m_mess( mess )
{}


std::string Error_MemAlloc::Name() const
{
    return "Memory Allocation error";
}


void Error_MemAlloc::PrintSelf() const
{
    Error_Base::PrintSelf();
    std::string concretisation;
    switch (m_alloc_type) {
    case Method::MALLOC:
        concretisation = "Can't allocate " + std::to_string( m_meaning_1 ) + " Bytes of memory using malloc.\n" + m_mess;
        break;
    case Method::CALLOC:
        concretisation = "Can't allocate " + std::to_string( m_meaning_1 ) + " elements with size " + std::to_string( m_meaning_2 ) + " Bytes each one using calloc.\n" + m_mess;
        break;
    case Method::REALLOC:
        concretisation = "Can't re-allocate memory block starting from address: " + std::to_string( m_meaning_2 ) + " to new size: " + std::to_string( m_meaning_1 ) + " Bytes using realloc.\n" + m_mess;
        break;
    case Method::OTHER:
        concretisation = "Undefined memory allocation error";
        break;
    }
    Log::error( m_logFile_name.generic_string() ) << std::string( m_tab_size, ' ' ) << concretisation << Log::endlog{};
    return;
}
