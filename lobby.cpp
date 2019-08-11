#include "lobby.h"

#define _click_delay 0.2

lobby::lobby()
{
    m_ready=false;
}

bool lobby::init(int tex_font[],int tex_buttons,int tex_decals,int window_width,int window_height,sound* pSoundEngine)
{
    m_window_width=window_width+170;//add extra for adjustment
    m_window_height=window_height+80;
    //store textures
    m_tex_font[0]=tex_font[0]; m_tex_font[1]=tex_font[1]; m_tex_font[2]=tex_font[2];
    m_tex_buttons=tex_buttons; m_tex_decals=tex_decals;

    //Buttons
    float x_pos=0; float y_pos=0; float width=m_window_width; float height=m_window_height;
    float but_y_start=0.59; float but_x_start=0.505;
    float but_width=0.125; float but_height=0.06;
    //ready
    m_butReady=button( int(x_pos+float(width)*(but_x_start)), int(y_pos+float(height)*(but_y_start) ),
                       int((but_width)*(float)width), int(but_height*(float)height), tex_buttons,
                       600, 719, 87, 130);
    //leave
    but_y_start=0.59; but_x_start=0.025;
    but_width=0.125; but_height=0.06;
    m_butLeave=button( int(x_pos+float(width)*(but_x_start)), int(y_pos+float(height)*(but_y_start) ),
                       int((but_width)*(float)width), int(but_height*(float)height), tex_buttons,
                       480, 599, 87, 130);
    //goal monopoly
    but_y_start=0.08; but_x_start=0.025;
    but_width=0.125; but_height=0.06;
    m_butGoal_monopoly=button( int(x_pos+float(width)*(but_x_start)), int(y_pos+float(height)*(but_y_start) ),
                       int((but_width)*(float)width), int(but_height*(float)height), tex_buttons,
                       0, 119, 87, 130);
    //goal buyout
    but_y_start=0.08; but_x_start=0.185;
    but_width=0.125; but_height=0.06;
    m_butGoal_buyout=button( int(x_pos+float(width)*(but_x_start)), int(y_pos+float(height)*(but_y_start) ),
                       int((but_width)*(float)width), int(but_height*(float)height), tex_buttons,
                       120, 239, 87, 130);
    //goal time
    but_y_start=0.08; but_x_start=0.345;
    but_width=0.125; but_height=0.06;
    m_butGoal_time=button( int(x_pos+float(width)*(but_x_start)), int(y_pos+float(height)*(but_y_start) ),
                       int((but_width)*(float)width), int(but_height*(float)height), tex_buttons,
                       240, 359, 87, 130);
    //goal money
    but_y_start=0.08; but_x_start=0.505;
    but_width=0.125; but_height=0.06;
    m_butGoal_money=button( int(x_pos+float(width)*(but_x_start)), int(y_pos+float(height)*(but_y_start) ),
                       int((but_width)*(float)width), int(but_height*(float)height), tex_buttons,
                       360, 479, 87, 130);

    //Decals
    float dec_y_start=0.23; float dec_x_start=0.025;
    float dec_width=0.125; float dec_height=0.29;
    //monopoly
    m_decGoal_monopoly=decal( int(x_pos+float(width)*(dec_x_start)), int(y_pos+float(height)*(dec_y_start) ),
                              int((dec_width)*(float)width), int(dec_height*(float)height), tex_buttons,
                              0, 119, 219, 427);
    m_decGoal_monopoly.masking(false);
    //buyout
    dec_y_start=0.23; dec_x_start=0.185;
    dec_width=0.125;   dec_height=0.29;
    m_decGoal_buyout=decal( int(x_pos+float(width)*(dec_x_start)), int(y_pos+float(height)*(dec_y_start) ),
                              int((dec_width)*(float)width), int(dec_height*(float)height), tex_buttons,
                              120, 239, 219, 427);
    m_decGoal_buyout.masking(false);
    //time
    dec_y_start=0.23; dec_x_start=0.345;
    dec_width=0.125;   dec_height=0.29;
    m_decGoal_time=decal( int(x_pos+float(width)*(dec_x_start)), int(y_pos+float(height)*(dec_y_start) ),
                              int((dec_width)*(float)width), int(dec_height*(float)height), tex_buttons,
                              240, 359, 219, 427);
    m_decGoal_time.masking(false);
    //money
    dec_y_start=0.23; dec_x_start=0.505;
    dec_width=0.125;   dec_height=0.29;
    m_decGoal_money=decal( int(x_pos+float(width)*(dec_x_start)), int(y_pos+float(height)*(dec_y_start) ),
                              int((dec_width)*(float)width), int(dec_height*(float)height), tex_buttons,
                              360, 479, 219, 427);
    m_decGoal_money.masking(false);

    //Text inbut boxes
    float tib_y_start=0.16; float tib_x_start=0.35;
    float tib_width=0.12; float tib_height=0.04;
    //time
    m_tibGoal_time=text_input_box( int(x_pos+float(width)*(tib_x_start)), int(y_pos+float(height)*(tib_y_start) ),
                                   int((tib_width)*(float)width), int(tib_height*(float)height), 6, tex_font, " SET " );
    m_tibGoal_time.set_only_number(true);
    //money
    tib_y_start=0.16; tib_x_start=0.51;
    tib_width=0.12;   tib_height=0.04;
    m_tibGoal_money=text_input_box( int(x_pos+float(width)*(tib_x_start)), int(y_pos+float(height)*(tib_y_start) ),
                                    int((tib_width)*(float)width), int(tib_height*(float)height), 6, tex_font, " SET " );
    m_tibGoal_money.set_only_number(true);


    //Console
    m_Console=console(10,480,630,150,60,6,tex_font,tex_decals,0,0,630,150,pSoundEngine);

    //Player list
    m_Player_list=text_list(650,40,135,580,0,tex_font,tex_buttons,0,0,135,150);

    m_game_mode=0;
    m_last_time=m_LMB_delay=0;
    m_server_flag=false;
    m_ready=true;

    return true;
}

bool lobby::set_server_flag(bool flag)
{
    m_server_flag=flag;
    //change ready button to start texture
    float x_pos=0; float y_pos=0; float width=m_window_width; float height=m_window_height;
    float but_y_start=0.59; float but_x_start=0.505;
    float but_width=0.125; float but_height=0.06;
    //ready
    m_butReady=button( int(x_pos+float(width)*(but_x_start)), int(y_pos+float(height)*(but_y_start) ),
                       int((but_width)*(float)width), int(but_height*(float)height), m_tex_buttons,
                       720, 839, 87, 130);


    return true;
}

bool lobby::add_player(string name)
{
    return m_Player_list.add_player(name);
}

bool lobby::remove_player(string name)
{
    return m_Player_list.remove_player(name);
}

bool lobby::text_to_console(string sender,string text_message)
{
    bool retVal=false;

    if(sender=="") retVal=m_Console.input_text(text_message);
    else retVal=m_Console.input_text(sender,text_message);

    return retVal;
}

int lobby::update(bool keys[],int mouse_pos[],bool mouse_but[],float time,string& message_to_send)
{
    bool return_value=0;
    //Timing
    if(m_LMB_delay>0)
    {
        m_LMB_delay-=time-m_last_time;
    }

    //Buttons
    //unmark all goals
    /*m_butGoal_monopoly.m_state=0;
    m_butGoal_buyout.m_state=0;
    m_butGoal_time.m_state=0;
    m_butGoal_money.m_state=0;*/

    //ready
    if( m_butReady.overlap_test(mouse_pos[0],mouse_pos[1]) )
    {
        if(mouse_but[0])
        {
            m_butReady.m_state=2;//change button texture
            if(m_LMB_delay<=0)
            {
                m_LMB_delay=_click_delay;

                m_tibGoal_time.unmark();
                m_tibGoal_money.unmark();

                return 3;
            }
        }
        else
        {
            m_butReady.m_state=1;//change button texture
        }
    }
    else m_butReady.m_state=0;//change button texture

    //leave
    if( m_butLeave.overlap_test(mouse_pos[0],mouse_pos[1]) )
    {
        if(mouse_but[0])
        {
            m_butLeave.m_state=2;//change button texture
            if(m_LMB_delay<=0)
            {
                m_LMB_delay=_click_delay;
                //XXXX

                m_tibGoal_time.unmark();
                m_tibGoal_money.unmark();

                return 2;
            }
        }
        else
        {
            m_butLeave.m_state=1;//change button texture
        }
    }
    else m_butLeave.m_state=0;//change button texture

    if(m_server_flag)//only server can change game mode settings
    {
        //monopoly
        if( m_butGoal_monopoly.overlap_test(mouse_pos[0],mouse_pos[1]) )
        {
            if(mouse_but[0])
            {
                m_butGoal_monopoly.m_state=2;//change button texture
                if(m_LMB_delay<=0)
                {
                    m_LMB_delay=_click_delay;
                    m_game_mode=goal_monopoly;
                    m_butGoal_monopoly.set_toggle(true);
                    m_butGoal_buyout.set_toggle(false);
                    m_butGoal_time.set_toggle(false);
                    m_butGoal_money.set_toggle(false);
                    m_tibGoal_time.unmark();
                    m_tibGoal_money.unmark();

                    return_value = 1;
                }
            }
            else
            {
                if(m_butGoal_monopoly.m_state!=2) m_butGoal_monopoly.m_state=1;//change button texture
            }
        }
        else if(m_butGoal_monopoly.m_state!=2) m_butGoal_monopoly.m_state=0;//change button texture

        //buyout
        if( m_butGoal_buyout.overlap_test(mouse_pos[0],mouse_pos[1]) )
        {
            if(mouse_but[0])
            {
                m_butGoal_buyout.m_state=2;//change button texture
                if(m_LMB_delay<=0)
                {
                    m_LMB_delay=_click_delay;
                    m_game_mode=goal_buyout;
                    m_butGoal_monopoly.set_toggle(false);
                    m_butGoal_buyout.set_toggle(true);
                    m_butGoal_time.set_toggle(false);
                    m_butGoal_money.set_toggle(false);
                    m_tibGoal_time.unmark();
                    m_tibGoal_money.unmark();

                    return_value = 1;
                }
            }
            else
            {
                if(m_butGoal_buyout.m_state!=2) m_butGoal_buyout.m_state=1;//change button texture
            }
        }
        else if(m_butGoal_buyout.m_state!=2) m_butGoal_buyout.m_state=0;//change button texture

        //time
        if( m_butGoal_time.overlap_test(mouse_pos[0],mouse_pos[1]) )
        {
            if(mouse_but[0])
            {
                m_butGoal_time.m_state=2;//change button texture
                if(m_LMB_delay<=0)
                {
                    m_LMB_delay=_click_delay;
                    m_game_mode=goal_time;
                    m_butGoal_monopoly.set_toggle(false);
                    m_butGoal_buyout.set_toggle(false);
                    m_butGoal_time.set_toggle(true);
                    m_butGoal_money.set_toggle(false);
                    m_tibGoal_time.unmark();
                    m_tibGoal_money.unmark();

                    return_value = 1;
                }
            }
            else
            {
                if(m_butGoal_time.m_state!=2) m_butGoal_time.m_state=1;//change button texture
            }
        }
        else if(m_butGoal_time.m_state!=2) m_butGoal_time.m_state=0;//change button texture

        //money
        if( m_butGoal_money.overlap_test(mouse_pos[0],mouse_pos[1]) )
        {
            if(mouse_but[0])
            {
                m_butGoal_money.m_state=2;//change button texture
                if(m_LMB_delay<=0)
                {
                    m_LMB_delay=_click_delay;
                    m_game_mode=goal_money;
                    m_butGoal_monopoly.set_toggle(false);
                    m_butGoal_buyout.set_toggle(false);
                    m_butGoal_time.set_toggle(false);
                    m_butGoal_money.set_toggle(true);
                    m_tibGoal_time.unmark();
                    m_tibGoal_money.unmark();

                    return_value = 1;
                }
            }
            else
            {
                if(m_butGoal_money.m_state!=2) m_butGoal_money.m_state=1;//change button texture
            }
        }
        else if(m_butGoal_money.m_state!=2) m_butGoal_money.m_state=0;//change button texture


        //Text input boxes
        if(mouse_but[0] && m_LMB_delay<=0 )//unmark all and test if marked
        {
            m_LMB_delay=_click_delay;
            m_tibGoal_time.unmark();
            m_tibGoal_money.unmark();
            if( m_tibGoal_time.mark_test(mouse_pos[0],mouse_pos[1]) )
            {
                m_game_mode=goal_time;
                m_butGoal_time.m_state=2;
                m_butGoal_monopoly.set_toggle(false);
                m_butGoal_buyout.set_toggle(false);
                m_butGoal_time.set_toggle(true);
                m_butGoal_money.set_toggle(false);

                return_value = 1;

            }
            if( m_tibGoal_money.mark_test(mouse_pos[0],mouse_pos[1]) )
            {
                m_game_mode=goal_money;
                m_butGoal_money.m_state=2;
                m_butGoal_monopoly.set_toggle(false);
                m_butGoal_buyout.set_toggle(false);
                m_butGoal_time.set_toggle(false);
                m_butGoal_money.set_toggle(true);

                return_value = 1;
            }
        }

        if( m_tibGoal_time.update(keys,time) )//change happend
        {
            return_value = 1;
        }
        if( m_tibGoal_money.update(keys,time) )//change happend
        {
            return_value = 1;
        }
    }

    if( !m_tibGoal_time.is_marked() && !m_tibGoal_money.is_marked() )//no console input if tib is marked
    {
        m_Console.update(keys,time,message_to_send);
    }

    m_last_time=time;

    return return_value;
}

bool lobby::draw(void)
{
    m_decGoal_monopoly.draw();
    m_decGoal_buyout.draw();
    m_decGoal_time.draw();
    m_decGoal_money.draw();
    m_tibGoal_time.draw();
    m_tibGoal_money.draw();
    m_Console.draw();
    m_Player_list.draw();
    m_butLeave.draw_button();
    m_butReady.draw_button();
    m_butGoal_monopoly.draw_button();
    m_butGoal_buyout.draw_button();
    m_butGoal_time.draw_button();
    m_butGoal_money.draw_button();

    return true;
}

int lobby::get_game_mode(void)
{
    return m_game_mode;
}

bool lobby::set_game_mode(int game_mode)
{
    m_game_mode=game_mode;
    m_butGoal_monopoly.set_toggle(false);
    m_butGoal_buyout.set_toggle(false);
    m_butGoal_time.set_toggle(false);
    m_butGoal_money.set_toggle(false);
    switch(game_mode)
    {
        case 0:
        {
            m_butGoal_monopoly.set_toggle(true);
            m_butGoal_monopoly.m_state=2;
        }break;

        case 1:
        {
            m_butGoal_buyout.set_toggle(true);
            m_butGoal_buyout.m_state=2;
        }break;

        case 2:
        {
            m_butGoal_time.set_toggle(true);
            m_butGoal_time.m_state=2;
        }break;

        case 3:
        {
            m_butGoal_money.set_toggle(true);
            m_butGoal_money.m_state=2;
        }break;

        default: return false;
    }

    return true;
}

int lobby::get_game_goal(void)
{
    if(m_game_mode==goal_time)  return m_tibGoal_time.get_value();
    if(m_game_mode==goal_money) return m_tibGoal_money.get_value();

    return 0;//error
}

bool lobby::set_game_goal(int game_mode,int goal)
{
    switch(game_mode)
    {
        case 2://time
        {
            m_tibGoal_time.set_value( (float)goal );
            m_tibGoal_money.set_text( " SET " );
        }break;

        case 3://money
        {
            m_tibGoal_money.set_value( (float)goal );
            m_tibGoal_time.set_text( " SET " );
        }break;

        default: return false;
    }

    return true;
}
