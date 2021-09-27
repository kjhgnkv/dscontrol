#pragma once

#include <string>

std::string getFileContent(const std::string& filename);

enum enum_type { type__NULL = -1, type_arg_object = 0, type_arg_protocol, type_arg_transport, type_arg_custom };

struct gengetopt_args_info
{
  const char *help_help; /**< @brief Print help and exit help description.  */
  const char *version_help; /**< @brief Print version and exit help description.  */
  const char *interactive_help; /**< @brief Enable interactive (wizard) mode. help description.  */
  char * include_directory_arg;	/**< @brief DSHost include directory. (default='../../DSHost/include').  */
  char * include_directory_orig;	/**< @brief DSHost include directory. original value given at command line.  */
  const char *include_directory_help; /**< @brief DSHost include directory. help description.  */
  char * output_directory_arg;	/**< @brief Output code directory. (default='./').  */
  char * output_directory_orig;	/**< @brief Output code directory. original value given at command line.  */
  const char *output_directory_help; /**< @brief Output code directory. help description.  */
  char * library_name_arg;	/**< @brief Library name (reverse domain name notation). (default='ds.host.component.test').  */
  char * library_name_orig;	/**< @brief Library name (reverse domain name notation). original value given at command line.  */
  const char *library_name_help; /**< @brief Library name (reverse domain name notation). help description.  */
  char * name_arg;	/**< @brief Project name (C++ identifier). (default='Test').  */
  char * name_orig;	/**< @brief Project name (C++ identifier). original value given at command line.  */
  const char *name_help; /**< @brief Project name (C++ identifier). help description.  */
  enum enum_type type_arg;	/**< @brief Component type. (default='object').  */
  char * type_orig;	/**< @brief Component type. original value given at command line.  */
  const char *type_help; /**< @brief Component type. help description.  */
  char * namespace_arg;	/**< @brief Code namespace (identifiers separated with ::). (default='').  */
  char * namespace_orig;	/**< @brief Code namespace (identifiers separated with ::). original value given at command line.  */
  const char *namespace_help; /**< @brief Code namespace (identifiers separated with ::). help description.  */
  const char *qt_gui_help; /**< @brief Generate support of Qt graphics. help description.  */

  unsigned int help_given ;	/**< @brief Whether help was given.  */
  unsigned int version_given ;	/**< @brief Whether version was given.  */
  unsigned int interactive_given ;	/**< @brief Whether interactive was given.  */
  unsigned int include_directory_given ;	/**< @brief Whether include-directory was given.  */
  unsigned int output_directory_given ;	/**< @brief Whether output-directory was given.  */
  unsigned int library_name_given ;	/**< @brief Whether library-name was given.  */
  unsigned int name_given ;	/**< @brief Whether name was given.  */
  unsigned int type_given ;	/**< @brief Whether type was given.  */
  unsigned int namespace_given ;	/**< @brief Whether namespace was given.  */
  unsigned int qt_gui_given ;	/**< @brief Whether qt-gui was given.  */

  int custom_component_mode_mode_counter; /**< @brief Counter for mode custom_component_mode */
  int object_component_mode_mode_counter; /**< @brief Counter for mode object_component_mode */
  int protocol_component_mode_mode_counter; /**< @brief Counter for mode protocol_component_mode */
  int transport_component_mode_mode_counter; /**< @brief Counter for mode transport_component_mode */
} ;

