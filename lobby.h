#ifndef LOBBY_H
#define LOBBY_H

#include "console.h"
#include "button.h"
#include "text_input_box.h"
#include "decal.h"
#include "text_list.h"


enum goals
{
    goal_monopoly=0,
    goal_buyout,
    goal_time,
    goal_money
};

class lobby
{
    public:
        //Constructors
        lobby();
        //Variabels
        bool m_ready;
        //Functions
        bool init(int tex_font[],int tex_buttons,int tex_decals,int window_width,int window_height,sound* pSoundEngine);
        bool set_server_flag(bool flag);
        bool add_player(string name);
        bool remove_player(string name);
        bool text_to_console(string sender,string text_message);
        int  update(bool keys[],int mouse_pos[],bool mouse_but[],float time,string& message_to_send);
        bool draw(void);
        int  get_game_mode(void);
        bool set_game_mode(int game_mode);
        int  get_game_goal(void);
        bool set_game_goal(int game_mode,int goal);

    private:
        //Variabels
        int            m_window_width,m_window_height;
        bool           m_server_flag;
        float          m_LMB_delay,m_last_time;
        int            m_game_mode;
        int            m_tex_font[3];
        int            m_tex_buttons,m_tex_decals;
        decal          m_decGoal_monopoly,m_decGoal_buyout,m_decGoal_time,m_decGoal_money;
        text_input_box m_tibGoal_time,m_tibGoal_money;
        console        m_Console;
        text_list      m_Player_list;
        button         m_butLeave,m_butReady,m_butGoal_monopoly,m_butGoal_buyout,m_butGoal_time,m_butGoal_money;
        //Functions
};

#endif // LOBBY_H
