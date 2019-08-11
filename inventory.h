#ifndef INVENTORY_H
#define INVENTORY_H

#include <vector>
#include "display.h"
#include "decal.h"

#define _numof_mercs 16

/*

ska visa:
decal på alla merc...ok
antal enheter man har för varje merc...ok
hur mkt pengar man har...ok
hur mkt värde för alla merc med nuvarande pris...ok



*/

struct sale
{
    int   merc_id;
    int   unit_count;
    float price_per_unit;
};

class inventory
{
    public:
        //Constructors
        inventory();
        inventory(int x_pos,int y_pos,int width,int height,int number_of_mercs,int decal_texture,int font_texture[3]);
        //Variables
        bool m_ready;
        //Functions
        bool get_unit_count(vector<float>& vInvUnitsCount);
        bool update_price(int unit_type,float prices);
        bool update_unit_count(int unit_type,float unit_count_diff);
        bool refresh_unit_count(int unit_type,float counter);
        bool update_wallet(float money,float value_sum);
        bool update(vector<float>& vPrices,vector<sale>& vSales,float money_count);
        bool draw(void);

    private:
        //Variables
        int             m_x_pos,m_y_pos,m_width,m_height;
        int             m_number_of_mercs;
        int             m_decal_texture,m_font_textures[3];
        int             m_unit_counters[_numof_mercs];
        float           m_unit_prices[_numof_mercs];
        display         m_diMoney_count,m_diValue_sum;
        decal           m_deFunds,m_deValue;

        vector<display> m_vDisplays_units;
        vector<display> m_vDisplays_price;
        vector<decal>   m_vDecals;
        //Functions

};

#endif // INVENTORY_H
