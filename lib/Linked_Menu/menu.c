#include "menu.h"
#define DEBUG_ON
#define LOG(msg) printf("***\nERROR in a LinkedMenu function, see FILE: %s LINE: %d\n>> %s***\n\n",__FILE__, __LINE__, msg)
/*  \func append_menu_entry()
    
    \brief appends a  Menu entry to the menu and takes care that the menu is 
    always a linked list where the last entry points to the first one

*   \param *menu    Menu in das der Eintrag eingefuegt werden soll
    \param name     Name der Menuepunkts (max. 20 Chars)
    
    \param optional_data_string
    \param optional_submenu  if the parameter optional_data_string != NULL, the parameter  optional_submenu will be ignored



*/
void append_menu_entry(Menu *menu, const char name[20], const char optional_data_string[], Menu_Entry *optional_submenu)
{

    if (menu == NULL){
    #if defined(DEBUG_ON)
        LOG("Tried to append to  an empty Menu! menu is NULL\n");
    #endif // DEBUG_ON
    return;
    }
    Menu_Entry *newEntry;

    // create the new menu entry
    if (optional_data_string == NULL)
    {

        newEntry = create_menu_entry(NULL, NULL, name, 1, NULL, optional_submenu);
    }

    else
        newEntry = create_menu_entry(NULL, NULL, name, 0, optional_data_string, NULL);

    if (newEntry == NULL){

        #if defined(DEBUG_ON)
            //printf("append menu entry failed due to fail in create_menu_entry()\n");
            LOG("append menu entry failed due to fail in create_menu_entry()\n");
        
        #endif // DEBUG_ON
        return;

    }
    // leeres Menu
    if (menu->beginn == NULL)
    /*  If Menu empty create new entry by parameters and and the exit menu entry

            MenuEntry1 next: ExitEntry
            MenuEntry previous: ExitEntry

            ExitEntry next: MenuEntry
            ExitEntry previous: MenuEntry
                    
            next pointer:
            ------------------------------------                                
            |                                  |
            ->  (MenuEntry1)  -> (ExitEntry) ---   
           

            previous pointer:
            --- (MenuEntry1)  <- (ExitEntry)  <-
            |                                  |
            ------------------------------------                   
        */
    {

        Menu_Entry *exit = create_menu_entry(newEntry, newEntry, "back", 0, "EXIT_ENTRY_OF_MENU", NULL);

        // connect new entry to exitentry
        newEntry->next = exit;
        newEntry->previous = exit;

        // init menu with new start entry
        menu->beginn = newEntry;
        menu->aktueller_Menu_Entry = newEntry;
        menu->uebergeordnetesMenu = NULL;
    }

    // if Menu already has two entries...
    else if (menu->beginn != NULL)
    {
        // insert entry betwen exit entry and the one before the exit entry
        // FIRST_ENTRY -->   EXIT_ENTRY
        //                  -->  NEW_ENTRY -->

        // get exit entry (the one 'before' the first entry)
        Menu_Entry *exit = menu->beginn->previous;

        newEntry->previous = exit->previous;
        newEntry->next = exit;
        exit->previous->next = newEntry;
        exit->previous = newEntry;

        if (optional_data_string != NULL)
        {
            strcpy(newEntry->name, name);
            newEntry->is_submenu = 0;
            strcpy(newEntry->data.msg, optional_data_string);
        }

        else
        {
            strcpy(newEntry->name, name);
            newEntry->is_submenu = 1;
            newEntry->data.submenu_beginn = optional_submenu;
        }
    }
};

/*
    \name  create_empty_menu(void)

    \brief creates an empty menu pointer
    \return *Menu or NULL if no memory is available

*/
Menu *create_empty_menu(void)
{
    Menu *menu = malloc(sizeof(Menu));

    if (menu == NULL)
        return NULL;

    menu->aktueller_Menu_Entry = NULL;
    menu->beginn = NULL;
    menu->uebergeordnetesMenu = NULL;

    return menu;
};

/*
    \name create_menu_entry
    \brief creates a menu entry with the given parameter
    \param prev         previous menu entry - NULL if none
    \param next         next menu entry - NULL if none
    \param name         char[20] for the displayed name
    \param is_submenu   0 for False, 1 for True
    \param optional_data_string     char[150] for a message which will be send via mqtt
    \param submenu_beginn           if is_submenu=1 the first menu_entry of the submenu  
    \return a new menu_entry pointer or NULL if no memory is available

*/
Menu_Entry *create_menu_entry(Menu_Entry *prev, Menu_Entry *next, const char name[20], int is_submenu, const char optional_data_string[150], Menu_Entry *submenu_beginn)
{
    Menu_Entry *entry = malloc(sizeof(Menu_Entry));
    if (entry == NULL){
        #if defined(DEBUG_ON)
        LOG("allocation for MenuEntry failed\n");
        #endif // DEBUG_ON
        return NULL;
    }

    if (name == NULL){
        #if defined(DEBUG_ON)
        LOG("Cannot create a MenuEntry with empty name! parameter name is NULL\n");
        #endif // DEBUG_ON
        return NULL;
    }

    entry->previous = prev;
    entry->next = next;
    strcpy(entry->name, name);
    entry->is_submenu = is_submenu;
    if (!is_submenu)
        strcpy(entry->data.msg, optional_data_string);
    else
        entry->data.submenu_beginn = submenu_beginn;

    return entry;
};

/*
    \name delete_menu(Menu *menu)
    \brief frees all menu entries inside the menu and the menu itself
    \param *menu the menu which shall be deleted
    \return 0 on SUCCESS -1 on FAILURE 
*/
int delete_menu(Menu *menu)
{

    if (menu == NULL){
        #if defined(DEBUG_ON)
            LOG("Tried to delete  an empty Menu! menu is NULL\n");
        #endif // DEBUG_ON
        return -1;

    }

    if (menu ->beginn == NULL || menu->beginn->previous == NULL){
    
        #if defined(DEBUG_ON)
            LOG("Tried to delete an broken Menu!  Calling free() on menu \n");
        #endif // DEBUG_ON
        free(menu);
        return -1;
    }


    Menu_Entry *currentptr = menu->beginn;
    Menu_Entry *nextptr = currentptr->next;
    currentptr->previous->next = NULL; // Ende abtrennen

    while (nextptr != NULL)
    {

        // save next pointer
        nextptr = currentptr->next;
 
        
        

        // free current pointer
        free(currentptr);

        // set current pointer to next, for next loop
        currentptr = nextptr;
    }
    free(menu);

    return 0;
}

/*
    \func replace_menu_entry(Menu *menu, char name_of_entry_to_remove[20], Menu_Entry *new_entry)

    \brief replaces the first menu entry which matches the given name with the new entry 
    \param *menu the menu in which to be replace
    \param name_of_entry_to_remove identifier of the menu_entry to remove
    \param *new_entry the new Menu Entry

*/
int replace_menu_entry(Menu *menu, char name_of_entry_to_remove[20], Menu_Entry *new_entry)
{
    if (menu == NULL){
    #if defined(DEBUG_ON)
        LOG("Tried to operate on an empty Menu! menu is NULL\n");
    #endif // DEBUG_ON
    return -1;
    }

    if (new_entry == NULL){
        #if defined(DEBUG_ON)
            LOG("Cannot replace a menu entry with an empty menu entry! Parameter new_entry is NULL!\n");
        #endif // DEBUG_ON
        return -1;
    }
    //search for entry by name
    Menu_Entry *current_entry = menu->beginn;
    Menu_Entry *exit_entry = menu->beginn->previous;


    while (current_entry && strcmp(current_entry->name, name_of_entry_to_remove) != 0 && current_entry != exit_entry)
    {

        current_entry = current_entry->next;
    }

    // ? current_entry != NULL &&
    if (current_entry != exit_entry)
    {
 
        new_entry->previous = current_entry->previous;
        new_entry->next = current_entry->next;

        current_entry->previous->next = new_entry;
        current_entry->next->previous = new_entry;
        free(current_entry);
    }

    return 0;
}

/**
*   \fn      navigate_in_menu(Menu *menu)
*   \brief   navigate in the menu
*   
*   This function can be called inside a loop by handing in the commands (BACK, SELECT or NEXT) as  Integers  (-1, 0 or  1) 
*   \param menu     the menu in which the navigation shall take place
*   \param cmd      the direction (previous/left/up = 1    next/right/down = -1  select = 0)      
*   \param select_handler_function a function with has an char array as input parameter and performs the wanted 
*                                  action every time "SELECT" is pressed AND the current Menu_Entry is not an submenu
*                                  (in this case navigate_in_menu will return the following submenu)
*   \return menu    can be: 
*                            - the same menu with different 'aktueller_Menu_Entry'
*                            - exactly the same menu, if SELECT is called and the select_handler_function has been called
*                            - a submenu 
*                            - NULL if selection is called on the exit_entry of the topmenu
*
*
**/
Menu *navigate_in_menue(Menu *menu, Navigation navigation, void (*select_handler_function)(Menu_Entry *menu_entry))
{
    if (menu == NULL){
        #if defined(DEBUG_ON)
            LOG("Tried to navigate in an empty Menu!\n");
        #endif // DEBUG_ON
        return menu;
    }

    if (menu ->aktueller_Menu_Entry == NULL){
        
        #if defined(DEBUG_ON)
            LOG("Tried to navigate in an broken Menu! menu->aktueller_Menu_Entry is NULL\n");
        #endif // DEBUG_ON
        return menu;
    }
    // if (menu)
    //     fprintf(stderr, "\nERROR, tried to reach an  empty Menu in File:%s Line:%d\n", __FILE__, __LINE__);


    Menu_Entry *aktuell;
    aktuell = malloc(sizeof(Menu_Entry));
    aktuell = menu->aktueller_Menu_Entry;

       if (aktuell == NULL)
            fprintf(stderr, "\nERROR, tried to reach an  empty Menu in File:%s Line:%d\n", __FILE__, __LINE__);




    if (navigation == NAVIGATION_BACK)
    {

        menu->aktueller_Menu_Entry = aktuell->previous;
    }
    else if (navigation == NAVIGATION_NEXT)
    {

        menu->aktueller_Menu_Entry = aktuell->next;
    }

    else if (navigation == NAVIGATION_SELECT)
    {
        if (aktuell->is_submenu)
        {
         

            Menu *new_sub_menu;
            new_sub_menu = malloc(sizeof(Menu));
            new_sub_menu->beginn = aktuell->data.submenu_beginn;
            new_sub_menu->aktueller_Menu_Entry = aktuell->data.submenu_beginn;
            new_sub_menu->uebergeordnetesMenu = menu;
            menu = new_sub_menu;
        }
        // gehe zu top menu
        else if (strcmp("EXIT_ENTRY_OF_MENU", aktuell->data.msg) == 0)
        {
            // printf("freeing menu\n");
            // free(menu);
            // menu = menu->uebergeordnetesMenu; // sollte ja immer null sein


             
            
            Menu *subMenu = menu;
            menu = subMenu->uebergeordnetesMenu;
            // free(subMenu);
            
            // if (menu != NULL)
            //     printf("\ngehe zu top menu..\n");
            // else
            //     printf("\n=== exit == \n");
            //     return menu;
        }

        else
        {
            select_handler_function(aktuell);
            //printf("\t MSG=%s\n", aktuell->data.msg);
        }
    }
    
    return menu;
}

Menu *navigate_in_menue_test(Menu *menu)
{
    printf("NAVIGATION  < = -1     0 = enter  1 = >:\n");

    Menu_Entry *aktuell;
    aktuell = malloc(sizeof(Menu_Entry));

    int cmd;

    while (menu != NULL)
    {
        //        printf("NAME=%s\n",aktuell->name);
        aktuell = menu->aktueller_Menu_Entry;
        printf("--> %s\n", aktuell->name);

        scanf("%d", &cmd);

        if (cmd < 0)
        {

            menu->aktueller_Menu_Entry = aktuell->previous;
        }
        else if (cmd > 0)
        {

            menu->aktueller_Menu_Entry = aktuell->next;
        }

        else if (cmd == 0)
        {
            if (aktuell->is_submenu)
            {
                // .. todo

                printf("erzeuge neues menu..");

                Menu *new_sub_menu;
                new_sub_menu = malloc(sizeof(Menu));
                new_sub_menu->beginn = aktuell->data.submenu_beginn;
                new_sub_menu->aktueller_Menu_Entry = aktuell->data.submenu_beginn;
                new_sub_menu->uebergeordnetesMenu = menu;
                menu = new_sub_menu;
            }
            // gehe zu top menu
            else if (strcmp("EXIT_ENTRY_OF_MENU", aktuell->data.msg) == 0)
            {

                menu = menu->uebergeordnetesMenu;
                if (menu != NULL)
                    printf("\ngehe zu top menu..\n");
                else
                    printf("\n=== exit == \n");
            }

            else
            {
                printf("\t MSG=%s\n", aktuell->data.msg);
            }
        }
    }
    return menu;
}