#ifndef WIN_WINDOW_H
#define WIN_WINDOW_H

#include <vector>
#include <stdlib.h>
#include "button.h"
#include "decal.h"
#include "display.h"
#include "coin_anim.h"


class win_window
{
    public:
        //Constructors
        win_window();
        //Variables
        bool m_ready;
        //Functions
        bool init(int window_width,int window_height,int texture_buttons,int texture_font[3],int texture_gameover);
        bool set_winner_name(string name);
        int  update(bool keys[],int mouse_pos[],bool mouse_but[],float time);
        bool draw(void);

    private:
        //Variables
        int      m_numof_coins;
        float    m_last_time,m_LMB_delay;
        int      m_texture;
        decal    m_deBackground;
        button   m_buRestart;
        display  m_diName;

        vector<coin_anim> m_coCoins;
        //Functions
};

#endif // WIN_WINDOW_H
