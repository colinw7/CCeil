#include <CCeilP.h>
#include <CFile.h>
#include <CStrUtil.h>
#include <CReadLine.h>
#include <iostream>

int
main(int argc, char **argv)
{
  /* Initialise Parser */

  ClParserInst->init();

  std::string filename;

  for (int i = 1; i < argc; ++i) {
    if (argv[i][0] == '-') {
      if (strcmp(&argv[i][1], "debug") == 0)
        ClParserInst->setDebug(true);
      else
        std::cerr << "Invalid option" << argv[i] << "\n";
    }
    else
      filename = argv[i];
  }

  if (filename != "") {
    CFile file(filename);

    std::vector<std::string> lines;

    file.toLines(lines);

    ClParserValuePtr value;

    auto num_lines = lines.size();

    for (uint i = 0; i < num_lines; ++i) {
      std::string line = CStrUtil::stripSpaces(lines[i]);

      if (line == "" || line[0] == '#')
        continue;

      ClParserExpr expr(line);

      if (expr.exec(value))
        std::cout << line << " = " << *value << "\n";
      else
        std::cout << line << " = Error\n";
    }
  }
  else {
    CReadLine readline;

    readline.setPrompt("> ");

    for (;;) {
      std::string line = readline.readLine();

      if (line == "" || line[0] == '#')
        continue;

      if (line == "exit") break;

      ClParserExpr expr(line);

      ClParserValuePtr value;

      if (expr.exec(value))
        std::cout << line << " = " << *value << "\n";
      else
        std::cout << line << " = Error\n";
    }
  }

  ClParserInst->term();

  return 0;
}
