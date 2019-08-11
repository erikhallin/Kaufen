#include "coin_anim.h"

coin_anim::coin_anim(int x_pos,int y_pos,int width,int height,int texture,
                     float tex_offset_x_min,float tex_offset_x_max,float tex_offset_y_min,float tex_offset_y_max,
                     float rot_speed,float fall_speed,int screen_width,int screen_height)
{
    m_x_pos=x_pos;
    m_y_pos=y_pos;
    m_size=width;
    m_screen_height=screen_height;
    m_screen_width=screen_width;

    m_deCoin_pic=decal( x_pos,y_pos,width,height,texture,tex_offset_x_min,tex_offset_x_max,tex_offset_y_min,tex_offset_y_max );

    m_rot_speed=rot_speed;
    m_fall_speed=fall_speed;
    m_rot_progress=0;
    m_current_pic=0;
    m_last_time=0;

    m_ready=true;
}

bool coin_anim::update(float time)
{

    m_rot_progress += (time-m_last_time)*m_rot_speed;

    if(m_rot_progress>1)//next pic
    {
        if(m_current_pic==7)//the end, start over
        {
            m_current_pic=0;
            m_deCoin_pic.shift_texture_offset(-7,0);
        }
        else
        {
            m_current_pic++;
            m_deCoin_pic.shift_texture_offset(1,0);
        }
        m_rot_progress=0;
    }

    if(m_rot_progress<-1)//prev pic
    {
        if(m_current_pic==0)//the end, start over
        {
            m_current_pic=7;
            m_deCoin_pic.shift_texture_offset(7,0);
        }
        else
        {
            m_current_pic--;
            m_deCoin_pic.shift_texture_offset(-1,0);
        }
        m_rot_progress=0;
    }

    //fall down
    m_y_pos += (time-m_last_time)*(m_fall_speed + m_y_pos/2);
    if( (int)m_y_pos>m_screen_height )
    {
        //restart pos
        m_y_pos=-m_size;
        m_x_pos=rand()%m_screen_width;
    }
    m_deCoin_pic.set_location(m_x_pos,m_y_pos,m_size,m_size);

    m_last_time=time;

    return true;
}

bool coin_anim::draw(void)
{
    m_deCoin_pic.draw();

    return true;
}
