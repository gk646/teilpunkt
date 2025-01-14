#ifndef TPUNKT_STF_TABLE_H
#define TPUNKT_STF_TABLE_H

#include <array>
#include <cstring>
#include "datastructures/FixedString.h"

// Simple Text Format

struct Descriptor
{
    const char key[ 16 ];
    int size;
};

template <Descriptor... Keys>
struct TextParser
{
    static constexpr int keyCount = sizeof...( Keys );
    static constexpr int maxSize = []
    {
        int m = 0;
        for( int s : { Keys.size... } )
        {
            if( s > m )
                m = s;
        }
        return m;
    }();

    std::array<Descriptor, keyCount> keys_ = { Keys... };
    std::array<tpunkt::FixedString<maxSize>, keyCount> values_{};

    bool parse( const char* input )
    {
        auto assignValue = [ & ]( size_t j, const char* val, size_t length ) -> bool
        {
            if( length > static_cast<size_t>( keys_[ j ].size ) )
                return false;
            values_[ j ].assign( val );
            return true;
        };

        size_t start = 0;

        for( int lineIdx = 0; lineIdx < keyCount; ++lineIdx )
        {
            const char* lineStart = input + start;
            const char* newlinePos = std::strchr( lineStart, '\n' );
            if( !newlinePos )
                return false;

            size_t lineLen = newlinePos - lineStart;
            const char* eq = std::strchr( lineStart, '=' );
            if( !eq || eq >= newlinePos )
                return false; // Missing '=' or misplaced

            size_t keyLen = eq - lineStart;
            size_t valueLen = newlinePos - eq - 1;

            const char* keyStr = lineStart;
            const char* valueStr = eq + 1;

            // Match this parsed key among our descriptors
            bool matched = false;
            for( size_t j = 0; j < keyCount; ++j )
            {
                // Compare up to keyLen, and check the descriptor's key ends there
                if( std::strncmp( keys_[ j ].key, keyStr, keyLen ) == 0 && keys_[ j ].key[ keyLen ] == '\0' )
                {
                    if( !assignValue( j, valueStr, valueLen ) )
                        return false;
                    matched = true;
                    break;
                }
            }
            if( !matched )
                return false; // Key not found in descriptors

            // Advance to next line
            start += ( lineLen + 1 );
        }
        return true;
    }

    // Retrieves the stored value for a given key
    const char* getValue( const char* key ) const
    {
        for( size_t i = 0; i < keyCount; ++i )
        {
            // Compare entire key string
            if( std::strcmp( keys_[ i ].key, key ) == 0 )
            {
                return values_[ i ].get();
            }
        }
        return nullptr; // Key not found
    }
};

#endif                  // TPUNKT_STF_TABLE_H