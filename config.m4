

PHP_ARG_WITH(tokyocabinet, for tokyocabinet support,
[  --with-tokyocabinet[=DIR]             Include tokyocabinet support])



if test "$PHP_TOKYOCABINET" != "no"; 
then

  
  PHP_NEW_EXTENSION(tokyocabinet, tokyocabinet.c, $ext_shared) 
  PHP_SUBST(TOKYOCABINET_SHARED_LIBADD)
  
  AC_DEFINE(HAVE_TOKYOCABINETLIB,1,[ ])

  TOKYOCABINET_INCDIR=$PHP_TOKYOCABINET/include
  TOKYOCABINET_LIBDIR=$PHP_TOKYOCABINET/lib
  
  PHP_ADD_LIBRARY_WITH_PATH(tokyocabinet, $TOKYOCABINET_LIBDIR, TOKYOCABINET_SHARED_LIBADD)
  PHP_ADD_INCLUDE($TOKYOCABINET_INCDIR)  
  
fi 

