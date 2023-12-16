#include "menu.h"

void test_append_menu_entry(void){
    Menu *newMenu;
    // Menu_Entry *newEntry= {}

    Menu *newSubMenu;
    newSubMenu->beginn = 
    newMenu->beginn = NULL;
    newMenu->aktueller_Menu_Entry = NULL;
    newMenu->uebergeordnetesMenu = NULL;

    
    char name[20] = "Name des Menus";
    char *name_to_long = "0123456789 0123456789 0123456789";

    char optional_data_msg[150];
    char optional_data_msg_to_long[200];


    append_menu_entry(newMenu,name,optional_data_msg,NULL);




}