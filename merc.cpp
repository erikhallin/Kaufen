#include "merc.h"

#define _click_delay 0.2
#define _buy_delay   3.0 //3.0

merc::merc()
{
    //ctor
}

merc::merc(int x_pos,int y_pos,int width,int height,int number_of_units,float price_init,float noise_settings[5],
           int button_texture,int unique_texture,
           float x_tex_offset_min,float x_tex_offset_max,float y_tex_offset_min,float y_tex_offset_max,
           int font_texture[3],sound* pSoundEngine,string name)
{
    m_LMB_delay=0;

    m_name=name;
    m_x_pos=x_pos;
    m_y_pos=y_pos;
    m_width=width;
    m_height=height;
    m_unit_count_init=m_unit_count=number_of_units;

    if(number_of_units==0) m_empty=true;
    else m_empty=false;

    m_price_init=price_init;
    m_marked_unit_count=0;
    m_buy_delay=_buy_delay;

    m_pSoundEngine=pSoundEngine;

    //set noise generator
    m_noise_gen.set_settings(noise_settings[0],noise_settings[1],noise_settings[2],(int)noise_settings[3],(int)noise_settings[4]);
    m_noise_gen_sec.set_settings(noise_settings[0],noise_settings[1]*100,noise_settings[2]/10,(int)noise_settings[3],(int)noise_settings[4]);

    //relative coord for contents
    float sell_x_min=0.0067; float sell_x_max=0.2667;
    float sell_y_min=0.69; float sell_y_max=0.98;
    float buy_x_min=0.7267; float buy_x_max=0.9867;
    float buy_y_min=0.69; float buy_y_max=0.98;
    float show_graph_x_min=0.7267; float show_graph_x_max=0.9867;
    float show_graph_y_min=0.01; float show_graph_y_max=0.30;
    float dec_1_x_min=0.2467; float dec_1_x_max=0.3467;
    float dec_1_y_min=0.43; float dec_1_y_max=0.57;
    float dec_10_x_min=0.1333; float dec_10_x_max=0.2333;
    float dec_10_y_min=0.43; float dec_10_y_max=0.57;
    float dec_100_x_min=0.02; float dec_100_x_max=0.12;
    float dec_100_y_min=0.43; float dec_100_y_max=0.57;
    float inc_1_x_min=0.6533; float inc_1_x_max=0.7533;
    float inc_1_y_min=0.43; float inc_1_y_max=0.57;
    float inc_10_x_min=0.7667; float inc_10_x_max=0.8667;
    float inc_10_y_min=0.43; float inc_10_y_max=0.57;
    float inc_100_x_min=0.88; float inc_100_x_max=0.98;
    float inc_100_y_min=0.43; float inc_100_y_max=0.57;


    //create and place buttons
    float tex_offset_x_min,tex_offset_x_max,tex_offset_y_min,tex_offset_y_max;
    //buy
    tex_offset_x_min=150; tex_offset_x_max=188; tex_offset_y_min=0; tex_offset_y_max=28;
    m_bBuy=button(int(x_pos+buy_x_min*(float)width),int(y_pos+buy_y_min*(float)height),
                  int((buy_x_max-buy_x_min)*(float)width),int((buy_y_max-buy_y_min)*(float)height),
                  button_texture,tex_offset_x_min,tex_offset_x_max,tex_offset_y_min,tex_offset_y_max);
    //sell
    tex_offset_x_min=189; tex_offset_x_max=227; tex_offset_y_min=0; tex_offset_y_max=28;
    m_bSell=button(x_pos+sell_x_min*(float)width,y_pos+sell_y_min*(float)height,
                   int((sell_x_max-sell_x_min)*(float)width),int((sell_y_max-sell_y_min)*(float)height),
                   button_texture,tex_offset_x_min,tex_offset_x_max,tex_offset_y_min,tex_offset_y_max);
    //show graph
    tex_offset_x_min=228; tex_offset_x_max=266; tex_offset_y_min=0; tex_offset_y_max=28;
    m_bShow_graph=button(x_pos+show_graph_x_min*(float)width,y_pos+show_graph_y_min*(float)height,
                         int((show_graph_x_max-show_graph_x_min)*(float)width),int((show_graph_y_max-show_graph_y_min)*(float)height),
                         button_texture,tex_offset_x_min,tex_offset_x_max,tex_offset_y_min,tex_offset_y_max);
    m_bShow_graph.set_toggle(true);

    //dec 1
    tex_offset_x_min=267; tex_offset_x_max=280; tex_offset_y_min=0; tex_offset_y_max=13;
    m_bDecrement_1=button(x_pos+dec_1_x_min*(float)width,y_pos+dec_1_y_min*(float)height,
                          int((dec_1_x_max-dec_1_x_min)*(float)width),int((dec_1_y_max-dec_1_y_min)*(float)height),
                          button_texture,tex_offset_x_min,tex_offset_x_max,tex_offset_y_min,tex_offset_y_max);
    //dec 10
    tex_offset_x_min=281; tex_offset_x_max=294; tex_offset_y_min=0; tex_offset_y_max=13;
    m_bDecrement_10=button(x_pos+dec_10_x_min*(float)width,y_pos+dec_10_y_min*(float)height,
                           int((dec_10_x_max-dec_10_x_min)*(float)width),int((dec_10_y_max-dec_10_y_min)*(float)height),
                           button_texture,tex_offset_x_min,tex_offset_x_max,tex_offset_y_min,tex_offset_y_max);
    //dec 100
    tex_offset_x_min=295; tex_offset_x_max=308; tex_offset_y_min=0; tex_offset_y_max=13;
    m_bDecrement_100=button(x_pos+dec_100_x_min*(float)width,y_pos+dec_100_y_min*(float)height,
                            int((dec_100_x_max-dec_100_x_min)*(float)width),int((dec_100_y_max-dec_100_y_min)*(float)height),
                            button_texture,tex_offset_x_min,tex_offset_x_max,tex_offset_y_min,tex_offset_y_max);
    //inc 1
    tex_offset_x_min=309; tex_offset_x_max=322; tex_offset_y_min=0; tex_offset_y_max=13;
    m_bIncrement_1=button(x_pos+inc_1_x_min*(float)width,y_pos+inc_1_y_min*(float)height,
                          int((inc_1_x_max-inc_1_x_min)*(float)width),int((inc_1_y_max-inc_1_y_min)*(float)height),
                          button_texture,tex_offset_x_min,tex_offset_x_max,tex_offset_y_min,tex_offset_y_max);
    //inc 10
    tex_offset_x_min=323; tex_offset_x_max=336; tex_offset_y_min=0; tex_offset_y_max=13;
    m_bIncrement_10=button(x_pos+inc_10_x_min*(float)width,y_pos+inc_10_y_min*(float)height,
                           int((inc_10_x_max-inc_10_x_min)*(float)width),int((inc_10_y_max-inc_10_y_min)*(float)height),
                           button_texture,tex_offset_x_min,tex_offset_x_max,tex_offset_y_min,tex_offset_y_max);
    //inc 100
    tex_offset_x_min=337; tex_offset_x_max=350; tex_offset_y_min=0; tex_offset_y_max=13;
    m_bIncrement_100=button(x_pos+inc_100_x_min*(float)width,y_pos+inc_100_y_min*(float)height,
                            int((inc_100_x_max-inc_100_x_min)*(float)width),int((inc_100_y_max-inc_100_y_min)*(float)height),
                            button_texture,tex_offset_x_min,tex_offset_x_max,tex_offset_y_min,tex_offset_y_max);

    //graph
    m_gPrice_graph=graph(m_x_pos,m_y_pos,m_width,m_height,60.0f,0.0f,m_price_init*2);
    m_show_graph=false;

    //displays
    int d_x_pos,d_y_pos,d_width,d_height;
    d_x_pos=x_pos+float(width)*0.293;   d_y_pos=y_pos+float(height)*0.05;
    d_width=float(width)*0.193;         d_height=float(height)*0.190;
    m_diUnits_left=display(d_x_pos,d_y_pos,d_width,d_height,10,font_texture,number_of_units);

    d_x_pos=x_pos+float(width)*0.507;   d_y_pos=y_pos+float(height)*0.05;
    d_width=float(width)*0.193;         d_height=float(height)*0.190;
    m_diUnit_price=display(d_x_pos,d_y_pos,d_width,d_height,10,font_texture,price_init);

    d_x_pos=x_pos+float(width)*0.367;   d_y_pos=y_pos+float(height)*0.38;
    d_width=float(width)*0.260; d_height=float(height)*0.24;
    m_diNumber_of_marked_units=display(d_x_pos,d_y_pos,d_width,d_height,10,font_texture,0);

    d_x_pos=x_pos+float(width)*0.367;   d_y_pos=y_pos+float(height)*0.71;
    d_width=float(width)*0.260;         d_height=float(height)*0.24;
    m_diPrice_for_marked_units=display(d_x_pos,d_y_pos,d_width,d_height,10,font_texture,0);

    //decal
    m_decal_texture_shift=y_tex_offset_max-y_tex_offset_min;
    m_deType_status=decal(x_pos+float(width)*0.007,y_pos+float(height)*0.01,
                          float(width)*0.26,float(height)*0.29,
                          unique_texture,x_tex_offset_min,x_tex_offset_max,y_tex_offset_min,y_tex_offset_max);

    m_update_res=0.2;// 0.2 sec per update
    m_update_next_time=0;
}

bool merc::interaction_test(int mouse_pos[],bool mouse_but[],float time,float sale_data[2])//true if sale
{
    //timing
    if(m_LMB_delay>0) m_LMB_delay-=time-m_time_last_cycle;
    if(m_buy_delay>0) m_buy_delay-=time-m_time_last_cycle;
    m_time_last_cycle=time;

    int mouse_x=mouse_pos[0];
    int mouse_y=mouse_pos[1];
    //check mouse over buttons
    if(m_bShow_graph.overlap_test(mouse_x,mouse_y))
    {
        if(mouse_but[0]&&m_LMB_delay<=0)
        {
            //play sound
            m_pSoundEngine->playSimpleSound(wav_button_press,1.0f);

            if(m_show_graph)//untoogle
            {
                m_show_graph=false;
                m_bShow_graph.m_state=2;
                m_gPrice_graph.set_status(1);
                m_gPrice_graph.set_view_x(false,60);//set static x view
            }
            else//toogle
            {
                m_show_graph=true;
                m_bShow_graph.m_state=2;
                m_gPrice_graph.set_status(3);//draw thick line
                m_gPrice_graph.set_view_x(true);//set dyn x view
            }

            m_LMB_delay=_click_delay;
        }
        else//toggle button
        {
            if(!m_show_graph)//hoover
            {
                m_bShow_graph.m_state=1;
                m_gPrice_graph.set_status(2);
            }
        }
    }
    else //outside button
    {
        if(!m_show_graph)
        {
            m_bShow_graph.m_state=0;
            m_gPrice_graph.set_status(1);
        }
    }

    if(m_show_graph) return false;//dont test other button (they are not shown)

    if(m_bBuy.overlap_test(mouse_x,mouse_y))
    {
        if(mouse_but[0]&&m_LMB_delay<=0)
        {//send sale request
            m_bBuy.m_state=2;
            m_LMB_delay=_click_delay;

            if(m_buy_delay>0)//not ready to buy jet
            {
                sale_data[0]=m_buy_delay;
                sale_data[1]=0;
                return true;
            }

            sale_data[1]=m_marked_unit_count; //[0] is type, [1] is number of units
            m_buy_delay=_buy_delay;
            return true;
        }
        else
        {
            if(mouse_but[0]) //waiting for delay
            {
                m_bBuy.m_state=2;
            }
            else m_bBuy.m_state=1; //just hoovering
        }
    }
    if(m_bSell.overlap_test(mouse_x,mouse_y))
    {
        if(mouse_but[0]&&m_LMB_delay<=0)
        {//send sale request
            m_bSell.m_state=2;
            m_LMB_delay=_click_delay;

            if(m_buy_delay>0)//not ready to buy jet
            {
                sale_data[0]=m_buy_delay;
                sale_data[1]=0;
                return true;
            }

            sale_data[1]=-m_marked_unit_count; //[0] is type, [1] is number of units
            m_buy_delay=_buy_delay;
            return true;
        }
        else
        {
            if(mouse_but[0]) //waiting for delay
            {
                m_bSell.m_state=2;
            }
            else m_bSell.m_state=1; //just hoovering
        }
    }


    if(m_bDecrement_1.overlap_test(mouse_x,mouse_y))
    {
        if(mouse_but[0]&&m_LMB_delay<=0)
        {
            //play sound
            m_pSoundEngine->playSimpleSound(wav_button_press,1.0f);

            m_bDecrement_1.m_state=2;
            m_LMB_delay=_click_delay;
            m_marked_unit_count-=1;
            if(m_marked_unit_count<0) m_marked_unit_count=0;
            m_diNumber_of_marked_units.set_value(m_marked_unit_count);
            //calc price
            float price=float(m_marked_unit_count)*m_price_curr;
            m_diPrice_for_marked_units.set_value(price);
        }
        else
        {
            if(mouse_but[0]) //waiting for delay
            {
                m_bDecrement_1.m_state=2;
            }
            else m_bDecrement_1.m_state=1; //just hoovering
        }
    }
    if(m_bDecrement_10.overlap_test(mouse_x,mouse_y))
    {
        if(mouse_but[0]&&m_LMB_delay<=0)
        {
            //play sound
            m_pSoundEngine->playSimpleSound(wav_button_press,1.0f);

            m_bDecrement_10.m_state=2;
            m_LMB_delay=_click_delay;
            m_marked_unit_count-=10;
            if(m_marked_unit_count<0) m_marked_unit_count=0;
            m_diNumber_of_marked_units.set_value(m_marked_unit_count);
            //calc price
            float price=float(m_marked_unit_count)*m_price_curr;
            m_diPrice_for_marked_units.set_value(price);
        }
        else
        {
            if(mouse_but[0]) //waiting for delay
            {
                m_bDecrement_10.m_state=2;
            }
            else m_bDecrement_10.m_state=1; //just hoovering
        }
    }
    if(m_bDecrement_100.overlap_test(mouse_x,mouse_y))
    {
        if(mouse_but[0]&&m_LMB_delay<=0)
        {
            //play sound
            m_pSoundEngine->playSimpleSound(wav_button_press,1.0f);

            m_bDecrement_100.m_state=2;
            m_LMB_delay=_click_delay;
            m_marked_unit_count-=100;
            if(m_marked_unit_count<0) m_marked_unit_count=0;
            m_diNumber_of_marked_units.set_value(m_marked_unit_count);
            //calc price
            float price=float(m_marked_unit_count)*m_price_curr;
            m_diPrice_for_marked_units.set_value(price);
        }
        else
        {
            if(mouse_but[0]) //waiting for delay
            {
                m_bDecrement_100.m_state=2;
            }
            else m_bDecrement_100.m_state=1; //just hoovering
        }
    }
    if(m_bIncrement_1.overlap_test(mouse_x,mouse_y))
    {
        if(mouse_but[0]&&m_LMB_delay<=0)
        {
            //play sound
            m_pSoundEngine->playSimpleSound(wav_button_press,1.0f);

            m_bIncrement_1.m_state=2;
            m_LMB_delay=_click_delay;
            m_marked_unit_count+=1;
            m_diNumber_of_marked_units.set_value(m_marked_unit_count);
            //calc price
            float price=float(m_marked_unit_count)*m_price_curr;
            m_diPrice_for_marked_units.set_value(price);
        }
        else
        {
            if(mouse_but[0]) //waiting for delay
            {
                m_bIncrement_1.m_state=2;
            }
            else m_bIncrement_1.m_state=1; //just hoovering
        }
    }
    if(m_bIncrement_10.overlap_test(mouse_x,mouse_y))
    {
        if(mouse_but[0]&&m_LMB_delay<=0)
        {
            //play sound
            m_pSoundEngine->playSimpleSound(wav_button_press,1.0f);

            m_bIncrement_10.m_state=2;
            m_LMB_delay=_click_delay;
            m_marked_unit_count+=10;
            m_diNumber_of_marked_units.set_value(m_marked_unit_count);
            //calc price
            float price=float(m_marked_unit_count)*m_price_curr;
            m_diPrice_for_marked_units.set_value(price);
        }
        else
        {
            if(mouse_but[0]) //waiting for delay
            {
                m_bIncrement_10.m_state=2;
            }
            else m_bIncrement_10.m_state=1; //just hoovering
        }
    }
    if(m_bIncrement_100.overlap_test(mouse_x,mouse_y))
    {
        if(mouse_but[0]&&m_LMB_delay<=0)
        {
            //play sound
            m_pSoundEngine->playSimpleSound(wav_button_press,1.0f);

            m_bIncrement_100.m_state=2;
            m_LMB_delay=_click_delay;
            m_marked_unit_count+=100;
            m_diNumber_of_marked_units.set_value(m_marked_unit_count);
            //calc price
            float price=float(m_marked_unit_count)*m_price_curr;
            m_diPrice_for_marked_units.set_value(price);
        }
        else
        {
            if(mouse_but[0]) //waiting for delay
            {
                m_bIncrement_100.m_state=2;
            }
            else m_bIncrement_100.m_state=1; //just hoovering
        }
    }

    /*//TEMPZZZZZZZZ
    //permasale
    sale_data[1]=m_marked_unit_count; //[0] is type, [1] is number of units
    return true;
    //XXXXXXXXXXXX*/

    return false;
}

bool merc::draw(void)
{
    /*//draw frame....
    glColor3f(1,1,1);
    glBegin(GL_LINE_STRIP);
    glVertex2f(m_x_pos,m_y_pos);
    glVertex2f(m_x_pos,m_y_pos+m_height);
    glVertex2f(m_x_pos+m_width,m_y_pos+m_height);
    glVertex2f(m_x_pos+m_width,m_y_pos);
    glVertex2f(m_x_pos,m_y_pos);
    glEnd();*/

    m_gPrice_graph.draw_graph();
    m_bShow_graph.draw_button();
    if(!m_show_graph)
    {
        m_bBuy.draw_button();
        m_bSell.draw_button();
        m_bDecrement_1.draw_button();
        m_bDecrement_10.draw_button();
        m_bDecrement_100.draw_button();
        m_bIncrement_1.draw_button();
        m_bIncrement_10.draw_button();
        m_bIncrement_100.draw_button();
        m_diUnits_left.draw_display();
        m_diUnit_price.draw_display();
        m_diNumber_of_marked_units.draw_display();
        m_diPrice_for_marked_units.draw_display();
    }
    m_deType_status.draw();

    return true;
}

bool merc::set_price(float price,float time)//not used?
{
    m_price_curr=price;

    //update graph
    m_gPrice_graph.add_point(time,price);

    //update displays
    //unit price
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
    m_diUnit_price.set_value(shown_price);
    //marked price
    price=price*float(m_marked_unit_count);
    shown_price=(int)price;//remove decimals
    /*
    price=price*float(m_marked_unit_count);
    if(price<10)
    {
        if(price>9.99) shown_price=int(price*10)/10; //1 decimal
        else shown_price=int(price*100)/100; //2 decimals
    }
    else shown_price=(int)price;//no decimals
    */
    m_diPrice_for_marked_units.set_value(shown_price);

    return true;
}

float merc::get_price(void)
{
    return m_price_curr;
}

float merc::get_price_init(void)
{
    return m_price_init;
}

int merc::get_unit_counter(void)
{
    return m_unit_count;
}

bool merc::set_unit_counter(int change)
{
    //change texture test
    if( m_unit_count-change <= 0 && !m_empty )
    {
        m_deType_status.shift_texture_offset(0,2);//now empty
        m_empty=true;
    }
    else if( m_unit_count<=0 && change<0 && m_empty )
    {
        m_deType_status.shift_texture_offset(0,-2);//now have something
        m_empty=false;
    }

    m_unit_count-=change;
    m_diUnits_left.set_value(m_unit_count);

    return true;
}

bool merc::refresh_unit_counter(int counter)
{
    //change texture test
    if( counter == 0 )
    {
        if(!m_empty)
        {
            m_deType_status.shift_texture_offset(0,2);//now empty
            m_empty=true;
        }
    }
    else
    {
        if(m_empty)
        {
            m_deType_status.shift_texture_offset(0,-2);//now have something
            m_empty=false;
        }
    }

    m_unit_count=counter;
    m_diUnits_left.set_value(m_unit_count);

    return true;
}

bool merc::set_unit_counter_init(int new_value)
{
    m_unit_count=new_value;
    m_diUnits_left.set_value(new_value);

    //change texture test
    if( new_value == 0 )
    {
        m_deType_status.shift_texture_offset(0,2);//now empty
    }

    return true;
}

int merc::get_unit_counter_init(void)
{
    return m_unit_count_init;
}

bool merc::set_price_init(float price_init)
{
    //update graph window
    m_price_init=price_init;
    m_gPrice_graph.set_view_y( false, 0, price_init*2 );

    return true;
}

bool merc::update_price(float time)//for server
{
    float price=m_price_curr=m_noise_gen.get_noise(time) + m_noise_gen_sec.get_noise(time);

    //update graph
    m_gPrice_graph.add_point(time,price);

    //update displays
    //unit price
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
    m_diUnit_price.set_value(shown_price);
    //marked price
    price=price*float(m_marked_unit_count);
    shown_price=(int)price;//remove decimals
    /*
    price=price*float(m_marked_unit_count);
    if(price<10)
    {
        if(price>9.99) shown_price=int(price*10)/10; //1 decimal
        else shown_price=int(price*100)/100; //2 decimals
    }
    else shown_price=(int)price;//no decimals
    */
    m_diPrice_for_marked_units.set_value(shown_price);

    return true;
}

string merc::get_name(void)
{
    return m_name;
}




float merc::get_gen_price(float time)
{
    return m_noise_gen.get_noise(time) + m_noise_gen_sec.get_noise(time);
}
