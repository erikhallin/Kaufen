#ifndef TEXT_INPUT_BOX_H
#define TEXT_INPUT_BOX_H

#include "display.h"
#include "sound.h"
#include <string>
#include <gl\gl.h>

using namespace std;

/*TO-DO

test if clicked, change state

test for input, if marked change text

ska synas att den är markerad...

*/

class text_input_box
{
    public:
        //Constructors
        text_input_box();
        text_input_box(int x_pos,int y_pos,int width,int height,int max_char,int font_texture[3],string text);
        text_input_box(int x_pos,int y_pos,int width,int height,int max_char,int font_texture[3],string text,sound* pSoundEngine);
        //Variables
        bool m_ready;
        //Functions
        bool   input_text(string text_string);
        bool   set_text(string sText);
        string get_text(void);
        bool   set_value(float value);
        float  get_value(void);
        bool   mark_test(int mouse_x_pos,int mouse_y_pos);
        bool   is_marked(void);
        bool   unmark(void);
        bool   change_border_color(float color[3]);
        bool   set_only_number(bool flag);
        bool   update(bool keys[],float time);
        bool   draw(void);


    private:
        //Variables
        bool    m_sound_enabled;
        bool    m_only_number_input;
        int     m_x_pos,m_y_pos,m_width,m_height,m_max_char;
        bool    m_caps_lock,m_marked;//if marked, input will be put in text display
        float   m_key_delay[256],m_time_last_cycle;
        string  m_text;
        display m_diTextBox;
        sound*  m_pSoundEngine;
        //Functions
        bool add_char(char letter);
        bool erase_char(void);
        char int_to_char(int key_id);
};



#endif // TEXT_INPUT_BOX_H
