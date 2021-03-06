#include "Loader.hpp"


//extern int yylex(); 
extern int yyparse();
extern FILE* yyin;
extern int line_num;

namespace del {
    void Loader::parse(Domain_Interface* domain, std::string file_path) {
        std::ifstream file(file_path);
        if (!file) {
            std::cerr << "Can't open file " << file_path << std::endl;
            return;
        }
        Domain_Buffer buffer = Domain_Buffer();

        Custom_Lexer lexer;
        lexer.lex(file);
        Custom_Parser parser(domain, &buffer, &lexer);

        /*parse_domain(domain, &buffer, file);*/

        file.close();
    }
}
