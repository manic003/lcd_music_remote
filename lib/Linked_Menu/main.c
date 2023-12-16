#include "menu.h"
#include <unistd.h>

void select_handler(Menu_Entry *input){

    printf("\n%s\n",input->name);

}

int main(void ){
    setvbuf(stdout, NULL, _IONBF, 0);
    char test[] ="{name1,identifier1},{name2,identifier2},{name3,identifier3},{sasd,asda}";

    char *nextElement;
    char *next_playlist_tuple;

    char *save_element_ptr;
    char *save_sub_element_ptr;

    nextElement = strtok_r(test,"{}",&save_element_ptr); // get first string inside '{'  '}' 
   
   
    while (nextElement != NULL){
        printf("\n===>%s<===\n",nextElement);
        if (strcmp(nextElement,",\0")){ // skip the strings which just contain the ','

            next_playlist_tuple = strtok_r(nextElement,",",&save_sub_element_ptr); // get first element from tuple aka NAME
            
            
            printf("name:%s\n",next_playlist_tuple);
            next_playlist_tuple = strtok_r(NULL,",",&save_sub_element_ptr);  // get second element from tuple aka playlist URI
            
            printf("ident:%s\n",next_playlist_tuple);
            
            
    }
    nextElement = strtok_r(NULL,"{}",&save_element_ptr);
    }
    
    // exit(0);
    //  Menu *menu1 = create_empty_menu();
    // free(menu1);
    // while(1){
    //     if (menu1 != NULL){
    //         printf("\nnoch da\n");
           
    //     }
    //     else {
    //     printf("%s", menu1->uebergeordnetesMenu);
    //     exit(0);
    //     }
    //      usleep(200000);
    // }

Menu *menu1 = create_empty_menu();
  Menu *radio_menu = create_empty_menu();
  append_menu_entry(radio_menu,"start radio","{'command':'start_radio'}",NULL);
  append_menu_entry(radio_menu,"stop radio","{'command':'stop_radio'}",NULL);
 
    char *test1 =  "0123456789 0123456789 0123456789";
    append_menu_entry(radio_menu,test1,NULL,NULL);
    printf("%s\n",radio_menu->beginn->previous->previous->name);
     //create_menu_entry(NULL, NULL, NULL, 1, NULL, NULL);
    

  append_menu_entry(menu1, "Fav. PLAYLISTS","Fav. PLAYLISTS",NULL);
  append_menu_entry(menu1, "play ? playlist", "play ? playlist", NULL);
  append_menu_entry(menu1, "radio ",NULL, radio_menu->beginn);
  append_menu_entry(menu1,"run netflix scene","{'command':'run_netflix'}",NULL);

    



    // append_menu_entry(menu1, "SUBerster Eintrag","nix2",NULL);
    // append_menu_entry(menu1, "SUBzweiter Eintrag", "nix2", NULL);
    // append_menu_entry(menu1, "SUBdritter Eintrag","nix3",NULL);

    // Menu_Entry *neu = create_menu_entry(NULL,NULL,"neuer",0,"epmmsdsd",NULL);
    // replace_menu_entry(menu1, "SUBdritter Eintrag", neu);


    // Menu *menu2 = create_empty_menu();
    // append_menu_entry(menu2, "erster Eintrag","nix2",NULL);
    // append_menu_entry(menu2, "zweiter Eintrag", "nix2", NULL);
    // append_menu_entry(menu2, "dritter Eintrag","nix3",NULL);
    // append_menu_entry(menu2,"to sub",NULL,menu1->beginn);


    Menu *active_menu = menu1;

    int cmd ;
    printf("====%s\n",active_menu->aktueller_Menu_Entry->name);
    while (1){
        
    
        scanf("%d",&cmd);
        active_menu =navigate_in_menue(active_menu, cmd,&select_handler);

        if (active_menu == NULL){
           
           printf("\n\n\t durchgang beendt setze wieder auf active_menu\n\n");
           active_menu = menu1;
            }


                    if (active_menu == NULL){
           
                 fprintf(stderr, "\nERROR, tried to reach an  empty Menu in File:%s Line:%d\n", __FILE__, __LINE__);
                 exit(0);
           active_menu = menu1;
            }
        if (active_menu->aktueller_Menu_Entry == NULL)
                    fprintf(stderr, "\nERROR, tried to reach an  empty Menu in File:%s Line:%d\n", __FILE__, __LINE__);

        

        printf("%s\n",active_menu->aktueller_Menu_Entry->name);
    }

//     // alter haendischer MENU aufbau ================================================================================================
//     Menu_Entry sme1= {NULL, NULL,  "sub_a", 0, "sub_datastring_1"};
//     Menu_Entry sme2= {&sme1, NULL, "sub_b", 0, "sub_datastring_2"};
//     sme1.next = &sme2;

//     Menu_Entry sme3 = {&sme2, &sme1, "back", 0, "sub_datastring_exit"};
//     sme2.next = &sme3;
//     sme1.previous = &sme3;

//     Menu sm1;
//     sm1.beginn =&sme1;
//     sm1.aktueller_Menu_Entry =&sme1;
//     sm1.uebergeordnetesMenu = NULL;

//    // Menu sm1 = {&sme1, NULL, NULL, NULL};
//     Menu_Entry me1= {NULL, NULL, "main_a", 0, "datastring_1"};
//     Menu_Entry me2= {&me1, NULL, "main_b", 0, "datastring_2"};
//     me1.next = &me2;

//     // TODO folgenden 7 Zeilen sollten durch die eine folgende ersetzbar sein...  was ist das pr
//    // Menu_Entry me3 = {&me2, NULL , "to sub", 1, &sme1}; // todo wie erreicht man auf diese Weise direkt das 2. union element???
//     Menu_Entry me3;
//     me3.previous = &me2;
//     me3.next = NULL;
//     strcpy(me3.name,"main_to sub");
//     me3.is_submenu =1;
//     me3.data.submenu_beginn =  &sme1;

//     me2.next = &me3;
//     Menu_Entry me4= {&me3, &me1, "back", 0, "datastring_exit"};
//     me3.next = &me4;
//     me1.previous=&me4;

//     Menu m1;
//     m1.beginn =&me1;
//     m1.aktueller_Menu_Entry =&me1;
//     m1.uebergeordnetesMenu = NULL;

//     // Menu m1 = {&me1, NULL, NULL, NULL};
//     sm1.uebergeordnetesMenu = &m1;
//     // navigate_in_menue(&m1);
//     printf("size of menueintag: %ld bytes\n", sizeof(m1));


    // Menu_Entry newEntry = {NULL, NULL, "test",  0,"0000saasda"};    
    // Menu_Entry exitEintrag = {&newEntry, &newEntry, "back\0", 0, "EXIT"};
    
    
    // printf("%s", exitEintrag.previous->name);
      
    // printf("\nin main exit entry: ==> %s<===\n", menu2->beginn->previous->name);
    // printf("\nin main first entry: ==> %s<===\n", menu2->beginn->name);

    // printf("\nin main exit entry: ==> %s<===\n", menu2->aktueller_Menu_Entry->previous->name);
    // printf("\nin main first entry: ==> %s<===\n", menu2->aktueller_Menu_Entry->name);

    //  printf("in main : ==> %s<=== pointeradress %d\n", menu2->beginn->next->name,  *( menu2->beginn->next->name));


   

    // Menu_Entry *pointerME = &me1;

    // Menu *main_menu = &m1;
 
    //printf("%s\n",me3.data.submenu_beginn ->beginn.name); // todo FEHLER finden bei Struct.. wie faengt man oben richtig an (was zuertst definieren..)
    // while (main_menu->aktueller_Menu_Entry){



    // }

    // while (pointerME){
        
    //     if (! pointerME->is_submenu){
    //         printf("NAME=%s, DATA=%s \n", pointerME->name, pointerME->data.msg);
    //     }
        
    //     pointerME = pointerME -> next;

    // }
    
}
