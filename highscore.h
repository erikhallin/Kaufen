#ifndef HIGHSCORE_H
#define HIGHSCORE_H

#include "decal.h"
#include "display.h"
#include "button.h"
#include <string>
#include <vector>

#define _click_delay 0.2

/*

beh�ver  display f�r varje spelare * antal v�rden
knappar f�r sorteringslista

struct f�r spelare
inneh�ller namn,vektor med v�rden

sortering:
sort val = namn eller v�rde x
vector med index val
int vector lista[i] ger placering
bubble sort: testsa 2 v�rden om fel ordning byt plats

knappar f�r att v�lja sortering

knappar f�r att k�pa spelare

*/

using namespace std;

struct player
{
    string        name;
    vector<float> vValues;
};

class highscore
{
    public:
        //Constructors
        highscore();
        highscore(int x_pos,int y_pos,int width,int height,int numof_players,int numof_values,int font_texture[3],int texture,
                  float tex_offset_x_min,float tex_offset_x_max,float tex_offset_y_min,float tex_offset_y_max);
        //Variables
        bool m_ready;
        //Functions
        bool update_values_for_player( string player_name, float* values );
        bool new_player(string name,float money_value,float value_sum);
        bool remove_player(string name);
        int  interaction_test(int mouse_pos[],bool mouse_but[],float time,string& bought_name);
        bool draw(void);

    private:
        //Variables
        bool   m_sort_by_name;

        int    m_x_pos,m_y_pos,m_width,m_height,m_texture;
        int    m_numof_players,m_numof_values,m_sort_value;
        int    m_font_textures[3];

        float  m_x_offset_min,m_y_offset_min,m_x_offset_max,m_y_offset_max;
        float  m_time_last_cycle,m_LMB_delay;

        button m_buSort_name,m_buSort_val1,m_buSort_val2;
        decal  m_deBackground;
        vector<int>      m_vQueue_list; //give index (first is first position) and will output player index
        vector<display>  m_vDisplays_names;//have constant index value
        vector<display>  m_vDisplays_values;//values belonging to player x = num of values * x
        vector<player>   m_vPlayers;
        vector<button>   m_vBuyoutButtons;
        //Functions
        bool   sort_list(void);
};

#endif // HIGHSCORE_H
