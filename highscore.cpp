#include "highscore.h"

highscore::highscore()
{
    m_ready=false;
}

highscore::highscore(int x_pos,int y_pos,int width,int height,int numof_players,int numof_values,int font_texture[3],int texture,
                     float tex_offset_x_min,float tex_offset_x_max,float tex_offset_y_min,float tex_offset_y_max)
{
    //clear vectors to be safe
    m_vBuyoutButtons.clear();
    m_vPlayers.clear();
    m_vDisplays_values.clear();
    m_vDisplays_names.clear();

    m_x_pos=x_pos;
    m_y_pos=y_pos;
    m_width=width;
    m_height=height;
    m_numof_players=numof_players;
    m_numof_values=numof_values;
    m_texture=texture;
    m_font_textures[0]=font_texture[0];
    m_font_textures[1]=font_texture[1];
    m_font_textures[2]=font_texture[2];

    m_sort_by_name=false;
    m_sort_value=0;
    m_LMB_delay=0;
    m_time_last_cycle=0;

    //fill queue list
    m_vQueue_list.clear();//to be safe
    for(int i=0;i<numof_players;i++)
    {
        m_vQueue_list.push_back(i);
    }

    //create decal (background)
    m_deBackground=decal(x_pos,y_pos,width,height,texture,tex_offset_x_min,tex_offset_x_max,tex_offset_y_min,tex_offset_y_max);

    //buttons
    float but_y_start=0.0; float but_x_start=0.05;
    float but_width=0.25; float but_height=0.1;
    //namesort
    m_buSort_name=button( int(x_pos+float(width)*(but_x_start)), int(y_pos+float(height)*(but_y_start) ),
                          int((but_width)*(float)width), int(but_height*(float)height), texture,
                          0, 42, 0, 18);
    //valuesort1
    but_y_start=0.0; but_x_start=0.40;
    but_width=0.25; but_height=0.1;
    m_buSort_val1=button( int(x_pos+float(width)*(but_x_start)), int(y_pos+float(height)*(but_y_start) ),
                          int((but_width)*(float)width), int(but_height*(float)height), texture,
                          43, 89, 0, 18);
    //valuesort2
    but_y_start=0.0; but_x_start=0.70;
    but_width=0.25; but_height=0.1;
    m_buSort_val2=button( int(x_pos+float(width)*(but_x_start)), int(y_pos+float(height)*(but_y_start) ),
                          int((but_width)*(float)width), int(but_height*(float)height), texture,
                          90, 136, 0, 18);

    //displays
    for(int i=0;i<numof_players;i++)
    {
        float x_name_start=0.0; float y_start=0.1;
        float y_shift=(1-y_start)/(float)numof_players;
        if(y_shift>0.15) y_shift=0.15; //cut-off

        float x_val_start=0.4;
        float x_val_shift=(1-x_val_start)/numof_values;
        float x_val_gap_size=0.02;

        m_vDisplays_names.push_back( display( int(x_pos+float(width)*(x_name_start)), int(y_pos+float(height)*(y_start+y_shift*(float)i)),
                                              int((x_val_start-x_name_start)*(float)width), int(y_shift*(float)height), 10, m_font_textures, "player") );
        //turn off border
        m_vDisplays_names[(int)m_vDisplays_names.size()-1].setting_flags(false);
        //value displays
        for(int j=0;j<numof_values;j++)
        {
            m_vDisplays_values.push_back( display( int(x_pos+float(width)*(x_val_start+x_val_shift*float(j))), int(y_pos+float(height)*(y_start+y_shift*(float)i)),
                                                   int((x_val_shift-x_val_gap_size)*(float)width), int(y_shift*(float)height), 10, m_font_textures, 0.0f ) );
            //turn off border
            m_vDisplays_values[(int)m_vDisplays_values.size()-1].setting_flags(false);
        }

        //buyout buttons
        but_y_start=0.10; but_x_start=0.98;
        float but_size=y_shift*0.55;
        if(but_size>0.09) but_size=0.09; //cut-off
        but_width=but_size; but_height=but_size; //square based on height
        for(int j=0;j<m_numof_players;j++)
        {
            m_vBuyoutButtons.push_back( button( int(x_pos+float(width)*(but_x_start)), int(y_pos+float(height)*(but_y_start+y_shift*(float)j+0.17*y_shift) ),
                                                int((but_width)*(float)width), int(but_height*(float)height), texture,
                                                137, 149, 0, 12 ) );
        }
    }

    m_ready=true;
}

bool highscore::update_values_for_player( string player_name, float* values )
{
    //valuse in vValues have to be pushed back before, otherwise crash!
    for(int i=0;i<(int)m_vPlayers.size();i++)
    {
        if(m_vPlayers[i].name==player_name)//find player
        {
            //cout<<player_name<<"\tvalues: ";
            int display_row_id=-1;
            for(int k=0;k<(int)m_vPlayers.size();k++)//find players display
            {
                if( i==m_vQueue_list[k] ) display_row_id=k;
            }
            if(display_row_id==-1) return false;

            for(int j=0;j<m_numof_values;j++)//assign values
            {
                m_vPlayers[i].vValues[j]=values[j];
                //cout<<m_vPlayers[i].vValues[j]<<"\t";
                //m_vDisplays_values[ i * m_numof_values + j ].set_value( values[j] ); //display update (wrong display)
                m_vDisplays_values[ display_row_id * m_numof_values + j ].set_value( values[j] ); //display update
            }
            break;
        }
    }

    if(!m_sort_by_name) sort_list();

    return true;
}

bool highscore::new_player(string name,float money_value,float value_sum)
{
    if(name!="") //noname, will refresh current player list (used for removing player)
    {
        m_numof_players++;

        player temp_player;
        temp_player.name=name;
        temp_player.vValues.push_back(money_value);//first val
        temp_player.vValues.push_back(value_sum);//second val

        m_vPlayers.push_back(temp_player);
    }

    //reset queue list
    m_vQueue_list.clear();
    for(int i=0;i<m_numof_players;i++)
    {
        m_vQueue_list.push_back(i);
    }

    //recreate all displays and buyout buttons
    m_vDisplays_names.clear();
    m_vDisplays_values.clear();
    m_vBuyoutButtons.clear();
    for(int i=0;i<m_numof_players;i++)
    {
        float x_name_start=0.0; float y_start=0.1;
        float y_shift=(1-y_start)/(float)m_numof_players;
        if(y_shift>0.15) y_shift=0.15; //cut-off

        float x_val_start=0.4;
        float x_val_shift=(1-x_val_start)/m_numof_values;
        float x_val_gap_size=0.02;

        m_vDisplays_names.push_back( display( int(m_x_pos+float(m_width)*(x_name_start)), int(m_y_pos+float(m_height)*(y_start+y_shift*(float)i)),
                                              int((x_val_start-x_name_start-x_val_gap_size)*(float)m_width), int(y_shift*(float)m_height), 10, m_font_textures, m_vPlayers[i].name ) );
        //turn off border
        m_vDisplays_names[(int)m_vDisplays_names.size()-1].setting_flags(false);
        //value dispalys
        for(int j=0;j<m_numof_values;j++)
        {
            m_vDisplays_values.push_back( display( int(m_x_pos+float(m_width)*(x_val_start+x_val_shift*float(j))), int(m_y_pos+float(m_height)*(y_start+y_shift*(float)i)),
                                                   int((x_val_shift-x_val_gap_size)*(float)m_width), int(y_shift*(float)m_height), 10, m_font_textures, m_vPlayers[i].vValues[j] ) );
            //turn off border
            m_vDisplays_values[(int)m_vDisplays_values.size()-1].setting_flags(false);
        }

        //recreate buyout buttons
        float but_y_start=0.10; float but_x_start=0.98;
        float but_size=y_shift*0.55;
        if(but_size>0.09) but_size=0.09; //cut-off
        float but_width=but_size; float but_height=but_size; //square based on height
        for(int j=0;j<m_numof_players;j++)
        {
            m_vBuyoutButtons.push_back( button( int(m_x_pos+float(m_width)*(but_x_start)), int(m_y_pos+float(m_height)*(but_y_start+y_shift*(float)j+0.17*y_shift) ),
                                                int((but_width)*(float)m_width), int(but_height*(float)m_height), m_texture,
                                                137, 149, 0, 12 ) );
        }
    }




    return true;
}

bool highscore::remove_player(string name)
{
    //find player
    int player_id=-1;
    for(int i=0;i<(int)m_vPlayers.size();i++)
    {
        if(m_vPlayers[i].name==name)
        {
            player_id=i;
            break;
        }
    }
    if(player_id==-1) return false;

    m_vPlayers.erase( m_vPlayers.begin() + player_id );
    m_numof_players--;

    new_player("",0.0f,0.0f); //recreates buttons and displays for the rest of the players

    return true;
}

int highscore::interaction_test(int mouse_pos[],bool mouse_but[],float time,string& bought_name)
{
    if(!m_ready) return -1;

    if(m_LMB_delay>0) m_LMB_delay-=time-m_time_last_cycle;
    m_time_last_cycle=time;

    //Buttons
    //sort by name
    if(m_buSort_name.overlap_test(mouse_pos[0],mouse_pos[1]))
    {
        if(mouse_but[0]&&m_LMB_delay<=0)
        {
            m_buSort_name.m_state=2;
            m_LMB_delay=_click_delay;
            m_sort_by_name=true;

            sort_list();
            return 2;
        }
        else
        {
            if(mouse_but[0]) //waiting for delay
            {
                m_buSort_name.m_state=2;
            }
            else m_buSort_name.m_state=1; //just hoovering
        }
    }
    //sort by money
    if(m_buSort_val1.overlap_test(mouse_pos[0],mouse_pos[1]))
    {
        if(mouse_but[0]&&m_LMB_delay<=0)
        {
            m_buSort_val1.m_state=2;
            m_LMB_delay=_click_delay;
            m_sort_by_name=false;
            m_sort_value=0;

            sort_list();
            return 2;
        }
        else
        {
            if(mouse_but[0]) //waiting for delay
            {
                m_buSort_val1.m_state=2;
            }
            else m_buSort_val1.m_state=1; //just hoovering
        }
    }
    //sort by value
    if(m_buSort_val2.overlap_test(mouse_pos[0],mouse_pos[1]))
    {
        if(mouse_but[0]&&m_LMB_delay<=0)
        {
            m_buSort_val2.m_state=2;
            m_LMB_delay=_click_delay;
            m_sort_by_name=false;
            m_sort_value=1;

            sort_list();
            return 2;
        }
        else
        {
            if(mouse_but[0]) //waiting for delay
            {
                m_buSort_val2.m_state=2;
            }
            else m_buSort_val2.m_state=1; //just hoovering
        }
    }
    //buyout buttons
    for(int i=0;i<(int)m_vBuyoutButtons.size();i++)
    {
        if(m_vBuyoutButtons[i].overlap_test(mouse_pos[0],mouse_pos[1]))
        {
            if(mouse_but[0]&&m_LMB_delay<=0)
            {
                m_vBuyoutButtons[i].m_state=2;
                m_LMB_delay=_click_delay;
                bought_name=m_vPlayers[ m_vQueue_list[i] ].name;
                return 1;
            }
            else
            {
                if(mouse_but[0]) //waiting for delay
                {
                    m_vBuyoutButtons[i].m_state=2;
                }
                else m_vBuyoutButtons[i].m_state=1; //just hoovering
            }
        }
    }

    return 0;
}

bool highscore::draw(void)
{
    if(!m_ready) return false;

    //m_deBackground.draw();
    for(int i=0;i<(int)m_vDisplays_names.size();i++)
    {
        m_vDisplays_names[i].draw_display();
    }
    for(int i=0;i<(int)m_vDisplays_values.size();i++)
    {
        m_vDisplays_values[i].draw_display();
    }
    m_buSort_name.draw_button();
    m_buSort_val1.draw_button();
    m_buSort_val2.draw_button();
    for(int i=0;i<(int)m_vBuyoutButtons.size();i++)
    {
        m_vBuyoutButtons[i].draw_button();
    }

    return true;
}

bool highscore::sort_list(void)
{
    bool done=false;
    bool change_displays=false;
    while(!done)
    {
        bool change_made=false;
        if(m_sort_by_name)
        {
            //cout<<"Sort by name\n";
            for(int i=0;i<(int)m_vPlayers.size()-1;i++)
            {
                if(m_vPlayers[ m_vQueue_list[i] ].name > m_vPlayers[ m_vQueue_list[i+1] ].name)//if name is "bigger" than next, swap
                {
                    //cout<<"SORT: "<<m_vPlayers[ m_vQueue_list[i] ].name<<" > "<<m_vPlayers[ m_vQueue_list[i+1] ].name<<endl;
                    change_made=true;
                    change_displays=true;
                    //swap place
                    int temp_val=m_vQueue_list[i];
                    m_vQueue_list[i]=m_vQueue_list[i+1];
                    m_vQueue_list[i+1]=temp_val;
                }
            }
        }
        else //sort by value
        {
            //cout<<"Sort by value: "<<m_sort_value<<endl;
            for(int i=0;i<(int)m_vPlayers.size()-1;i++)
            {
                if(m_vPlayers[ m_vQueue_list[i] ].vValues[m_sort_value] < m_vPlayers[ m_vQueue_list[i+1] ].vValues[m_sort_value])//if value is bigger than next, swap
                {
                    cout<<"SORT: "<<m_vPlayers[ m_vQueue_list[i] ].vValues[m_sort_value]<<" < "<<m_vPlayers[ m_vQueue_list[i+1] ].vValues[m_sort_value]<<endl;
                    change_made=true;
                    change_displays=true;
                    //swap place
                    int temp_val=m_vQueue_list[i];
                    m_vQueue_list[i]=m_vQueue_list[i+1];
                    m_vQueue_list[i+1]=temp_val;
                }
            }
        }

        if(!change_made)
        {
            done=true;
        }
    }

    if(change_displays)
    {
        for(int i=0;i<(int)m_vPlayers.size();i++)
        {
            //input name
            m_vDisplays_names[ i ].set_text( m_vPlayers[ m_vQueue_list[i] ].name );
            //input belonging values
            for(int j=0;j<m_numof_values;j++)
            {
                //2 is m_numof_values\ /
                m_vDisplays_values[ i*2+j ].set_value( m_vPlayers[ m_vQueue_list[i] ].vValues[j] );
            }
        }
    }


    return true;
}
