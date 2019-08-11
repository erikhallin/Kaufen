#include "win_window.h"

#define _click_delay 0.2

win_window::win_window()
{
    m_numof_coins=1000;
    m_ready=false;
}

bool win_window::init(int window_width,int window_height,int texture_buttons,int texture_font[3],int texture_gameover)
{
    //clear vectors
    m_coCoins.clear();

    m_last_time=m_LMB_delay=0;

    float tex_x_min,tex_x_max,tex_y_min,tex_y_max;
    //background
    tex_x_min=0; tex_x_max=window_width; tex_y_min=0; tex_y_max=window_height;
    m_deBackground=decal( 0,0,window_width,window_height,texture_gameover,tex_x_min,tex_x_max,tex_y_min,tex_y_max );
    m_deBackground.masking(false);

    //reset button
    tex_x_min=480; tex_x_max=639; tex_y_min=219; tex_y_max=266;
    float x_pos= 0.40 *(float)window_width; float y_pos= 0.80 *(float)window_height;
    float width= 0.20 *(float)window_width; float height= 0.08 *(float)window_height;
    m_buRestart=button(x_pos,y_pos,width,height,texture_buttons,tex_x_min,tex_x_max,tex_y_min,tex_y_max);

    //name display
    x_pos= 0.35 *(float)window_width; y_pos= 0.35 *(float)window_height;
    width= 0.30 *(float)window_width; height=0.07 *(float)window_height;
    m_diName=display(x_pos,y_pos,width,height,20,texture_font,"Everybody");
    m_diName.setting_flags(false);//no borders

    //create coins
    int min_size=5;
    int max_size=15;

    for(int i=0;i<m_numof_coins;i++)
    {
        int xpos=rand()%window_width;
        int ypos=rand()%window_height;
        int size=rand()%max_size+min_size;

        float rot_speed= float(rand()%21-10)*5 ; //-10 to +10
        if( rot_speed<3 && rot_speed>-3) rot_speed=9;

        float fall_speed= float(rand()%200+50)*1 ; //-10 to +10
        m_coCoins.push_back( coin_anim( xpos,ypos,size,size,texture_gameover,0,31,651,682,rot_speed,fall_speed,window_width,window_height ) );
    }

    m_ready=true;

    return true;
}

bool win_window::set_winner_name(string name)
{
    m_diName.set_text(name);

    return true;
}

int win_window::update(bool keys[],int mouse_pos[],bool mouse_but[],float time)
{
    //timing
    if(m_LMB_delay>0)
    {
        m_LMB_delay-=time-m_last_time;
    }

    //coin update
    for(int i=0;i<(int)m_coCoins.size();i++)
    {
        //update coins, fall and rotate
        m_coCoins[i].update(time);
    }

    //button
    if( m_buRestart.overlap_test(mouse_pos[0],mouse_pos[1]) )
    {
        if(mouse_but[0])
        {
            m_buRestart.m_state=2;//change button texture
            if(m_LMB_delay<=0)
            {
                m_LMB_delay=_click_delay;
                return 1;
            }
        }
        else
        {
            m_buRestart.m_state=1;//change button texture
        }
    }
    else m_buRestart.m_state=0;//change button texture

    m_last_time=time;

    return 0;
}

bool win_window::draw(void)
{
    m_deBackground.draw();
    m_diName.draw_display();
    m_buRestart.draw_button();

    //coins
    for(int i=0;i<(int)m_coCoins.size();i++)
    {
        m_coCoins[i].draw();
    }

    return true;
}
