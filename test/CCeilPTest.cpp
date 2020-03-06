#include <std_c++.h>
#include <CFile/CFile.h>
#include <CStrUtil/CStrUtil.h>
#include <CReadLine/CReadLine.h>
#include <CCeilP/CCeilP.h>

int
main(int argc, char **argv)
{
  /* Initialise Parser */

  ClParserInst->init();

  string filename;

  for (int i = 1; i < argc; ++i) {
    if (argv[i][0] == '-') {
      if (strcmp(&argv[i][1], "debug") == 0)
        ClParserInst->setDebug(true);
      else
        cerr << "Invalid option" << argv[i] << endl;
    }
    else
      filename = argv[i];
  }

  if (filename != "") {
    CFile file(filename);

    vector<string> lines;

    file.toLines(lines);

    ClParserValuePtr value;

    int num_lines = lines.size();

    for (int i = 0; i < num_lines; ++i) {
      string line = CStrUtil::stripSpaces(lines[i]);

      if (line == "" || line[0] == '#')
        continue;

      ClParserExpr expr(line);

      if (expr.exec(value))
        cout << line << " = " << value << endl;
      else
        cout << line << " = " << "Error" << endl;
    }
  }
  else {
    CReadLine readline;

    readline.setPrompt("> ");

    for (;;) {
      string line = readline.readLine();

      if (line == "" || line[0] == '#')
        continue;

      if (line == "exit") break;

      ClParserExpr expr(line);

      ClParserValuePtr value;

      if (expr.exec(value))
        cout << line << " = " << value << endl;
      else
        cout << line << " = " << "Error" << endl;
    }
  }

  ClParserInst->term();

  return 0;
}
