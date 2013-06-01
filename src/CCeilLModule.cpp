#include <CCeilL.h>

using std::string;
using std::cerr;
using std::endl;

/*------------------------------------------------------------------*
 *
 * defineModule
 *   Define a module which can subsequently be imported.
 *
 * CALL:
 *   defineModule(const string &name, ClModuleDef *module_def);
 *
 * INPUT:
 *   name       : Module Name.
 *
 *   module_def : Structure containing the method names and their
 *              : associated routine.
 *
 * OUTPUT:
 *   None
 *
 * RETURNS:
 *   None
 *
 * NOTES:
 *   None
 *
 *------------------------------------------------------------------*/

void
ClModuleMgr::
defineModule(const string &name, ClModuleDef *module_def)
{
  ClModule *module = createModule(name);

  for (int i = 0; module_def[i].name != ""; ++i) {
    if      (CStrUtil::casecmp(module_def[i].name, "init") == 0)
      module->setInitProc((ClModuleInitProc) module_def[i].proc);
    else if (CStrUtil::casecmp(module_def[i].name, "set" ) == 0)
      module->setSetProc ((ClModuleSetProc ) module_def[i].proc);
    else if (CStrUtil::casecmp(module_def[i].name, "term") == 0)
      module->setTermProc((ClModuleTermProc) module_def[i].proc);
    else if (CStrUtil::casecmp(module_def[i].name, "help") == 0)
      module->setHelpProc((ClModuleHelpProc) module_def[i].proc);
    else
      cerr << "Invalid Module Procedure Name '" << module_def[i].name << "'" << endl;
  }

  module_list_[name] = module;
}

void
ClModuleMgr::
defineModule(const string &name)
{
  ClModule *module = createModule(name);

  module_list_[name] = module;
}

ClModule *
ClModuleMgr::
createModule(const string &name)
{
  ClModule *module = getModule(name);

  delete module;

  module = new ClModule(name);

  return module;
}

void
ClModuleMgr::
setInitProc(const string &name, ClModuleInitProc proc, void *data)
{
  ClModule *module = getModule(name);

  if (module)
    module->setInitProc(proc, data);
}

void
ClModuleMgr::
setSetProc(const string &name, ClModuleSetProc proc, void *data)
{
  ClModule *module = getModule(name);

  if (module)
    module->setSetProc(proc, data);
}

void
ClModuleMgr::
setTermProc(const string &name, ClModuleTermProc proc, void *data)
{
  ClModule *module = getModule(name);

  if (module)
    module->setTermProc(proc, data);
}

void
ClModuleMgr::
setHelpProc(const string &name, ClModuleHelpProc proc, void *data)
{
  ClModule *module = getModule(name);

  if (module)
    module->setHelpProc(proc, data);
}

/*------------------------------------------------------------------*
 *
 * importModule
 *   Import a module which has been previously defined using
 *   defineModule() or exists as an external dynamic library
 *   (not implemeted yet).
 *
 * CALL:
 *   bool flag = importModule(const string &name);
 *
 * INPUT:
 *   name : Name of Module to Import.
 *
 * OUTPUT:
 *   None
 *
 * RETURNS:
 *   flag : Whether the import was successful or not.
 *
 * NOTES:
 *   The module is imported by executing the "init"
 *   methods routine from the module's definition
 *   structure.
 *
 *------------------------------------------------------------------*/

bool
ClModuleMgr::
importModule(const string &name)
{
  ClModule *module = getModule(name);

  if (module == NULL) {
    if (! importExternalModule(name))
      return false;

    module = getModule(name);

    if (module == NULL)
      return false;
  }

  return module->execInitProc();
}

/*------------------------------------------------------------------*
 *
 * getModuleList
 *   Get a list of the names of the currently defined modules.
 *
 * CALL:
 *   getModuleList(StringVectorT &modules)
 *
 * INPUT:
 *   None
 *
 * OUTPUT:
 *   modules : List of Modules
 *
 * RETURNS:
 *   None
 *
 * NOTES:
 *   None
 *
 *------------------------------------------------------------------*/

void
ClModuleMgr::
getModuleList(StringVectorT &modules) const
{
  modules.clear();

  ModuleMap::const_iterator p1 = module_list_.begin();
  ModuleMap::const_iterator p2 = module_list_.end  ();

  for ( ; p1 != p2; ++p1)
    modules.push_back((*p1).second->getName());
}

/*------------------------------------------------------------------*
 *
 * isModuleLoaded
 *   Check if the named module has been imported or not.
 *
 * CALL:
 *   bool flag = isModuleLoaded(const string &name);
 *
 * INPUT:
 *   name : Module Name.
 *
 * OUTPUT:
 *   None
 *
 * RETURNS:
 *   flag : Whether the module has been imported or not.
 *
 * NOTES:
 *   None
 *
 *------------------------------------------------------------------*/

bool
ClModuleMgr::
isModuleLoaded(const string &name) const
{
  ClModule *module = getModule(name);

  if (module == NULL)
    return false;

  return module->isLoaded();
}

/*------------------------------------------------------------------*
 *
 * execModuleSet
 *   Call a module's set method for the specified option
 *   and its value.
 *
 * CALL:
 *   bool flag =
 *     execModuleSet(const string &name, char *option, char *args);
 *
 * INPUT:
 *   name   : Name of Module
 *
 *   option : Option Name to Set.
 *
 *   args   : String containing additional arguments
 *          : to use to set the option.
 *
 * OUTPUT:
 *     None
 *
 * RETURNS:
 *   flag   : Whether the set was successful or not.
 *
 * NOTES:
 *   Calls the module's "set" method if defined.
 *
 *------------------------------------------------------------------*/

bool
ClModuleMgr::
execModuleSet(const string &name, const string &option, const string &args)
{
  ClModule *module = getModule(name);

  if (module == NULL)
    return false;

  return module->execSetProc(option, args);
}

/*------------------------------------------------------------------*
 *
 * execModuleHelp
 *   Call a module's help method.
 *
 * CALL:
 *   bool flag =
 *     execModuleHelp(const string &name, const string &args);
 *
 * INPUT:
 *   name : Name of Module
 *
 *   args : String containing additional arguments
 *        : supplied when help was invoked.
 *
 * OUTPUT:
 *   None
 *
 * RETURNS:
 *   flag : Whether the set was successful or not.
 *
 * NOTES:
 *   Calls the module's "help" method if defined.
 *
 *------------------------------------------------------------------*/

bool
ClModuleMgr::
execModuleHelp(const string &name, const string &args)
{
  ClModule *module = getModule(name);

  if (module == NULL)
    return false;

  return module->execHelpProc(args);
}

/*------------------------------------------------------------------*
 *
 * reinitModules
 *   Reinitialise all modules.
 *
 * CALL:
 *   reinitModules();
 *
 * INPUT:
 *   None
 *
 * OUTPUT:
 *   None
 *
 * RETURNS:
 *   None
 *
 * NOTES:
 *   Called when the 'new' command is entered.
 *
 *------------------------------------------------------------------*/

void
ClModuleMgr::
reinitModules()
{
  ModuleMap::const_iterator p1 = module_list_.begin();
  ModuleMap::const_iterator p2 = module_list_.end  ();

  for ( ; p1 != p2; ++p1) {
    ClModule *module = (*p1).second;

    module->execReinitProc();
  }
}

/*------------------------------------------------------------------*
 *
 * reinitModule
 *   Reinitialise the named module.
 *
 * CALL:
 *   bool flag = reinitModule(char *name);
 *
 * INPUT:
 *   name : Module Name.
 *
 * OUTPUT:
 *   None
 *
 * RETURNS:
 *   flag : Whether the reinitialisation was successful or not.
 *
 * NOTES:
 *   None
 *
 *------------------------------------------------------------------*/

bool
ClModuleMgr::
reinitModule(const string &name)
{
  ClModule *module = getModule(name);

  if (module == NULL)
    return false;

  return module->execReinitProc();
}

/*------------------------------------------------------------------*
 *
 * termModules
 *   Terminate all modules.
 *
 * CALL:
 *   termModules();
 *
 * INPUT:
 *   None
 *
 * OUTPUT:
 *   None
 *
 * RETURNS:
 *   None
 *
 * NOTES:
 *   None
 *
 *------------------------------------------------------------------*/

void
ClModuleMgr::
termModules()
{
  ModuleMap::const_iterator p1 = module_list_.begin();
  ModuleMap::const_iterator p2 = module_list_.end  ();

  for ( ; p1 != p2; ++p1) {
    ClModule *module = (*p1).second;

    module->execTermProc();
  }
}

/*------------------------------------------------------------------*
 *
 * termModule
 *   Terminate the named module.
 *
 * CALL:
 *   bool flag = termModule(const string &name);
 *
 * INPUT:
 *   name : Module Name.
 *
 * OUTPUT:
 *   None
 *
 * RETURNS:
 *   flag : Whether the termination was successful or not.
 *
 * NOTES:
 *   None
 *
 *------------------------------------------------------------------*/

bool
ClModuleMgr::
termModule(const string &name)
{
  ClModule *module = getModule(name);

  if (module == NULL)
    return false;

  return module->execTermProc();
}

/*------------------------------------------------------------------*
 *
 * importExternalModule
 *   Import Module from External Dynamic Library (where
 *   supported).
 *
 * CALL:
 *   bool flag =
 *     importExternalModule(const string &name);
 *
 * INPUT:
 *   name : Module Name.
 *
 * OUTPUT:
 *   None
 *
 * RETURNS:
 *   flag : Whether the import was successful or not.
 *
 * NOTES:
 *   Searches the directory lists contained in the "CL_LIBRARY_PATH" and "LD_LIBRARY_PATH"
 *   environment variables for a dynamic library called 'cl_<name>.so'. If this is found it
 *   is loaded and the symbol 'cl_<name>_module_def' is searched for. If this is found it
 *   is taken as the address of a structure containing the module definition and the module
 *   is defined by calling defineModule().
 *
 *------------------------------------------------------------------*/

bool
ClModuleMgr::
importExternalModule(const string &/*name*/)
{
#ifdef CL_IMPORT_EXTERNAL
  string libname = moduleSearchLib("CL_LIBRARY_PATH", name);

  if (libname == NULL)
    libname = moduleSearchLib("LD_LIBRARY_PATH", name);

  if (libname == NULL)
    libname = moduleSearchLib(NULL, name);

  if (libname == NULL) {
    ClLanguageMgrInst->error("Failed to Find Module for %s", name);
    return false;
  }

  /*-------------*/

  char *symbol = NULL;

  CShLib shlib(libname);

  if (! shlib.open()) {
    ClLanguageMgrInst->error("Failed to Open Library %s", libname.c_str());
    return false;
  }

  string symname;

  if (name[0] != '_') {
    symname = "cl_";

    symname += name;
    symname += "_module_def";

    if (! shlib.getData(symname, &symbol)) {
      ClLanguageMgrInst->error("Failed to Find Symbol %s - %s", symname.c_str(), error);
      return false;
    }
  }
  else {
    symname = "_init";

    if (! shlib.getData(symname, &symbol)) {
      ClLanguageMgrInst->error("Failed to Find Symbol %s - %s", symname.c_str(), error);
      return false;
    }
  }

  if (symbol == NULL)
    return false;

  /*-------------*/

  defineModule(name, (ClModuleDef *) symbol);

  return true;
#else
  return false;
#endif
}

#ifdef CL_IMPORT_EXTERNAL
/*------------------------------------------------------------------*
 *
 * moduleSearchLib
 *   Search the directory list contained in the
 *   specified environment variable for a file
 *   called 'cl_<name>.so'.
 *
 * CALL:
 *   string filename =
 *     moduleSearchLib(const string &envname, const string &name);
 *
 * INPUT:
 *   envname  : Environment Variable Name.
 *
 *   name     : Module Name.
 *
 * RETURNS:
 *   filename : Full path name of the library file (NULL if not found).
 *
 *------------------------------------------------------------------*/

string
ClModuleMgr::
moduleSearchLib(const string &envname, const string &name)
{
  if (COS::checkenv(envname))
    envval = COS::getenv(envname);
  else
    envval = "/usr/lib /lib";

  StringVectorT paths;

  CStrUtil::addWords(envval, paths);

  unsigned int num_paths = paths.size();

  string filename;

  unsigned int i = 0;

  for ( ; i < num_paths; ++i) {
    filename = paths[i];

    if (name[0] != '_') {
      filename += "/cl_";
      filename += name;
      filename += ".so";
    }
    else {
      filename += "/lib";
      filename += &name[1];
      filename += ".so";
    }

    if (CFile::isRegular(filename))
      break;
  }

  if (i >= num_paths)
    return "";

  return filename;
}
#endif

/*------------------------------------------------------------------*
 *
 * getModule
 *   Get the Module Data Structure for the Named Module.
 *
 * CALL:
 *   ClModule *module =
 *     getModule(const string &name);
 *
 * INPUT:
 *   name   : Module Name.
 *
 * OUTPUT:
 *   None
 *
 * RETURNS:
 *   module : Module Data Structure (NULL if not found).
 *
 * NOTES:
 *   None
 *
 *------------------------------------------------------------------*/

ClModule *
ClModuleMgr::
getModule(const string &name) const
{
  ModuleMap::const_iterator p = module_list_.find(name);

  if (p != module_list_.end())
    return (*p).second;

  return NULL;
}

/*------------------------------------------------------------------*
 *
 * deleteModules
 *   Delete all resources used by modules.
 *
 * CALL:
 *   deleteModules();
 *
 * INPUT:
 *   None
 *
 * OUTPUT:
 *   None
 *
 * RETURNS:
 *   None
 *
 * NOTES:
 *   Called before CL terminates.
 *
 *------------------------------------------------------------------*/

void
ClModuleMgr::
deleteModules()
{
  ModuleMap::const_iterator p1 = module_list_.begin();
  ModuleMap::const_iterator p2 = module_list_.end  ();

  for ( ; p1 != p2; ++p1)
    delete (*p1).second;

  module_list_.clear();
}

//------------------

ClModule::
ClModule(const string &name) :
 name_(name)
{
  init_proc_ = NULL; init_data_ = NULL;
  set_proc_  = NULL; set_data_  = NULL;
  term_proc_ = NULL; term_data_ = NULL;
  help_proc_ = NULL; help_data_ = NULL;

  state_ = MODULE_UNLOADED;
}

bool
ClModule::
execInitProc()
{
  if (isLoaded())
    return true;

  if (init_proc_) {
    bool rc = (*init_proc_)(CL_MODULE_INIT_TYPE_INIT, init_data_);

    if (rc)
      state_ = MODULE_LOADED;

    return rc;
  }
  else
    return false;
}

bool
ClModule::
execReinitProc()
{
  if (! isLoaded())
    return false;

  if (init_proc_)
    return (*init_proc_)(CL_MODULE_INIT_TYPE_REINIT, init_data_);
  else
    return false;
}

bool
ClModule::
execSetProc(const string &option, const string &args)
{
  if (! isLoaded())
    return false;

  if (set_proc_)
    return (*set_proc_)(option.c_str(), args.c_str(), set_data_);
  else
    return false;
}

bool
ClModule::
execTermProc()
{
  if (! isLoaded())
    return false;

  if (term_proc_)
    return (*term_proc_)(term_data_);
  else
    return false;
}

bool
ClModule::
execHelpProc(const string &args)
{
  if (! isLoaded())
    return false;

  if (help_proc_)
    return (*help_proc_)(args.c_str(), help_data_);
  else
    return false;
}
