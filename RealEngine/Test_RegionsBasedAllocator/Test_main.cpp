#include "Vector.hpp"
#include "RegionsBasedAllocator.hpp"

class Descriptor
{
public:
    Descriptor( std::string _desc )
    {
        desc = _desc;
        if( desc == "1" ) { CPU = GPU = 1; }
        else if( desc == "2" ) { CPU = GPU = 2; }
        else if( desc == "3" ) { CPU = GPU = 3; }
        else { CPU = GPU = -1; }
    }
    ~Descriptor() {
        desc.clear();
    }
    Descriptor( const Descriptor &other )
    {
        desc = other.desc;
        if( desc == "1" ) { CPU = GPU = 1; }
        else if( desc == "2" ) { CPU = GPU = 2; }
        else if( desc == "3" ) { CPU = GPU = 3; }
        else { CPU = GPU = -1; }
    }
    Descriptor( const Descriptor &&other ) noexcept
    {
        desc = other.desc;
        if( desc == "1" ) { CPU = GPU = 1; }
        else if( desc == "2" ) { CPU = GPU = 2; }
        else if( desc == "3" ) { CPU = GPU = 3; }
        else { CPU = GPU = -1; }
    }
    Descriptor &operator=( const Descriptor &other )
    {
        desc = std::string(other.desc);
        if( desc == "1" ) { CPU = GPU = 1; }
        else if( desc == "2" ) { CPU = GPU = 2; }
        else if( desc == "3" ) { CPU = GPU = 3; }
        else { CPU = GPU = -1; }
        return *this;
    }
    Descriptor &operator=( Descriptor &&other ) noexcept
    {
        desc = std::string( other.desc );
        if( desc == "1" ) { CPU = GPU = 1; }
        else if( desc == "2" ) { CPU = GPU = 2; }
        else if( desc == "3" ) { CPU = GPU = 3; }
        else { CPU = GPU = -1; }
        return *this;
    }

private:
    int CPU;            // Handler CPU
    int GPU;            // Handler GPU
    std::string desc;
};


int main()
{
    const size_t mem_size = 100;
    Descriptor *mem = (Descriptor*)calloc( mem_size, sizeof( Descriptor ) );
    uint8_t *mem2 = (uint8_t*)calloc( mem_size, sizeof( std::string ) );

    Descriptor* arr[100];
    for( size_t i = 0; i < 100; ++i ) {
        arr[i] = mem + i;
    }
    try {
        std::shared_ptr<RegionsBasedAllocator<Descriptor>> RB_Alloc( new RegionsBasedAllocator<Descriptor>( mem, mem_size ) );
        std::shared_ptr<Vector<Descriptor>> vec(new Vector<Descriptor>( 2, RB_Alloc));

        vec->EmplaceBack( "1" );
        vec->EmplaceBack( "2" );
        vec->EmplaceBack( "3" );
        vec->EmplaceBack( "4" );
        vec->EmplaceBack( "5" );
        vec->EmplaceBack( "6" );
        vec->EmplaceBack( "7" );
        vec->EmplaceBack( "8" );
        vec->EmplaceBack( "9" );
        vec->EmplaceBack( "0" );

    }
    catch( Error_BasePtr e ) {
        TRACE_CUSTOM_PRNT_RET_VAL( e, -1, "Something went wrong" );
    }


    return 0;
}