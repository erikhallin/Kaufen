#ifndef CONSOLE_H
#define CONSOLE_H

#include <vector>
#include <string>
#include "decal.h"
#include "display.h"
#include "sound.h"

using namespace std;

/*

output:
visar meddelande som skickas till object (strings)
sparar n�gra stycken, kastas sedan

input:
skicka vem som skickar, ID
l�ngs ner finns aktuell string som inte skickats �nnu
ska kunna ta emot fr�n annan dator


visar:
en texture
strings ovanp�

beh�ver:
texture ID
tex_offset
message strukt:
  fr�n vem? ID namn?
  string vector


FIXA:
l�gg till alla konstiga symboler till display.h.....fixa input fr�n key (lite off fortfarande)
visar bara stora bokst�veer, shift sensor.....
st�ng av ramar i display,funk.....
bakgrundstexture.....

sen highscore tabell


*/

struct message
{
    string sender_tag;
    //int    sender_color?
    string text_string;
};

class console
{
    public:
        //Constructors
        console();
        console(int x_pos,int y_pos,int width,int height,int max_char,int numof_displayed_messages,int font_texture[3],int texture,
                float tex_offset_x_min,float tex_offset_x_max,float tex_offset_y_min,float tex_offset_y_max,sound* pSoundEngine);
        //Variables
        bool m_ready;
        //Functions
        bool input_text(string sender_tag,string text_string);
        bool input_text(string text_string);
        int  update(bool keys[],float time,string& message_to_send);
        bool draw(void);
    private:
        //Variables
        bool            m_caps_lock,m_handle_message_locally;
        int             m_x_pos,m_y_pos,m_width,m_height,m_max_char;
        int             m_numof_displayed_messages;
        int             m_texture;
        float           m_x_offset_min,m_y_offset_min,m_x_offset_max,m_y_offset_max;
        float           m_key_delay[256],m_time_last_cycle;
        string          m_owner_temp_string;
        display         m_diOwner_temp_string;
        decal           m_deBackground;
        vector<message> m_vMessages;//send to log?
        vector<display> m_vDisplays;

        sound*          m_pSoundEngine;

        //Functions
        bool add_char_to_owner_string(char letter);
        bool erase_char_from_owner_string(void);
        bool send_owner_string(void);
        char int_to_char(int key_id);
};


#endif // CONSOLE_H
