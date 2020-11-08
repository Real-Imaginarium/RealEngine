#pragma once

#include <string>
#include <vector>

struct Type_1
{
    char f1[2];
    uint16_t f2;
    float f3;
    Type_1() {
        f1[0] = 'f';
        f1[1] = 'a';
        f2 = 5226;
        f3 = 0.123f;
    }
    bool operator==( const Type_1 &other ) { return f1[0] == other.f1[0] && f1[1] == other.f1[1] && f2 == other.f2 && f3 == other.f3; }
};

struct Type_2
{
    double f1;
    std::string f2;
    Type_2() {
        f1 = 0.456;
        f2 = "Mein Herz Brennt!";
    }
    bool operator==( const Type_2 &other ) { return f1 == other.f1 && f2 == other.f2; }
};

struct Type_3
{
    const char *f1;
    uint64_t f2;
    char f3;
    int16_t f4;
    Type_3() {
        f1 = "Links 2 3 4";
        f2 = 0;
        f3 = 's';
        f4 = 1500;
    }
    bool operator==( const Type_3 &other ) { return !strcmp(f1, other.f1) && f2 == other.f2 && f3 == other.f3 && f4 == other.f4; }
};

struct Type_4
{
    bool f1;
    double f2[3];
    std::string f3;
    Type_4() {
        f1 = true;
        f2[0] = 0.789;
        f2[1] = 0.123;
        f2[2] = 0.456;
        f3 = "Feuer Frei!";
    }
    bool operator==( const Type_4 &other ) { return f1 == other.f1 && f2[0] == other.f2[0] && f2[1] == other.f2[1] && f2[2] == other.f2[2] && f3 == other.f3; }
};

struct Type_5
{
    std::vector<int32_t> f1;
    bool f2;
    Type_5() {
        f1 = {1, 2, -3, 4, 5, -6};
        f2 = true;
    }
    bool operator==( const Type_5 &other ) { return f1 == other.f1 && f2 == other.f2; }
};

struct Type_6
{
    char f1;
    char f2;
    float f3;
    uint16_t f4;
    const char *f5;
    Type_6() {
        f1 = 'z';
        f2 = 'j';
        f3 = 0.789f;
        f4 = 166;
        f5 = "Ich Will";
    }
    bool operator==( const Type_6 &other ) { return f1 == other.f1 && f2 == other.f2 && f3 == other.f3 && f4 == other.f4 && !strcmp(f5, other.f5); }
};


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
        desc = std::string( other.desc );
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

    std::string Desc() const { return desc; }

    bool operator==( const Descriptor &other ) { return desc == other.desc; }

private:
    int CPU;            // Handler CPU
    int GPU;            // Handler GPU
    std::string desc;
};


static RegionsBasedAllocator<uint8_t> alloc( 1000000000 );
class SomeObject
{
public:
    SomeObject() : f1( 0x0102030405060708 ) {}
    ~SomeObject()
    {
        size_t a = 7;
    }

    void *operator new( size_t size )
    {
        return ::new( (void *)alloc.Allocate( size ) ) SomeObject();
    }
    void *operator new[]( size_t size )
    {
        return ::operator new[]( size, (void *)alloc.Allocate( size ) );
    }
    void operator delete( void *start )
    {
        alloc.Deallocate( start, sizeof( SomeObject ) );
    }
    void operator delete[]( void *start )
    {
        alloc.Deallocate( start, *(size_t *)start * sizeof( SomeObject ) + sizeof( size_t ) );
    }
private:
    uint64_t f1;
};