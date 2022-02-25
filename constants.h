/* all string constants that are used in library_client.c */

#define BOOKSTART "["
#define DELIM "\""
#define ERRMSG "***ERROR***: "
#define VRFLIBENT "Make sure you entered the library."
#define SUCENTLIB "***SUCCESS***: You have entered the library!"
#define SUCLOGMESG "***SUCCESS***: You have logged-in successfully!"
#define SUCREGMESG "***SUCCESS***: You have registered successfully!"
#define SUCADDBOOK "***SUCCESS***: You have added a book to the library!"
#define SUCDELBOOK "***SUCCESS***: You have deleted the book from the library!"
#define SUCOUTMESG "***SUCCESS***: You have logged-out successfully!"

#define DELLABEL "delete_book"
#define ERRORLABEL "error"
#define IDLABEL "id"
#define ENTRLABEL "enter_library"
#define EXITLABEL "exit"
#define ADDBOOKLABEL "add_book"
#define GETBOOKLABEL "get_book"
#define GETBOOKSLABEL "get_books"
#define LOGINLABEL "login"
#define LOGOUTLABEL "logout"
#define REGISTERLABEL "register"
#define TOOMANYREQLABEL "Too many"

#define COOKIEDELIM ";"
#define COOKIESTART "connect.sid"
#define MAXCOOKIELEN 128
#define MAXTOKLEN 320
#define MAXINPUTLEN 64
#define TOKENSTART "token"
#define TRUE 1
#define UNKNOWNCOMM "Unknown command.\n"

#define ACCURL "/api/v1/tema/library/access"
#define BOKURL "/api/v1/tema/library/books"
#define LOGURL "/api/v1/tema/auth/login"
#define OUTURL "/api/v1/tema/auth/logout"
#define PLTYPE "application/json"
#define REGURL "/api/v1/tema/auth/register"

#define IDBKPROMPT "id="
#define PASSPROMPT "password="
#define USERPROMPT "username="
#define TITLPROMPT "title="
#define AUTHPROMPT "author="
#define GENRPROMPT "genre="
#define PUBLPROMPT "publisher="
#define PGCTPROMPT "page_count="

#define ATITL "Please insert a valid title."
#define AAUTH "Please insert a valid author."
#define AGENR "Please insert a valid genre."
#define APUBL "Please insert a valid publisher."
#define ANUMB "Please insert a valid number."

#define JSONPASS "password"
#define JSONUSER "username"
#define JSONTITL "title"
#define JSONAUTH "author"
#define JSONGENR "genre"
#define JSONNUMB "page_count"
#define JSONPUBL "publisher"
