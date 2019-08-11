#include "text_list.h"

text_list::text_list()
{
    m_ready=false;
}

text_list::text_list(int x_pos,int y_pos,int width,int height,int numof_players,int font_texture[3],int texture,
                     float tex_offset_x_min,float tex_offset_x_max,float tex_offset_y_min,float tex_offset_y_max)
{
    //clear vectors to be safe
    m_vDisplays_names.clear();
    m_vPlayer_names.clear();

    m_x_pos=x_pos;
    m_y_pos=y_pos;
    m_width=width;
    m_height=height;
    m_numof_players=numof_players;
    m_texture=texture;
    m_font_textures[0]=font_texture[0];
    m_font_textures[1]=font_texture[1];
    m_font_textures[2]=font_texture[2];

    m_LMB_delay=0;

    //create decal (background)
    m_deBackground=decal(x_pos,y_pos,width,height,texture,tex_offset_x_min,tex_offset_x_max,tex_offset_y_min,tex_offset_y_max);

    //displays
    for(int i=0;i<numof_players;i++)
    {
        float x_name_start=0.0; float y_start=0.01;
        float y_shift=(1-y_start)/(float)numof_players;
        if(y_shift>0.05) y_shift=0.05; //cut-off

        float x_name_end=1.0;

        m_vDisplays_names.push_back( display( int(x_pos+float(width)*(x_name_start)), int(y_pos+float(height)*(y_start+y_shift*(float)i)),
                                              int((x_name_end-x_name_start)*(float)width), int(y_shift*(float)height), 20, m_font_textures, "player") );
        //turn off border
        m_vDisplays_names[(int)m_vDisplays_names.size()-1].setting_flags(false);
    }

    m_ready=true;
}

bool text_list::add_player(string name)
{
    if(name!="") //noname, will refresh current player list (used for removing player)
    {
        m_numof_players++;
        m_vPlayer_names.push_back(name);
    }

    //recreate all displays and buyout buttons
    m_vDisplays_names.clear();
    for(int i=0;i<m_numof_players;i++)
    {
        float x_name_start=0.0; float y_start=0.01;
        float y_shift=(1-y_start)/(float)m_numof_players;
        if(y_shift>0.05) y_shift=0.05; //cut-off

        float x_name_end=1.0;

        m_vDisplays_names.push_back( display( int(m_x_pos+float(m_width)*(x_name_start)), int(m_y_pos+float(m_height)*(y_start+y_shift*(float)i)),
                                              int((x_name_end-x_name_start)*(float)m_width), int(y_shift*(float)m_height), 20, m_font_textures, m_vPlayer_names[i] ) );
        //turn off border
        m_vDisplays_names[(int)m_vDisplays_names.size()-1].setting_flags(false);
    }

    return true;
}

bool text_list::remove_player(string name)
{
    //find player
    int player_id=-1;
    for(int i=0;i<(int)m_vPlayer_names.size();i++)
    {
        if(m_vPlayer_names[i]==name)
        {
            player_id=i;
            break;
        }
    }
    if(player_id==-1) return false;

    m_vPlayer_names.erase( m_vPlayer_names.begin() + player_id );
    m_numof_players--;

    add_player(""); //recreates buttons and displays for the rest of the players

    return true;
}

bool text_list::draw(void)
{
    if(!m_ready) return false;

    //m_deBackground.draw();
    for(int i=0;i<(int)m_vDisplays_names.size();i++)
    {
        m_vDisplays_names[i].draw_display();
    }

    return true;
}
