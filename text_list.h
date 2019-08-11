#ifndef TEXT_LIST_H
#define TEXT_LIST_H

#include "decal.h"
#include "display.h"
#include "button.h"
#include <string>
#include <vector>

#define _click_delay 0.2

using namespace std;

class text_list
{
    public:
        //Constructors
        text_list();
        text_list(int x_pos,int y_pos,int width,int height,int numof_players,int font_texture[3],int texture,
                  float tex_offset_x_min,float tex_offset_x_max,float tex_offset_y_min,float tex_offset_y_max);
        //Variables
        bool m_ready;
        //Functions
        bool add_player(string name);
        bool remove_player(string name);
        bool draw(void);

    private:
        //Variables
        int    m_x_pos,m_y_pos,m_width,m_height,m_texture;
        int    m_numof_players;
        int    m_font_textures[3];

        float  m_x_offset_min,m_y_offset_min,m_x_offset_max,m_y_offset_max;
        float  m_LMB_delay;

        decal  m_deBackground;
        vector<display>  m_vDisplays_names;//have constant index value
        vector<string>   m_vPlayer_names;
        //Functions
        bool   sort_list(void);
};

#endif // TEXT_LIST_H
