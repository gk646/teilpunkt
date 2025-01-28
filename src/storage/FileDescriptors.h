#ifndef TPUNKT_FILEDESCRIPTORS_H
#define TPUNKT_FILEDESCRIPTORS_H

namespace tpunkt
{
    struct StringFileDescriptor final
    {
      //  static FileDescriptor fromString(const char* string);

      private:
        bool valid = false;
        FileSystemPath path;
    };


  struct IDFileDescriptor final
  {

  };

} // namespace tpunkt

#endif // TPUNKT_FILEDESCRIPTORS_H