// Supported with union (c) 2018-2021 Union team

#ifndef __ZSTRING_H__VER1__
#define __ZSTRING_H__VER1__

#define USING_UNION_STRING_METHODS True
#define UNPROTECT_ZSTRING_METHODS  True

namespace Gothic_I_Addon {

  inline void CreateDirectories( CStringA path ) {
    CStringA sub_path = path.GetPattern( "", "\\", -1 );
    if( !sub_path.IsEmpty() )
      CreateDirectories( sub_path );
    CreateDirectoryA( path, 0 );
  }

  enum zTSTR_KIND {
    zSTR_ONLY,
    zSTR_BEFORE,
    zSTR_AFTER,
    zSTR_TO,
    zSTR_FROM
  };

  enum zTSTR_ALIGN {
    zSTR_RIGHT,
    zSTR_CENTER,
    zSTR_LEFT
  };

  class zSTRING {
    int   allocater;
    char* vector;
    int   length;
    int   reserved;
  public:
    zMEMPOOL_DECLARATION( zSTRING, 0x008AE6D8 )

    // Constructors
    void zSTRING_OnInit( char const* )    zCall( 0x004010C0 );
    void zSTRING_OnInit()                 zCall( 0x00402F90 );
    void zSTRING_OnInit( zSTRING const& ) zCall( 0x00417CA0 );
    void zSTRING_OnInit( int )            zCall( 0x0046D520 );
    void zSTRING_OnInit( long )           zCall( 0x0046D620 );
    void zSTRING_OnInit( unsigned long )  zCall( 0x0046D720 );
    void zSTRING_OnInit( float, int )     zCall( 0x0046D820 );
    void zSTRING_OnInit( double, int )    zCall( 0x0046D970 );
    void zSTRING_OnInit( char )           zCall( 0x0051FA10 );
    void zSTRING_OnInit( zSTRING const* ) zCall( 0x00722540 );
    zSTRING( char const* vec )            zInit( zSTRING_OnInit( vec ));
    zSTRING()                             zInit( zSTRING_OnInit() );
    zSTRING( zSTRING const& val )         zInit( zSTRING_OnInit( val ));
    zSTRING( int val )                    zInit( zSTRING_OnInit( val ));
    zSTRING( long val )                   zInit( zSTRING_OnInit( val ));
    zSTRING( unsigned long val )          zInit( zSTRING_OnInit( val ));
    zSTRING( float val, int rad = 3 )     zInit( zSTRING_OnInit( val, rad ));
    zSTRING( double val, int rad = 5 )    zInit( zSTRING_OnInit( val, rad ));
    zSTRING( char val )                   zInit( zSTRING_OnInit( val ));
    zSTRING( zSTRING const* val )         zInit( zSTRING_OnInit( val ));
    virtual ~zSTRING()                    zCall( 0x00401160 );
    
    // Special Union CStringA constructor
    zSTRING( CStringA const& val ) { zSTRING_OnInit( (char const*)val ); }

    // Same as union methods
    char& operator[]( unsigned int )        zCall( 0x00447300 );
    zSTRING& operator += ( char const* )    zCall( 0x0066C7D0 );
    zSTRING& operator =  ( char const* )    zCall( 0x004D5310 );
    zSTRING& operator =  ( zSTRING const& ) zCall( 0x005988F0 );
    char* ToChar() const                    zCall( 0x00465C00 );
    void Clear()                            zCall( 0x00598A50 );
    bool IsEmpty() const                    zCall( 0x0077F500 );
    zSTRING& Lower()                        zCall( 0x0046CF70 );
    zSTRING& Upper()                        zCall( 0x0046D090 );
    zSTRING& UpperFirstLowerRest()          zCall( 0x0046D1B0 );

    // Default type cast
    inline operator CStringA()             { return ToChar(); }
    inline operator const CStringA() const { return ToChar(); }
    inline operator const char*() const  { return ToChar(); }
    inline int Length() const            { return length;   }

#if USING_UNION_STRING_METHODS
#if !UNPROTECT_ZSTRING_METHODS
  protected:
#endif
#endif
    // Own methods
    zSTRING& Align( zTSTR_ALIGN, int, char )                                      zCall( 0x0046D300 );
    zSTRING PickWord( unsigned int, zSTRING const&, zSTRING const& ) const        zCall( 0x0046DAC0 );
    char const* PickWordPos( unsigned int, zSTRING const&, zSTRING const& ) const zCall( 0x0046DDB0 );
    zSTRING PickWord_Old( int, zSTRING const& ) const                             zCall( 0x0046DF30 );
    int Insert( unsigned int, zSTRING const& )                                    zCall( 0x0046E010 );
    zSTRING Inserted( unsigned int, zSTRING const& ) const                        zCall( 0x0046E140 );
    int Overwrite( unsigned int, zSTRING const& )                                 zCall( 0x0046E2E0 );
    zSTRING Overwritten( unsigned int, zSTRING const& ) const                     zCall( 0x0046E4E0 );
    int DeleteRight( unsigned int )                                               zCall( 0x0046E600 );
    int Delete( unsigned int, unsigned long )                                     zCall( 0x0046E710 );
    zSTRING Deleted( unsigned int, unsigned long ) const                          zCall( 0x0046E820 );
    int Delete( zSTRING const&, zTSTR_KIND )                                      zCall( 0x0046E980 );
    zSTRING Deleted( zSTRING const&, zTSTR_KIND ) const                           zCall( 0x0046EB60 );
    zSTRING Copied( unsigned int, unsigned long ) const                           zCall( 0x0046EE10 );
    int Copy( zSTRING const&, zTSTR_KIND const& )                                 zCall( 0x0046EFA0 );
    int Copy( unsigned int, unsigned long )                                       zCall( 0x0046EC60 );
    zSTRING Copied( zSTRING const&, zTSTR_KIND const& ) const                     zCall( 0x0046F260 );
    void TrimLeft( char )                                                         zCall( 0x0046F360 );
    void TrimRight( char )                                                        zCall( 0x0046F4A0 );
    int Search( int, char const*, unsigned int ) const                            zCall( 0x0046F650 );
    int SearchRev( zSTRING const&, unsigned int ) const                           zCall( 0x0046F760 );
    bool __cdecl Sprintf( char const*, ... )                                      zCall( 0x0046F9F0 );
    int Search( zSTRING const&, unsigned int ) const                              zCall( 0x00495310 );
    long ToInt() const                                                            zCall( 0x00598B20 );
    float ToFloat() const                                                         zCall( 0x00598B40 );
    int Search( char const*, unsigned int ) const                                 zCall( 0x00598B60 );
    void Init()                                                                   zCall( 0x00629900 );

  public:
#if USING_UNION_STRING_METHODS

#pragma region Union CStringA methods
#define INLINE inline
#define NOINLINE __declspec( noinline )

#pragma region Get
    INLINE const char& operator[]( unsigned int index ) const {
      return vector[index];
    }

    INLINE char& First() {
      return ((CStringA&)*this).First();
    }

    INLINE const char& First() const {
      return ((CStringA&)*this).First();
    }

    INLINE char& Last() {
      return ((CStringA&)*this).Last();
    }

    INLINE const char& Last() const {
      return ((CStringA&)*this).Last();
    }

    INLINE uint32 ToInt32( const uint32& fromRad = 10 ) const {
      return ((CStringA&)*this).ToInt32( fromRad );
    }

    INLINE uint64 ToInt64( const uint32& fromRad = 10 ) const {
      return ((CStringA&)*this).ToInt64( fromRad );
    }

    INLINE real32 ToReal32() const {
      return ((CStringA&)*this).ToReal32();
    }

    INLINE real64 ToReal64() const {
      return ((CStringA&)*this).ToReal64();
    }

    INLINE bool_t Compare( const zSTRING& str ) const {
      return ((CStringA&)*this).Compare( str.ToChar() );
    }

    INLINE bool_t Compare( const char* vec ) const {
      return ((CStringA&)*this).Compare( vec );
    }

    // case Insensitive
    INLINE bool_t CompareI( const zSTRING& str ) const {
      return ((CStringA&)*this).CompareI( str.ToChar() );
    }

    // case Insensitive
    INLINE bool_t CompareI( const char* vec ) const {
      return ((CStringA&)*this).CompareI( vec );
    }

    INLINE bool_t CompareMasked( const CStringA& str ) const {
      return ((CStringA&)*this).CompareMasked( (CStringA&)str );
    }

    INLINE bool_t CompareMaskedI( const CStringA& str ) const {
      return ((CStringA&)*this).CompareMaskedI( (CStringA&)str );
    }

    INLINE bool_t Contains(const zSTRING& cmp) const {
        return this->HasWord(cmp);
    }

    INLINE bool_t HasWord( const zSTRING& cmp ) const {
      return ((CStringA&)*this).HasWord( cmp );
    }

    // case Insensitive
    INLINE bool_t HasWordI( const zSTRING& cmp ) const {
      return ((CStringA&)*this).HasWordI( cmp );
    }

    INLINE bool_t MatchesMask( const zSTRING& mask ) const {
      return ((CStringA&)*this).MatchesMask( mask );
    }

    INLINE bool_t StartWith( const zSTRING& cmp ) const {
      return ((CStringA&)*this).StartWith( (CStringA&)cmp );
    }

    INLINE bool_t EndWith( const zSTRING& cmp ) const {
      return ((CStringA&)*this).EndWith( (CStringA&)cmp );
    }

    INLINE uint32 Search(
      const zSTRING& cmp,
      const uint32&  pos,
      const bool_t&  end
      ) const {
      return ((CStringA&)*this).Search( cmp,
                                        pos,
                                        end
                                        );
    }

    // case Insensitive
    INLINE uint32 SearchI(
      const zSTRING& cmp,
      const uint32&  pos,
      const bool_t&  end
      ) const {
      return ((CStringA&)*this).Search( cmp,
                                        pos,
                                        end
                                        );
    }

    INLINE uint32 SearchReverse(
      const zSTRING& cmp,
      const uint32&  pos = 0,
      const bool_t&  end = False
      ) const {
      return ((CStringA&)*this).SearchReverse( cmp,
                                               pos,
                                               end
                                               );
    }

    // case Insensitive
    INLINE uint32 SearchReverseI(
      const zSTRING& cmp,
      const uint32&  pos = 0,
      const bool_t&  end = False
      ) const {
      return ((CStringA&)*this).SearchReverseI( cmp,
                                                pos,
                                                end
                                                );
    }

    INLINE zSTRING GetWord(
      const zSTRING& mask = " \t",
      const int32&   num = 1
      ) const {
      return (zSTRING)((CStringA&)*this).GetWord( mask,
                                                  num
                                                  );
    }

    INLINE zSTRING GetWordEx(
      const zSTRING& mask = " \t",
      const int32&   num = 1,
      const uint32&  offset = 0,
      uint32*        idx = Null
      ) const {
      return (zSTRING)((CStringA&)*this).GetWordEx( mask,
                                                    num,
                                                    offset,
                                                    idx
                                                    );
    }

    INLINE zSTRING GetPattern(
      const zSTRING& lmask,
      const zSTRING& rmask,
      const int32&   num = 1
      ) const {
      return (zSTRING)((CStringA&)*this).GetPattern( lmask,
                                                     rmask,
                                                     num
                                                     );
    }

    INLINE zSTRING GetPatternEx(
      const zSTRING& lmask,
      const zSTRING& rmask,
      const int32&   num = 1,
      const uint32&  offset = 0,
      uint32*        idx = Null
      ) const {
      return (zSTRING)((CStringA&)*this).GetPatternEx( lmask,
                                                       rmask,
                                                       num,
                                                       offset,
                                                       idx
                                                       );
    }

    INLINE zSTRING GetWordSmart( const int& word_index = 1, const bool& groupSymbols = false ) const {
      return (zSTRING)((CStringA&)*this).GetWordSmart( word_index,
                                                       groupSymbols
                                                       );
    }

    INLINE zSTRING GetWordSmartEx( const int& word_index = 1, const bool& groupSymbols = false, const uint& offset = 0, uint* word_offset = Null ) const {
      return (zSTRING)((CStringA&)*this).GetWordSmartEx( word_index,
                                                         groupSymbols,
                                                         offset,
                                                         word_offset
                                                         );
    }

    INLINE zSTRING GetSymbol(
      const zSTRING* separators,
      const uint32&  sep_num,
      const uint32&  word_index   = 1,
      uint32*        inline_index = Null
      ) const {
      return (zSTRING)((CStringA&)*this).GetSymbol( (const CStringA*)separators,
                                                    sep_num,
                                                    word_index,
                                                    inline_index
                                                    );
    }

    INLINE zSTRING GetSymbolEx(
      const zSTRING*  separators,
      const uint32&   sep_num,
      const zSTRING** brackets,
      const uint32&   br_num,
      const uint32&   word_index   = 1,
      uint32*         inline_index = Null
      ) const {
      return (zSTRING)((CStringA&)*this).GetSymbolEx( (const CStringA*)separators,
                                                      sep_num,
                                                      (const CStringA**)brackets,
                                                      br_num,
                                                      word_index,
                                                      inline_index
                                                      );
    }

    INLINE zSTRING Copy(
      const uint& index,
      const uint& length ) const {
      return ((CStringA&)*this).Copy( index, length );
    }
#pragma endregion

#pragma region Set
    INLINE zSTRING& Reverse() {
      return (zSTRING&)((CStringA&)*this).Reverse();
    }

    INLINE zSTRING& Cut( const uint32& len ) {
      this->Delete( 0, len );
      return *this;
    }

    INLINE zSTRING& Cut( const uint32&   idx, const uint32& len ) {
      this->Delete( idx, len );
      return *this;
    }

    INLINE zSTRING& Put( const zSTRING& str, const uint32& idx ) {
      this->Insert( idx, str );
      return *this;
    }

    INLINE zSTRING& Put( const char* vec, const uint32& idx ) {
      this->Insert( idx, vec );
      return *this;
    }

    NOINLINE zSTRING& Replace( const zSTRING& ptr1, const zSTRING& ptr2, bool looped = false ) {
      bool FirstIteration = true;
      uint Index = 0;
      while( true ) {
        uint Length = looped ? Index :
          FirstIteration ? 0 : Index + ptr2.Length();

        Index = Search( ptr1, Length, False );
        if( Index == Invalid )
          break;

        FirstIteration = false;
        Cut( Index, ptr1.Length() );
        Put( ptr2, Index );
      }
      return *this;
    }

    NOINLINE bool_t WriteToFile( const zSTRING& fname, const bool_t& createDirs = True ) const {
      if( createDirs )
        CreateDirectories( fname.GetPattern( "", "\\", -1 ) );

      FILE* hFile = fopen( fname, "w+b" );
      if( !hFile )
        return False;
      _setmode( _fileno( hFile ), 0x8000 );

      bool_t bWrite = (fwrite( vector, sizeof( char ), length, hFile ) > 0);
      fclose( hFile );
      return bWrite;
    }

    NOINLINE bool_t ReadFromFile( const zSTRING& fname ) {
      if( _access( fname, None ) > NotFound )
        return False;

      FILE* File = fopen( fname, "rb" );
      if( !File )
        return False;
      _setmode( _fileno( File ), 0x8000 );

      fseek( File, 0, SEEK_END );
      uint BufferLength = ftell( File );
      fseek( File, 0, SEEK_SET );

      Clear();
      char*  Buffer = new char[BufferLength];
      Buffer[BufferLength] = 0;
      bool_t IsReaded = fread( Buffer, sizeof( char ), BufferLength, File ) > 0;
      *this = Buffer;

      fclose( File );
      delete[] Buffer;
      return IsReaded;
    }

    NOINLINE bool_t ReadFromVdf( const zSTRING& fname, const long32& flags ) {
      long32 File = Vdfs32::vdf_fopen( fname.ToChar(), flags );
      if( File == Invalid )
        return False;

      Clear();
      uint32 BufferLength = Vdfs32::vdf_ffilesize( File );
      char*  Buffer = new char[BufferLength];
      Buffer[BufferLength] = 0;
      bool_t IsReaded = Vdfs32::vdf_fread( File, Buffer, BufferLength ) > 0;
      *this = Buffer;

      Vdfs32::vdf_fclose( File );
      delete[] Buffer;
      return IsReaded;
    }
#pragma endregion

#undef INLINE
#undef NOINLINE
#pragma endregion

#endif
  };

  inline zSTRING operator + ( const zSTRING& s1, const zSTRING& s2 ) {
    return zSTRING( s1 ) += s2.ToChar();
  }

  inline zSTRING operator + ( const zSTRING& s1, const char* s2 ) {
    return zSTRING( s1 ) += s2;
  }

  inline zSTRING& operator += ( zSTRING& s1, const zSTRING& s2 ) {
    return s1.Put( s2, s1.Length() );
  }

  // case Insensitive
  inline bool_t operator == ( const zSTRING& s1, const zSTRING& s2 ) {
    return s1.CompareI( s2 );
  }

  // case Insensitive
  inline bool_t operator == ( const zSTRING& s1, const char* s2 ) {
    return s1.CompareI( s2 );
  }

  // case Insensitive
  inline bool_t operator != ( const zSTRING& s1, const zSTRING& s2 ) {
    return !s1.CompareI( s2 );
  }

  // case Insensitive
  inline bool_t operator != ( const zSTRING& s1, const char* s2 ) {
    return !s1.CompareI( s2 );
  }

} // namespace Gothic_I_Addon

#undef USING_UNION_STRING_METHODS
#undef UNPROTECT_ZSTRING_METHODS
#endif // __ZSTRING_H__VER1__