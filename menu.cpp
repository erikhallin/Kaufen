#include "menu.h"

#define _click_delay 0.2
#define _graph_delay 0.05

menu::menu()
{
    m_ready=false;
}

bool menu::init(int window_width,int window_height,int texture_buttons,int texture_font[3],int texture_help,sound* pSoundEngine,float time,float version)
{
    m_add_graph_point_time=m_last_time=0;
    float x_pos,y_pos,width,height;
    float tex_x_min,tex_x_max,tex_y_min,tex_y_max;

    //create title decal
    x_pos= 0.045 *(float)window_width; y_pos= 0.10 *(float)window_height; width= 0.90 *(float)window_width; height= 0.40 *(float)window_height;
    tex_x_min=0; tex_x_max=720; tex_y_min=0; tex_y_max=257;
    m_deTitle=decal( x_pos,y_pos,width,height,texture_buttons,tex_x_min,tex_x_max,tex_y_min,tex_y_max );

    //Help decal
    tex_x_min=0; tex_x_max=window_width; tex_y_min=0; tex_y_max=window_height;
    m_deHelp=decal( 0,0,window_width,window_height,texture_help,tex_x_min,tex_x_max,tex_y_min,tex_y_max );
    m_deHelp.show_decal(false);
    m_deHelp.masking(false);

    //credits decal
    x_pos= 0.84 *(float)window_width; y_pos= 0.97 *(float)window_height; width= 0.15 *(float)window_width; height= 0.0313 *(float)window_height;
    tex_x_min=885; tex_x_max=1004; tex_y_min=75; tex_y_max=94;
    m_deCred=decal( x_pos,y_pos,width,height,texture_buttons,tex_x_min,tex_x_max,tex_y_min,tex_y_max );

    //version display
    stringstream ss;
    ss<<version;
    x_pos= 0.01 *(float)window_width; y_pos= 0.97 *(float)window_height; width= 0.15 *(float)window_width; height= 0.0313 *(float)window_height;
    m_diVersion=display( x_pos,y_pos,width,height,10,texture_font,ss.str() );
    m_diVersion.setting_flags(false);

    //create buttons
    x_pos= 0.42 *(float)window_width; y_pos= 0.55 *(float)window_height; width= 0.10 *(float)window_width; height= 0.058 *(float)window_height;
    tex_x_min=721; tex_x_max=802; tex_y_min=0; tex_y_max=36;
    m_buHost= button( x_pos,y_pos,width,height,texture_buttons,tex_x_min,tex_x_max,tex_y_min,tex_y_max );

    x_pos= 0.42 *(float)window_width; y_pos= 0.65 *(float)window_height; width= 0.10 *(float)window_width; height= 0.058 *(float)window_height;
    tex_x_min=803; tex_x_max=882; tex_y_min=0; tex_y_max=36;
    m_buJoin= button( x_pos,y_pos,width,height,texture_buttons,tex_x_min,tex_x_max,tex_y_min,tex_y_max );

    x_pos= 0.42 *(float)window_width; y_pos= 0.75 *(float)window_height; width= 0.10 *(float)window_width; height= 0.058 *(float)window_height;
    tex_x_min=721; tex_x_max=802; tex_y_min=111; tex_y_max=147;
    m_buHowTo=button( x_pos,y_pos,width,height,texture_buttons,tex_x_min,tex_x_max,tex_y_min,tex_y_max );

    x_pos= 0.42 *(float)window_width; y_pos= 0.85 *(float)window_height; width= 0.10 *(float)window_width; height= 0.058 *(float)window_height;
    tex_x_min=803; tex_x_max=882; tex_y_min=111; tex_y_max=147;
    m_buExit= button( x_pos,y_pos,width,height,texture_buttons,tex_x_min,tex_x_max,tex_y_min,tex_y_max );

    x_pos= 0.58 *(float)window_width; y_pos= 0.66 *(float)window_height; width= 0.12 *(float)window_width; height= 0.04 *(float)window_height;
    tex_x_min=885; tex_x_max=981; tex_y_min=0; tex_y_max=24;
    m_buSearchLAN= button( x_pos,y_pos,width,height,texture_buttons,tex_x_min,tex_x_max,tex_y_min,tex_y_max );


    //create text boxes
    float color[3]={0.4,0.4,0.4};
    x_pos= 0.05 *(float)window_width; y_pos= 0.55 *(float)window_height; width= 0.33 *(float)window_width; height= 0.06 *(float)window_height;
    m_tibPlayer_name=text_input_box( x_pos,y_pos,width,height,20,texture_font,"Player Name",pSoundEngine );
    m_tibPlayer_name.change_border_color(color);

    x_pos= 0.56 *(float)window_width; y_pos= 0.55 *(float)window_height; width= 0.33 *(float)window_width; height= 0.06 *(float)window_height;
    m_tibServer_name=text_input_box( x_pos,y_pos,width,height,20,texture_font,"Server Name",pSoundEngine );
    m_tibServer_name.change_border_color(color);

    x_pos= 0.05 *(float)window_width; y_pos= 0.65 *(float)window_height; width= 0.33 *(float)window_width; height= 0.06 *(float)window_height;
    m_tibIP_number  =text_input_box( x_pos,y_pos,width,height,20,texture_font,"IP number",pSoundEngine );
    m_tibIP_number.change_border_color(color);

    //create background graphs
    m_vNoise_gens.clear();//reset old values
    m_vNoise_gens_sec.clear();
    m_vBackgroundGraphs.clear();//reset old values
    int numof_graphs=10;
    int min_x_pos=0; int max_x_pos=2;
    int min_y_pos=-150; int max_y_pos=200;
    int min_width=window_width; int max_width=window_width+100;
    int min_height=window_height; int max_height=window_height+100;
    for(int i=0;i<numof_graphs;i++)
    {
        int xpos = rand()%(max_x_pos-min_x_pos)+min_x_pos;
        int ypos = rand()%(max_y_pos-min_y_pos)+min_y_pos;
        int xwidth = rand()%(max_width-min_width)+min_width;
        int yheight = rand()%(max_height-min_height)+min_height;
        float transparancy= float( (rand()%900)+100 ) /1000;
        float graph_width_sec=20;

        m_vNoise_gens.push_back( noise_generator(0.80,0.20,0.50,6,rand()%1000) );
        m_vNoise_gens_sec.push_back( noise_generator(0.80,3,0.05,6,rand()%1000) );
        m_vBackgroundGraphs.push_back( graph(xpos,ypos,xwidth,yheight,graph_width_sec,0.0f,1.0f) );

        //set color
        m_vBackgroundGraphs[i].set_color(0,transparancy,0,1);

        //add fake points
        for(int j=500;j>=0;j--)
        {
            float x=time-( float(j)*_graph_delay );
            m_vBackgroundGraphs[i].add_point( x, m_vNoise_gens[i].get_noise(x) + m_vNoise_gens_sec[i].get_noise(x) );
        }
    }

    m_show_help=false;

    m_ready=true;

    return true;
}

int menu::update(bool keys[],int mouse_pos[],bool mouse_but[],float time)
{
    //update backgrounds graphs
    if(time>m_add_graph_point_time)//time to add a point
    {
        for(int i=0;i<(int)m_vBackgroundGraphs.size();i++)
        {
            m_vBackgroundGraphs[i].add_point( time,m_vNoise_gens[i].get_noise(time) + m_vNoise_gens_sec[i].get_noise(time)  );
        }
        m_add_graph_point_time=time+_graph_delay;
    }

    if(m_LMB_delay>0)
    {
        m_LMB_delay-=time-m_last_time;
    }

    if(mouse_but[0] && m_LMB_delay<=0 && m_show_help)
    {
        //hide help
        m_deHelp.show_decal(false);
        m_show_help=false;
        m_LMB_delay=_click_delay;
    }

    if(m_show_help)
    {
        m_last_time=time;
        return 0;//skip button test if help is shown
    }

    if(mouse_but[0] && m_LMB_delay<=0)
    {
        //hide help
        m_deHelp.show_decal(false);
        m_show_help=false;
    }

    //button test
    //hosting
    if( m_buHost.overlap_test(mouse_pos[0],mouse_pos[1]) )
    {
        if(mouse_but[0])
        {
            m_buHost.m_state=2;//change button texture
            if(m_LMB_delay<=0)
            {
                m_LMB_delay=_click_delay;
                return 1;
            }
        }
        else
        {
            m_buHost.m_state=1;//change button texture
        }
    }
    else m_buHost.m_state=0;//change button texture

    //joining
    if( m_buJoin.overlap_test(mouse_pos[0],mouse_pos[1]) )
    {
        if(mouse_but[0])
        {
            m_buJoin.m_state=2;//change button texture
            if(m_LMB_delay<=0)
            {
                m_LMB_delay=_click_delay;
                return 2;
            }
        }
        else
        {
            m_buJoin.m_state=1;//change button texture
        }
    }
    else m_buJoin.m_state=0;//change button texture

    //help
    if( m_buHowTo.overlap_test(mouse_pos[0],mouse_pos[1]) )
    {
        if(mouse_but[0])
        {
            m_buHowTo.m_state=2;//change button texture
            if(m_LMB_delay<=0)
            {
                m_LMB_delay=_click_delay;
                m_deHelp.show_decal(true);
                m_show_help=true;
                return 3;
            }
        }
        else
        {
            m_buHowTo.m_state=1;//change button texture
        }
    }
    else m_buHowTo.m_state=0;//change button texture

    //exit
    if( m_buExit.overlap_test(mouse_pos[0],mouse_pos[1]) )
    {
        if(mouse_but[0])
        {
            m_buExit.m_state=2;//change button texture
            if(m_LMB_delay<=0)
            {
                m_LMB_delay=_click_delay;
                return 4;
            }
        }
        else
        {
            m_buExit.m_state=1;//change button texture
        }
    }
    else m_buExit.m_state=0;//change button texture

    //exit
    if( m_buSearchLAN.overlap_test(mouse_pos[0],mouse_pos[1]) )
    {
        if(mouse_but[0])
        {
            m_buSearchLAN.m_state=2;//change button texture
            if(m_LMB_delay<=0)
            {
                m_LMB_delay=_click_delay;
                return 5;
            }
        }
        else
        {
            m_buSearchLAN.m_state=1;//change button texture
        }
    }
    else m_buSearchLAN.m_state=0;//change button texture

    if( mouse_but[0] && m_LMB_delay<=0 )
    {
        m_LMB_delay=_click_delay;
        //interactive text box
        //unmark all
        m_tibServer_name.unmark();
        m_tibPlayer_name.unmark();
        m_tibIP_number.unmark();
        //mark test
        m_tibServer_name.mark_test(mouse_pos[0],mouse_pos[1]);
        m_tibPlayer_name.mark_test(mouse_pos[0],mouse_pos[1]);
        m_tibIP_number.mark_test(mouse_pos[0],mouse_pos[1]);
    }


    //check for keyboard input for marked text boxes
    m_tibServer_name.update(keys,time);
    m_tibPlayer_name.update(keys,time);
    m_tibIP_number.update(keys,time);

    m_last_time=time;
    return 0;
}

bool menu::draw()
{
    //help
    if(m_show_help)
    {
        m_deHelp.draw();
        return true;
    }
    //backgrounds graphs
    for(int i=0;i<(int)m_vBackgroundGraphs.size();i++)
    {
        m_vBackgroundGraphs[i].draw_graph();
    }
    //title
    m_deTitle.draw();
    //buttons
    m_buHowTo.draw_button();
    m_buHost.draw_button();
    m_buJoin.draw_button();
    m_buExit.draw_button();
    m_buSearchLAN.draw_button();
    //text boxes
    m_tibServer_name.draw();
    m_tibPlayer_name.draw();
    m_tibIP_number.draw();
    //credits
    m_deCred.draw();
    //version
    m_diVersion.draw_display();

    return true;
}

bool menu::set_IP(string sIP)
{
    m_tibIP_number.set_text(sIP);
    return true;
}

string menu::get_IP(void)
{
    return m_tibIP_number.get_text();
}

string menu::get_player_name(void)
{
    string name=m_tibPlayer_name.get_text();
    //convert spaces to _
    for(int i=0;i<(int)name.size();i++)
    {
        if(name[i]==' ') name[i]='_';
    }

    return name;
}

bool menu::set_player_name(string name)
{
    m_tibPlayer_name.set_text(name);
    return true;
}

string menu::get_server_name(void)
{
    string name=m_tibServer_name.get_text();
    //convert spaces to _
    for(int i=0;i<(int)name.size();i++)
    {
        if(name[i]==' ') name[i]='_';
    }

    return name;
}

bool menu::info_missing(string type)
{
    if(type=="player_name")
    {
        float color[3]={1,0,0};
        m_tibPlayer_name.change_border_color(color);
        return true;
    }
    if(type=="server_name")
    {
        float color[3]={1,0,0};
        m_tibServer_name.change_border_color(color);
        return true;
    }
    if(type=="IP_number")
    {
        float color[3]={1,0,0};
        m_tibIP_number.change_border_color(color);
        return true;
    }

    return false;
}
