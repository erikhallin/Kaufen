#ifndef MERC_H
#define MERC_H

#include "button.h"
#include "display.h"
#include "graph.h"
#include "decal.h"
#include "noise_generator.h"
#include "sound.h"
#include <string>

#include <gl\gl.h> //temp XXXXXXXXXX

using namespace std;

/*
ska innehålla:
9 knappar: köp, sälj, visa graph, 6x ändra antal enheter
1 graph: visar värde
4 displays: antal enheter kvar, enhetspris, antal markerade enheter, pris för markerade enheter
1 bild: vara/status

construktor behöver:
skärm koord, bredd, höjd, namn/typ (unit texture), perlin noise info....

skärm koord för innehåll i merc ställs in relativt

*/

class merc
{
    public:
        //Constructors
        merc();
        merc(int x_pos,int y_pos,int width,int height,int number_of_units,float price_init,float noise_settings[5],
             int button_texture,int unique_texture,
             float x_tex_offset_min,float x_tex_offset_max,float y_tex_offset_min,float y_tex_offset_max,
             int font_texture[3],sound* pSoundEngine,string name);
        //Variables
        bool m_ready;
        //Functions
        bool  interaction_test(int mouse_pos[],bool mouse_but[],float time,float sale_data[2]);
        //bool  update(int mouse_pos[],bool mouse_but[],float time,float sale_data[3],vector<float>& vInvUnitsCount);//OLD
        bool  draw(void);
        bool  set_price(float price,float time);
        float get_price(void);
        float get_price_init(void);
        int   get_unit_counter(void);
        bool  set_unit_counter(int change);
        bool  refresh_unit_counter(int counter);
        bool  set_unit_counter_init(int new_value);
        int   get_unit_counter_init(void);
        bool  set_price_init(float price_init);
        bool  update_price(float time);
        string get_name(void);
        //bool  clear(void);

    private:
        //Variables
        bool            m_empty;
        string          m_name;
        bool            m_show_graph;
        int             m_x_pos,m_y_pos,m_width,m_height;
        float           m_price_init,m_price_curr;
        float           m_update_res,m_update_next_time,m_time_last_cycle,m_buy_delay;
        int             m_unit_count_init,m_unit_count,m_marked_unit_count;
        float           m_marked_unit_price;
        button          m_bBuy,m_bSell,m_bShow_graph,m_bDecrement_1,m_bDecrement_10,m_bDecrement_100,m_bIncrement_1,m_bIncrement_10,m_bIncrement_100;
        graph           m_gPrice_graph;
        display         m_diUnits_left,m_diUnit_price,m_diNumber_of_marked_units,m_diPrice_for_marked_units;
        decal           m_deType_status;//shift y-2 to draw next status
        int             m_status;//0=error, 1=units left, 2=finished
        noise_generator m_noise_gen,m_noise_gen_sec;
        float           m_LMB_delay;
        float           m_decal_texture_shift;

        sound*          m_pSoundEngine;


        //Functions
        float get_gen_price(float time);

};

#endif // MERC_H
