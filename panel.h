#ifndef PANEL_H
#define PANEL_H

#include <iostream> //temp
#include <gl\gl.h>
#include <math.h>
#include "merc.h"
#include "button.h"
#include "graph.h"
#include "display.h"
#include "decal.h"
#include "inventory.h"
#include "console.h"
#include "highscore.h"
#include "sound.h"

#include <vector>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <fstream>
#include <sstream>

#define _key_delay 0.3
#define _numof_mercs 16
#define _sale_delay 0.3 //0.3

using namespace std;

struct st_player_values
{
    string name;
    float  money;
    float  value_sum;
    int    owned_units[_numof_mercs];
    int    score;
};

class panel
{
    public:
        //Constructors
        panel();
        //Variables
        bool  m_ready;
        //Functions
        bool  init(int tex_font[],int tex_buttons,int tex_decals,sound* pSoundEngine);
        //bool  reload_mercs(string merc_data); OLD
        bool  clear_mercs(void);
        float get_price(int unit_type);
        float get_price_init(int unit_type);
        bool  get_prices(float* float_array);
        bool  set_prices(float prices[],float time);
        bool  refresh_unit_counter(int unit_type,int counter);
        int   get_unit_counter(int unit_type);
        bool  set_unit_counter(string customer_name,int unit_type,int change);
        bool  set_owned_unit_counter_for_player(string name, float unit_counter[]);
        bool  set_unit_counter_init(int unit_type,int new_value);
        bool  set_merc_price_init(int unit_type,float price);
        bool  draw_panel(void);
        //int   update(bool keys[],int mouse_pos[],bool mouse_but[],float time);
        int   update2(bool keys[],int mouse_pos[],bool mouse_but[],float time,float sale_data[],string& message_to_send);
        bool  update_prices(float time);
        bool  update_inventory(int unit_type,int unit_counter_change);
        bool  refresh_inventory(int unit_type,int counter);
        bool  update_inventory_wallet(float money,float value_sum);
        bool  update_player_highscore(string player_name, float money, float value_sum);
        bool  add_new_player(string name,float money_value,float value_sum);
        bool  remove_player(string name);
        bool  text_to_console(string sender,string text_message);
        bool  set_win_conditions(int game_mode,int game_goal);
        bool  set_win_conditions_testing_flag(bool flag);
        string get_merc_name(int unit_type);
        bool  set_start_time_sec(float time_at_start);

    private:
        //Variables
        bool  m_textures_ready;
        float m_time,m_time_last_cycle,m_time_at_start,m_sale_delay_timer,m_key_delay;
        float m_next_time;//TEMP
        float m_money_count,m_value_sum;
        int   m_game_mode,m_game_goal;
        bool  m_game_over_testing,m_show_quit_question;

        int   m_tex_font[3];
        int   m_tex_buttons,m_tex_decals;

        sound*          m_pSoundEngine;

        decal           m_decQuitQuestion;
        inventory       m_Inventory;
        console         m_Console;
        highscore       m_Highscore;
        vector<merc>    m_Mercs;
        vector<button>  m_vButtons;
        vector<graph>   m_vGraphs;
        vector<display> m_vDisplays;
        vector<decal>   m_vDecals;

        vector<st_player_values> m_vPlayer_values;

        //Functions
        bool game_over_test(string& winner);
};

#endif // PANEL_H
