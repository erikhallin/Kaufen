#include "panel.h"

panel::panel()
{
    m_ready=false;
}

bool panel::init(int tex_font[],int tex_buttons,int tex_decals,sound* pSoundEngine)
{
    m_vPlayer_values.clear(); //erase players from previous game
    m_Mercs.clear(); //to be safe, needed when panel is reloaded with init()

    //store textures
    m_tex_font[0]=tex_font[0]; m_tex_font[1]=tex_font[1]; m_tex_font[2]=tex_font[2];
    m_tex_buttons=tex_buttons; m_tex_decals=tex_decals;

    m_pSoundEngine=pSoundEngine;

    srand(time(0));//seed random number generator

    //create mercs
    //load data from text file
    ifstream file("data\\merc_data.txt");
    if(file)
    {
        vector<string> names;
        float noise[_numof_mercs][5]; //default values
        int   unit_counter[_numof_mercs];
        //load data
        string line,word;
        getline(file,line); //skip first line (header)
        getline(file,line); //skip second line (header)
        for(int i=0;i<_numof_mercs;i++)
        {
            getline(file,line);//get line
            stringstream ss(line);
            for(int j=0;j<4;j++)// 4 values to read to noise
            {
                ss>>word;//get word
                float value=atof(word.c_str());//convert to float
                if(value<0) value=1;//error test
                noise[i][j]=value;//store
            }
            noise[i][4]=rand()%10000;//get random
            ss>>word;//get word (unit counter)
            float value=atof(word.c_str());//convert to float
            if(value<1) value=1;//error test
            unit_counter[i]=int(value);
            //get name of merc
            ss>>word;//get word
            names.push_back(word);
        }
        file.close();

        //construct mercs
        int row_counter=0;
        int column_counter=0;
        for(int i=0;i<_numof_mercs;i++)
        {
            int xpos=10+160*(i%4);
            int ypos=40+110*row_counter;
            int tex_xpos_min=40*i; int tex_xpos_max=40*i+39;
            int tex_ypos_min=0; int tex_ypos_max=28;
            m_Mercs.push_back( merc (xpos,ypos,150,100,unit_counter[i],noise[i][2],noise[i],tex_buttons,tex_decals,
                                     tex_xpos_min,tex_xpos_max,tex_ypos_min,tex_ypos_max,tex_font,pSoundEngine,names[i]) );
            column_counter++;
            if(column_counter>3)
            {
                row_counter++;
                column_counter=0;
            }
        }
    }
    else
    {
        cout<<"Error: could not load merc_data\n";

        float price_init;
        float noise[]={0.9,0.1,100,8,1};
        //price init,     persistance,    frequency,      amplitude (price),   octaves,    seed
        price_init=1.000; noise[0]=0.900; noise[1]=0.100; noise[2]=price_init; noise[3]=8; noise[4]=rand()%10000;
        m_Mercs.push_back(merc(10,40,150,100,100,price_init,noise,tex_buttons,tex_decals,0,39,0,28,tex_font,pSoundEngine,"UNKNOWN"));
        price_init=10.00; noise[0]=0.900; noise[1]=0.100; noise[2]=price_init; noise[3]=8; noise[4]=rand()%10000;
        m_Mercs.push_back(merc(170,40,150,100,100,price_init,noise,tex_buttons,tex_decals,0,39,0,28,tex_font,pSoundEngine,"UNKNOWN"));
        price_init=100.0; noise[0]=0.900; noise[1]=0.100; noise[2]=price_init; noise[3]=8; noise[4]=rand()%10000;
        m_Mercs.push_back(merc(330,40,150,100,100,price_init,noise,tex_buttons,tex_decals,0,39,0,28,tex_font,pSoundEngine,"UNKNOWN"));
        price_init=100.0; noise[0]=0.900; noise[1]=0.100; noise[2]=price_init; noise[3]=8; noise[4]=rand()%10000;
        m_Mercs.push_back(merc(490,40,150,100,100,price_init,noise,tex_buttons,tex_decals,0,39,0,28,tex_font,pSoundEngine,"UNKNOWN"));
        price_init=100.0; noise[0]=0.900; noise[1]=0.100; noise[2]=price_init; noise[3]=8; noise[4]=rand()%10000;
        m_Mercs.push_back(merc(10,150,150,100,100,price_init,noise,tex_buttons,tex_decals,0,39,0,28,tex_font,pSoundEngine,"UNKNOWN"));
        price_init=100.0; noise[0]=0.900; noise[1]=0.100; noise[2]=price_init; noise[3]=8; noise[4]=rand()%10000;
        m_Mercs.push_back(merc(170,150,150,100,100,price_init,noise,tex_buttons,tex_decals,0,39,0,28,tex_font,pSoundEngine,"UNKNOWN"));
        price_init=100.0; noise[0]=0.900; noise[1]=0.100; noise[2]=price_init; noise[3]=8; noise[4]=rand()%10000;
        m_Mercs.push_back(merc(330,150,150,100,100,price_init,noise,tex_buttons,tex_decals,0,39,0,28,tex_font,pSoundEngine,"UNKNOWN"));
        price_init=100.0; noise[0]=0.900; noise[1]=0.100; noise[2]=price_init; noise[3]=8; noise[4]=rand()%10000;
        m_Mercs.push_back(merc(490,150,150,100,100,price_init,noise,tex_buttons,tex_decals,0,39,0,28,tex_font,pSoundEngine,"UNKNOWN"));
        price_init=100.0; noise[0]=0.900; noise[1]=0.100; noise[2]=price_init; noise[3]=8; noise[4]=rand()%10000;
        m_Mercs.push_back(merc(10,260,150,100,100,price_init,noise,tex_buttons,tex_decals,0,39,0,28,tex_font,pSoundEngine,"UNKNOWN"));
        price_init=100.0; noise[0]=0.900; noise[1]=0.100; noise[2]=price_init; noise[3]=8; noise[4]=rand()%10000;
        m_Mercs.push_back(merc(170,260,150,100,100,price_init,noise,tex_buttons,tex_decals,0,39,0,28,tex_font,pSoundEngine,"UNKNOWN"));
        price_init=100.0; noise[0]=0.900; noise[1]=0.100; noise[2]=price_init; noise[3]=8; noise[4]=rand()%10000;
        m_Mercs.push_back(merc(330,260,150,100,100,price_init,noise,tex_buttons,tex_decals,0,39,0,28,tex_font,pSoundEngine,"UNKNOWN"));
        price_init=100.0; noise[0]=0.900; noise[1]=0.100; noise[2]=price_init; noise[3]=8; noise[4]=rand()%10000;
        m_Mercs.push_back(merc(490,260,150,100,100,price_init,noise,tex_buttons,tex_decals,0,39,0,28,tex_font,pSoundEngine,"UNKNOWN"));
        price_init=100.0; noise[0]=0.900; noise[1]=0.100; noise[2]=price_init; noise[3]=8; noise[4]=rand()%10000;
        m_Mercs.push_back(merc(10,370,150,100,100,price_init,noise,tex_buttons,tex_decals,0,39,0,28,tex_font,pSoundEngine,"UNKNOWN"));
        price_init=100.0; noise[0]=0.900; noise[1]=0.100; noise[2]=price_init; noise[3]=8; noise[4]=rand()%10000;
        m_Mercs.push_back(merc(170,370,150,100,100,price_init,noise,tex_buttons,tex_decals,0,39,0,28,tex_font,pSoundEngine,"UNKNOWN"));
        price_init=100.0; noise[0]=0.900; noise[1]=0.100; noise[2]=price_init; noise[3]=8; noise[4]=rand()%10000;
        m_Mercs.push_back(merc(330,370,150,100,100,price_init,noise,tex_buttons,tex_decals,0,39,0,28,tex_font,pSoundEngine,"UNKNOWN"));
        price_init=100.0; noise[0]=0.900; noise[1]=0.100; noise[2]=price_init; noise[3]=8; noise[4]=rand()%10000;
        m_Mercs.push_back(merc(490,370,150,100,100,price_init,noise,tex_buttons,tex_decals,0,39,0,28,tex_font,pSoundEngine,"UNKNOWN"));
    }
    m_next_time=0; //temp

    //Inventory
    m_Inventory=inventory(655,40,136,425,16,tex_decals,tex_font);

    //Console
    m_Console=console(10,480,630,150,60,6,tex_font,tex_decals,0,0,630,150,pSoundEngine);

    //Highscore
    m_Highscore=highscore(650,476,135,150,0,2,tex_font,tex_buttons,0,0,135,150);

    //Quit window
    m_decQuitQuestion=decal( 165,250,480,72,tex_buttons,0,480,428,500 );

    m_value_sum=m_money_count=100;

    m_time_at_start=m_time_last_cycle=m_key_delay=0;
    m_show_quit_question=m_game_over_testing=false;
    m_sale_delay_timer=_sale_delay;

    m_ready=true;
    return true;
}

/*bool panel::reload_mercs(string merc_data)
{
    //clear old mercs
    m_Mercs.clear();

    //create file with new data
    ofstream created_file("data\\server_merc_data.txt");
    created_file<<merc_data;
    created_file.close();

    //read from new file
    ifstream file("data\\server_merc_data.txt");
    if(file)
    {
        float noise[_numof_mercs][5]; //default values
        int   unit_counter[_numof_mercs];
        //load data
        string line,word;
        getline(file,line); //skip first line (header)
        getline(file,line); //skip second line (header)
        for(int i=0;i<_numof_mercs;i++)
        {
            getline(file,line);//get line
            stringstream ss(line);
            for(int j=0;j<4;j++)// 4 values to read to noise
            {
                ss>>word;//get word
                float value=atof(word.c_str());//convert to float
                if(value<0) value=1;//error test
                noise[i][j]=value;//store
            }
            noise[i][4]=rand()%10000;//get random
            ss>>word;//get word
            float value=atof(word.c_str());//convert to float
            if(value<1) value=1;//error test
            unit_counter[i]=int(value);
        }

        //construct mercs
        int row_counter=0;
        int column_counter=0;
        for(int i=0;i<_numof_mercs;i++)
        {
            int xpos=10+160*(i%4);
            int ypos=40+110*row_counter;
            int tex_xpos_min=40*i; int tex_xpos_max=40*i+39;
            int tex_ypos_min=0; int tex_ypos_max=28;
            m_Mercs.push_back(merc(xpos,ypos,150,100,unit_counter[i],noise[i][2],noise[i],m_tex_buttons,m_tex_decals,
                                   tex_xpos_min,tex_xpos_max,tex_ypos_min,tex_ypos_max,m_tex_font,m_pSoundEngine));
            column_counter++;
            if(column_counter>3)
            {
                row_counter++;
                column_counter=0;
            }
        }
    }
    file.close();

    return true;
}*/

bool panel::clear_mercs(void)
{
    for(int i=0;i<_numof_mercs;i++)
    {
        m_Mercs.clear();
    }

    return true;
}

float panel::get_price(int unit_type)
{
    return m_Mercs[unit_type].get_price();
}

float panel::get_price_init(int unit_type)
{
    return m_Mercs[unit_type].get_price_init();
}

bool panel::get_prices(float* float_array)
{
    for(int i=0;i<_numof_mercs;i++)
    {
        float_array[i+3]=m_Mercs[i].get_price();
    }

    return true;
}

bool panel::set_prices(float prices[],float time)
{
    for(int i=0;i<_numof_mercs;i++)
    {
        m_Mercs[i].set_price(prices[i],time);
        m_Inventory.update_price( i, m_Mercs[i].get_price() );
    }

    return true;
}

int panel::get_unit_counter(int unit_type)
{
    return m_Mercs[unit_type].get_unit_counter();
}

bool panel::refresh_unit_counter(int unit_type,int counter)//refresh values sent by server (for clients)
{
    return m_Mercs[unit_type].refresh_unit_counter(counter);
}

bool panel::set_unit_counter(string customer_name,int unit_type,int change)//for mercs
{
    //find player
    for(int i=0;i<(int)m_vPlayer_values.size();i++)
    {
        if( m_vPlayer_values[i].name == customer_name )
        {
            m_vPlayer_values[i].owned_units[unit_type]+=change;//update player vector
            break;
        }
    }

    m_Mercs[unit_type].set_unit_counter(change);

    return true;
}

bool panel::set_owned_unit_counter_for_player(string name, float unit_counter[_numof_mercs])
{
    for(int i=0;i<(int)m_vPlayer_values.size();i++)
    {
        if(m_vPlayer_values[i].name==name)
        {
            for(int j=0;j<_numof_mercs;j++)
            {
                m_vPlayer_values[i].owned_units[j]=unit_counter[j];
            }

            return true;
        }
    }

    return false;//player not found
}

bool panel::set_unit_counter_init(int unit_type,int new_value)//for mercs
{
    m_Mercs[unit_type].set_unit_counter_init(new_value);

    return true;
}

bool panel::set_merc_price_init(int unit_type,float price)
{
    m_Mercs[unit_type].set_price_init(price);

    return true;
}

bool panel::draw_panel(void)
{
    for(int i=0;i<(int)m_Mercs.size();i++)
    {
        m_Mercs[i].draw();
    }
    m_Inventory.draw();
    m_Console.draw();
    m_Highscore.draw();

    if(m_show_quit_question) m_decQuitQuestion.draw();

    return true;
}

int panel::update2(bool keys[],int mouse_pos[],bool mouse_but[],float time,float sale_data[],string& message_to_send)
{
    m_time_last_cycle=m_time;
    m_time=time;

    if(m_sale_delay_timer>0) m_sale_delay_timer-=time-m_time_last_cycle;
    if(m_key_delay>0) m_key_delay-=time-m_time_last_cycle;

    //quit test
    if(keys[27] && m_key_delay<=0)//toggle quit screen
    {
        //play sound
        m_pSoundEngine->playSimpleSound(wav_button_press,1.0f);
        m_show_quit_question=!m_show_quit_question;
        m_key_delay=_key_delay;
    }
    if(keys[13] && m_key_delay<=0)//confirm quit
    {
        //play sound
        m_pSoundEngine->playSimpleSound(wav_button_press,1.0f);

        m_key_delay=_key_delay;
        if(m_show_quit_question) return 10;//quit
    }

    //update mercs
    for(int i=0;i<(int)m_Mercs.size();i++)
    {
        if(m_Mercs[i].interaction_test(mouse_pos,mouse_but,time,sale_data))
        {
            //general merc timer delay
            if(m_sale_delay_timer>0)//not ready due to recent sale
            {
                return 0;
            }

            //specific merc timer delay
            if(sale_data[1]==0)//not ready to buy jet, cooldown
            {
                return 5;
            }
            //sale happend, sale_data[1] stores unit counter
            m_sale_delay_timer=_sale_delay;
            sale_data[0]=i; //send merc id
            return 1;//sale request happend
        }
    }

    if(m_game_over_testing)
    {
        string winner("");
        if( game_over_test(winner) )//test if we have a winner
        {
            message_to_send=winner;
            return 2;
        }
    }

    string bought_player("");
    int retVal=m_Highscore.interaction_test(mouse_pos,mouse_but,time,bought_player); //key test
    switch(retVal)
    {
        case -1://error
        {
            cout<<"ERROR Highscore was not loaded properly\n";
            return -1;
        }break;

        case 1://buyout
        {
            message_to_send=bought_player;
            return 3;
        }break;

        case 2://change sorting (only for sound)
        {
            //play sound
            m_pSoundEngine->playSimpleSound(wav_button_press,1.0f);
        }break;
    }

    retVal=m_Console.update(keys,time,message_to_send); //key test
    switch(retVal)
    {
        case 1://message sent
        {
            return 4;
        }break;
    }

    return 0;
}

bool panel::update_prices(float time)//for server
{
    for(int i=0;i<_numof_mercs;i++)
    {
        m_Mercs[i].update_price(time);
        m_Inventory.update_price( i, m_Mercs[i].get_price() );
    }

    return true;
}

bool panel::update_inventory(int unit_type,int unit_counter_change)
{
    return m_Inventory.update_unit_count(unit_type,unit_counter_change);
}

bool panel::refresh_inventory(int unit_type,int counter)
{
    return m_Inventory.refresh_unit_count(unit_type,counter);
}

bool panel::update_inventory_wallet(float money,float value_sum)
{
    return m_Inventory.update_wallet(money,value_sum);
}

bool panel::update_player_highscore(string player_name, float money, float value_sum)
{
    //update local vector
    bool playerfound=false;
    for(int i=0;i<(int)m_vPlayer_values.size();i++)
    {
        if( m_vPlayer_values[i].name==player_name )
        {
            playerfound=true;
            m_vPlayer_values[i].money=money;
            m_vPlayer_values[i].value_sum=value_sum;
            break;
        }
    }
    if(!playerfound) return false;

    //update highscore
    float values[]={(int)money,(int)value_sum};//cut decimals
    return m_Highscore.update_values_for_player( player_name,values );
}

bool panel::add_new_player(string name,float money_value,float value_sum)
{
    st_player_values temp_player;
    temp_player.name=name;
    temp_player.money=money_value;
    temp_player.value_sum=value_sum;
    for(int i=0;i<_numof_mercs;i++) temp_player.owned_units[i]=0;//clear
    m_vPlayer_values.push_back(temp_player);

    return m_Highscore.new_player(name,money_value,value_sum);
}

bool panel::remove_player(string name)
{
    //reset that players owned units
    //find player
    for(int i=0;i<(int)m_vPlayer_values.size();i++)
    {
        if( m_vPlayer_values[i].name == name )
        {
            for(int j=0;j<_numof_mercs;j++)
            {
                int units_counter=m_vPlayer_values[i].owned_units[j];
                if(units_counter>0)
                {
                    m_vPlayer_values[i].owned_units[j]-=units_counter;//update player vector
                    m_Mercs[j].set_unit_counter(-units_counter);//give back units
                }
            }
            break;
        }
    }

    for(int i=0;i<(int)m_vPlayer_values.size();i++)
    {
        if( m_vPlayer_values[i].name==name )
        {
            m_vPlayer_values.erase( m_vPlayer_values.begin()+i );
            break;
        }
    }

    return m_Highscore.remove_player(name);
}

bool panel::text_to_console(string sender,string text_message)
{
    bool retVal=false;

    if(sender=="") retVal=m_Console.input_text(text_message);
    else retVal=m_Console.input_text(sender,text_message);

    return retVal;
}

bool panel::set_win_conditions(int game_mode,int game_goal)
{
    m_game_mode=game_mode;
    m_game_goal=game_goal;

    return true;
}

bool panel::game_over_test(string& winner)
{
    switch(m_game_mode)
    {
        case 0://monopoly
        {
            //test if all units have been bought
            bool out_of_units=true;;
            for(int i=0;i<_numof_mercs;i++)
            {
                if( m_Mercs[i].get_unit_counter() > 0 )
                {
                    out_of_units=false;
                    break;
                }
            }

            if(out_of_units)//test if only one owner
            {

                //get start unit count per merc
                int unit_counter_init[_numof_mercs];
                for(int i=0;i<_numof_mercs;i++)
                {
                    unit_counter_init[i]=m_Mercs[i].get_unit_counter_init();
                }
                //reset score
                for(int i=0;i<(int)m_vPlayer_values.size();i++)
                {
                    m_vPlayer_values[i].score=0;
                }
                //get unit count owned by player
                for(int i=0;i<(int)m_vPlayer_values.size();i++)
                {
                    for(int j=0;j<_numof_mercs;j++)
                    {
                        if( m_vPlayer_values[i].owned_units[j]==unit_counter_init[j] )
                        {
                            m_vPlayer_values[i].score++;//player gets one point
                        }
                    }
                }
                //test if one has all units
                for(int i=0;i<(int)m_vPlayer_values.size();i++)
                {
                    if( m_vPlayer_values[i].score==_numof_mercs )
                    {
                        winner=m_vPlayer_values[i].name;
                        return true;
                    }
                }

            }

        }break;

        case 1://buyout
        {
            //test if all units have been bought
            bool game_over=true;;
            for(int i=0;i<_numof_mercs;i++)
            {
                if( m_Mercs[i].get_unit_counter() > 0 )
                {
                    game_over=false;
                    break;
                }
            }

            if(game_over)//find winner
            {
                //reset score
                for(int i=0;i<(int)m_vPlayer_values.size();i++)
                {
                    m_vPlayer_values[i].score=0;
                }
                //get start unit count per merc
                int unit_counter_init[_numof_mercs];
                for(int i=0;i<_numof_mercs;i++)
                {
                    unit_counter_init[i]=m_Mercs[i].get_unit_counter_init();
                }
                //get unit count owned by player
                for(int i=0;i<(int)m_vPlayer_values.size();i++)
                {
                    for(int j=0;j<_numof_mercs;j++)
                    {
                        if( m_vPlayer_values[i].owned_units[j]==unit_counter_init[j] )
                        {
                            m_vPlayer_values[i].score++;//player gets one point
                        }
                    }
                }
                //find highest score
                int highest_score=0;
                for(int i=0;i<(int)m_vPlayer_values.size();i++)
                {
                    if( m_vPlayer_values[i].score > highest_score )
                    {
                        highest_score=m_vPlayer_values[i].score;
                    }
                }
                //test if draw
                vector<string> winners;
                for(int i=0;i<(int)m_vPlayer_values.size();i++)
                {
                    if( m_vPlayer_values[i].score == highest_score )
                    {
                        winners.push_back( m_vPlayer_values[i].name );
                    }
                }
                if( (int)winners.size()==1 )//one winner
                {
                    winner=winners[0];
                }
                if( (int)winners.size()==0 )//no winner
                {
                    winner="Nobody";
                }
                if( (int)winners.size()>1 )//draw
                {
                    for(int i=0;i<(int)winners.size();i++)
                    {
                        winner.append(winners[i]);
                        if( i!=(int)winners.size()-1 )
                        {
                            winner.append(" & ");
                        }
                    }
                }

                return true;
            }
        }break;

        case 2://time
        {
            float end_time_min=m_game_goal;
            float time_now_min=m_time/60;

            if( time_now_min > end_time_min+m_time_at_start/60 )
            {
                //find player with highest value_sum
                float highest_value=0;
                for(int i=0;i<(int)m_vPlayer_values.size();i++)
                {
                    if( m_vPlayer_values[i].value_sum > highest_value )
                    {
                        highest_value=m_vPlayer_values[i].value_sum;
                    }
                }
                //test if draw
                vector<string> winners;
                for(int i=0;i<(int)m_vPlayer_values.size();i++)
                {
                    if( m_vPlayer_values[i].value_sum == highest_value )
                    {
                        winners.push_back( m_vPlayer_values[i].name );
                    }
                }
                if( (int)winners.size()==1 )//one winner
                {
                    winner=winners[0];
                }
                if( (int)winners.size()==0 )//no winner
                {
                    winner="Nobody";
                }
                if( (int)winners.size()>1 )//draw
                {
                    for(int i=0;i<(int)winners.size();i++)
                    {
                        winner.append(winners[i]);
                        if( i!=(int)winners.size()-1 )
                        {
                            winner.append(" & ");
                        }
                    }
                }

                return true;
            }
        }break;

        case 3://money
        {
            bool game_over=false;
            float money_goal=m_game_goal;
            for(int i=0;i<(int)m_vPlayer_values.size();i++)
            {
                if( m_vPlayer_values[i].value_sum >= money_goal )
                {
                    game_over=true;
                }
            }
            if(game_over)
            {
                //find highest value sum
                float highest_value=0;
                for(int i=0;i<(int)m_vPlayer_values.size();i++)
                {
                    if( m_vPlayer_values[i].value_sum > highest_value )
                    {
                        highest_value=m_vPlayer_values[i].value_sum;
                    }
                }
                //test if draw
                vector<string> winners;
                for(int i=0;i<(int)m_vPlayer_values.size();i++)
                {
                    if( m_vPlayer_values[i].value_sum == highest_value )
                    {
                        winners.push_back( m_vPlayer_values[i].name );
                    }
                }
                if( (int)winners.size()==1 )//one winner
                {
                    winner=winners[0];
                }
                if( (int)winners.size()==0 )//no winner
                {
                    winner="Nobody";
                }
                if( (int)winners.size()>1 )//draw
                {
                    for(int i=0;i<(int)winners.size();i++)
                    {
                        winner.append(winners[i]);
                        if( i!=(int)winners.size()-1 )
                        {
                            winner.append(" & ");
                        }
                    }
                }
                return true;
            }
        }break;
    }

    return false;
}

bool panel::set_win_conditions_testing_flag(bool flag)
{
    m_game_over_testing=flag;
    return true;
}

string panel::get_merc_name(int unit_type)
{
    return m_Mercs[unit_type].get_name();
}

bool panel::set_start_time_sec(float time_at_start)
{
    m_time_at_start=time_at_start;
    return true;
}


