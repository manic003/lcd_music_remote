#ifdef __cplusplus
extern "C" {
#endif


#ifndef menu_H_
#define menu_H_

#include <string.h>
#include <stdlib.h>
#include <stdio.h>


typedef enum{NAVIGATION_BACK = -1, NAVIGATION_SELECT = 0, NAVIGATION_NEXT  =1} Navigation;


typedef struct _menu_entry {
  struct _menu_entry *previous;
  struct _menu_entry *next;
  char name[20];
  int is_submenu;
  union data{
    char msg[150];
    struct _menu_entry *submenu_beginn;

  } data;

} Menu_Entry;


/*
  \struct
  \var Menu_Eintrag:  *seginn

*/

typedef struct _Menu {
  Menu_Entry *beginn;
  //Menu_Eintrag *ende;
  Menu_Entry *aktueller_Menu_Entry;
  struct _Menu *uebergeordnetesMenu;
  //Menu_Eintrag *exit_eintrag;
} _Menu ; 

typedef struct _Menu Menu;

Menu *navigate_in_menue(Menu *menu, Navigation navigation, void (*select_handler_function)(Menu_Entry *menu_entry));
Menu *navigate_in_menue_test(Menu *menu); // TODO remove


Menu* create_empty_menu(void);


void append_menu_entry(Menu *menu,const char name[20], const char optional_data_string[], Menu_Entry *optional_submenu);

Menu_Entry* create_menu_entry(Menu_Entry *prev, Menu_Entry *next,const char name[20],int is_submenu,const char optional_data_string[150], Menu_Entry *submenu_beginn);

int replace_menu_entry(Menu *menu, char name_of_entry_to_remove[20], Menu_Entry *new_entry);

Menu *navigate_in_menue_test(Menu *menu);

#endif /* menu_H_ */

#ifdef __cplusplus
} // extern "C"
#endif