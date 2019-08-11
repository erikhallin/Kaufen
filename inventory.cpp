#include "inventory.h"

inventory::inventory()
{
    m_ready=false;
}

inventory::inventory(int x_pos,int y_pos,int width,int height,int number_of_mercs,int decal_texture,int font_texture[3])
{
    m_x_pos=x_pos;
    m_y_pos=y_pos;
    m_width=width;
    m_height=height;
    m_number_of_mercs=number_of_mercs;
    m_decal_texture=decal_texture;
    m_font_textures[0]=font_texture[0];
    m_font_textures[1]=font_texture[1];
    m_font_textures[2]=font_texture[2];

    //clear arrays
    for(int i=0;i<_numof_mercs;i++)
    {
        m_unit_counters[i]=0;
        m_unit_prices[i]=0;
    }

    //create decals & displays
    for(int i=0;i<m_number_of_mercs;i++)
    {
        float x_init=0.01; float y_init=0.01;
        float x_offset_row1=0.3; float x_offset_row2=0.65; float y_offset=0.055;
        float de_width=0.216; float de_height=0.05;
        float di_width=0.3; float di_height=0.05;
        float tex_x_offset=40; //int tex_y_offset=0; alla tex på en rad änsålänge

        m_vDecals.push_back(decal( int(x_pos+float(width)*(x_init)), int(y_pos+float(height)*(y_init+y_offset*(float)i)),
                                   int(de_width*width), int(de_height*height),
                                   decal_texture,
                                   tex_x_offset*float(i), tex_x_offset*float(i)+39, 0.0f, 28.0f ));

        m_vDisplays_price.push_back(display( int(x_pos+float(width)*(x_init+x_offset_row1)), int(y_pos+float(height)*(y_init+y_offset*(float)i)),
                                             int(di_width*width), int(di_height*height), 10, font_texture, 0.0f));

        m_vDisplays_units.push_back(display( int(x_pos+float(width)*(x_init+x_offset_row2)), int(y_pos+float(height)*(y_init+y_offset*(float)i)),
                                             int(di_width*width), int(di_height*height), 10, font_texture, 0.0f));
    }
    //money counter display
    float di_xpos=0.31; float di_ypos=0.90;
    float di_width=0.64; float di_height=0.05;
    m_diMoney_count=display( int(x_pos+float(width)*(di_xpos)), int(y_pos+float(height)*(di_ypos)),
                             int(di_width*width), int(di_height*height), 10, font_texture, 0.0f);

    //Funds decal
    di_width=0.20; di_height=0.05;
    m_deFunds=decal( int(x_pos+float(width)*(0.01)), int(y_pos+float(height)*(di_ypos)),
                     int(di_width*width), int(di_height*height),
                     decal_texture,
                     0.0f, 49.0f, 120.0f, 149.0f );

    //value display
    di_xpos=0.31; di_ypos=0.96;
    di_width=0.64; di_height=0.05;
    m_diValue_sum=display( int(x_pos+float(width)*(di_xpos)), int(y_pos+float(height)*(di_ypos)),
                           int(di_width*width), int(di_height*height), 10, font_texture, 0.0f);
    //Value decal
    di_width=0.20; di_height=0.05;
    m_deValue=decal( int(x_pos+float(width)*(0.01)), int(y_pos+float(height)*(di_ypos)),
                     int(di_width*width), int(di_height*height),
                     decal_texture,
                     0.0f, 49.0f, 180.0f, 209.0f );

    m_ready=true;
}

bool inventory::get_unit_count(vector<float>& vInvUnitsCount)
{
    for(int i=0;i<(int)m_vDisplays_units.size();i++)
    {
        vInvUnitsCount.push_back(m_vDisplays_units[i].get_value());
    }

    return true;
}

bool inventory::update_price(int unit_type,float price)
{
    if(unit_type<0 || unit_type>_numof_mercs) return false;

    m_unit_prices[unit_type]=price;

    //update display
    float shown_price=(int)price;//remove decimals
    /*
    float shown_price=price;
    if(price<100)
    {
        if(price>9.99) shown_price=float(int(price*10))/10; //1 decimal
        else shown_price=float(int(price*100))/100; //2 decimals
    }
    else shown_price=(int)price;//no decimals
    */
    m_vDisplays_price[unit_type].set_value(shown_price);


    return true;
}

bool inventory::update_unit_count(int unit_type,float unit_count_diff)
{
    if(unit_type<0 || unit_type>_numof_mercs) return false;

    m_unit_counters[unit_type]+=unit_count_diff;
    m_vDisplays_units[unit_type].set_value(m_unit_counters[unit_type]);

    return true;
}

bool inventory::refresh_unit_count(int unit_type,float counter)
{
    if(unit_type<0 || unit_type>_numof_mercs) return false;

    m_unit_counters[unit_type]=counter;
    m_vDisplays_units[unit_type].set_value(m_unit_counters[unit_type]);

    return true;
}

bool inventory::update_wallet(float money,float value_sum)
{
    float shown_money=(int)money;
    float shown_value_sum=(int)value_sum;

    m_diMoney_count.set_value(shown_money);
    m_diValue_sum.set_value(shown_value_sum);

    return true;
}

bool inventory::update(vector<float>& vPrices,vector<sale>& vSales,float money_count)
{
    //check sales
    if(!vSales.empty())
    {
        for(int i=0;i<(int)vSales.size();i++)
        {
            m_vDisplays_units[ vSales[i].merc_id ].add_value(vSales[i].unit_count);
        }
    }
    //merc prices
    for(int i=0;i<(int)m_vDisplays_price.size();i++)
    {
        float price=vPrices[i];
        float shown_price=vPrices[i];
        if(price<100)
        {
            if(price>9.99) shown_price=float(int(price*10))/10; //1 decimal
            else shown_price=float(int(price*100))/100; //2 decimals
        }
        else shown_price=(int)price;//no decimals

        m_vDisplays_price[i].set_value(shown_price);
    }
    //store money count
    m_diMoney_count.set_value(money_count);
    //calc value sum and store
    float value_sum=money_count;
    for(int i=0;i<(int)vPrices.size();i++)
    {
        value_sum+=vPrices[i]*m_vDisplays_units[i].get_value();
    }
    m_diValue_sum.set_value(value_sum);

    return true;
}

bool inventory::draw(void)
{
    if(!m_ready) return false;

    //Decals
    for(int i=0;i<(int)m_vDecals.size();i++)
    {
        m_vDecals[i].draw();
    }
    //Displays
    for(int i=0;i<(int)m_vDisplays_units.size();i++)
    {
        m_vDisplays_units[i].draw_display();
    }
    for(int i=0;i<(int)m_vDisplays_price.size();i++)
    {
        m_vDisplays_price[i].draw_display();
    }
    m_diMoney_count.draw_display();
    m_diValue_sum.draw_display();
    m_deFunds.draw();
    m_deValue.draw();

    return true;
}
