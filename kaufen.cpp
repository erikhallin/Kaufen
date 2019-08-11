#include "kaufen.h"

#include <windows.h> //for sleep between sends

/* TO-DO



*/

kaufen::kaufen()
{
    m_ready=false;
    m_textures_ready=false;
    m_hosting=false;
    m_client_flag_playerlist=false;
    m_client_flag_soc_reply=false;
    m_client_flag_server_inventory=false;
    m_money_start=100;
    m_my_soc_id=0;
    m_time_next_resend=0;
    m_resend_data_id_counter=0;
    m_conf_mode=false;
    m_state=st_init;
}

kaufen::~kaufen()
{
    delete m_pSoundEngine;
}

bool kaufen::init(bool keys[256],bool mouse_but[4],int mouse_pos[2],int window_width,int window_height)
{
    //temp

    /*string stemp("test");
    float fval[256];
    memcpy(fval,stemp.c_str(),256);

    cout<<"f[1] : "<<fval[1]<<endl;;
    char carr[256];
    memcpy(carr,fval,256);
    cout<<"rest: "<<carr<<endl;*/

    //xxxx

    cout<<"WallStreeting Version "<<_version<<endl;

    //trigger network blocking
    if( !m_NetCom.block_trap() ) cout<<"ERROR Problem with network\n";

    m_default_port=5001;
    m_check_for_broadcast_reply=false;
    m_my_money_count=_start_money;
    m_my_value_sum=_start_money;

    m_window_width=window_width;
    m_window_height=window_height;

    //get pointers to arrays for input
    m_pKeys=keys;
    m_pMouse_but=mouse_but;
    m_pMouse_pos=mouse_pos;

    //load textures
    load_textures();
    //load sounds
    load_sounds();

    //create lobby
    m_Lobby.init(m_tex_font,m_tex_buttons,m_tex_decals,window_width,window_height,m_pSoundEngine);
    //create the panel
    m_Panel.init(m_tex_font,m_tex_buttons,m_tex_decals,m_pSoundEngine);
    //create the main menu
    m_Menu.init(window_width,window_height,m_tex_menu,m_tex_font,m_tex_help,m_pSoundEngine,(float)clock()/CLOCKS_PER_SEC,_version);
    //create the game over screen
    m_GameOver.init(window_width,window_height,m_tex_buttons,m_tex_font,m_tex_gameover);

    //init logfile if debugmode
    if(m_conf_mode) m_Log.init();

    m_state=st_menu;
    m_server_state=0;
    m_time_last_inv_refresh_sent=m_time_last_data_refresh_sent=m_time_last_show_data=m_time_this_cycle=m_time_last_cycle=0;
    m_curr_player_for_inv_broadcast=0;
    m_resend_data_id_counter=1;

    m_ready=true;

    return true;
}

bool kaufen::init_network(string net_status)
{
    if(!m_NetCom.init(net_status)) return false;

    if(net_status=="server")
    {
        if(!m_NetCom.set_port_and_bind(m_default_port)) return false;

        m_hosting=true;
    }
    if(net_status=="client")
    {
        m_hosting=false;
    }

    return true;
}

bool kaufen::start_hosting(void) //for server
{
    m_ping_delay=0.5; //0.5 sec
    m_ping_last_time=0;

    //tell panel to test for winner
    m_Panel.set_win_conditions_testing_flag(true);

    m_server_state=st_lobby;
    m_server_name=m_Menu.get_server_name();
    m_my_name=m_Menu.get_player_name();
    m_my_soc_id=m_NetCom.get_server_socket();

    m_Players.clear();//to be safe

    //store host player
    st_player player_temp;
    player_temp.soc_id = m_NetCom.get_server_socket();
    player_temp.name = m_Menu.get_player_name();
    player_temp.money= m_my_money_count;
    player_temp.value_sum= m_my_value_sum;
    player_temp.replied_sec_ago=0;
    player_temp.last_ping=0;
    player_temp.time_out=5;
    for(int i=0;i<_numof_mercs;i++) player_temp.owned_units[i]=0;

    m_Panel.add_new_player(player_temp.name,m_money_start,m_money_start); //update highscore

    m_Players.push_back(player_temp);

    return m_NetCom.start_to_listen(10);
}

bool kaufen::send_start_package(SOCKET SocReceiver)
{
    float server_inventory[_numof_mercs*2+8];
    for(int i=0;i<_numof_mercs;i++)
    {
        server_inventory[i+8]=m_Panel.get_unit_counter(i);//units left
        server_inventory[i+_numof_mercs+8]=m_Panel.get_price_init(i);//merc init price
    }
    server_inventory[1]=1010;
    server_inventory[2]=m_state;//3 = lobby, 4 = in game
    server_inventory[3]=m_game_mode;
    server_inventory[4]=m_game_goal;
    //m_server_soc_id will be the soc_id for clients, but they can not send to that socket, this value have to be updated when it have reached the client
    server_inventory[5]=m_server_soc_id=m_my_soc_id;
    server_inventory[6]=m_tick_time_at_game_start;
    server_inventory[7]=_version;
    server_inventory[0]=sizeof(server_inventory);
    Sleep(10);
    m_NetCom.send_data(server_inventory,SocReceiver);

    //create player list
    int counter=0;
    int max_size=10;
    string player_list("pld\n");
    for(int i=0;i<(int)m_Players.size();i++)
    {
        //name
        player_list.append(m_Players[i].name);
        player_list.append(1,' ');
        //soc_id
        stringstream ss;
        ss<<m_Players[i].soc_id;
        string sSoc_id=ss.str();
        player_list.append(sSoc_id);
        player_list.append(1,' ');
        //money
        stringstream ss0;
        ss0 << m_Players[i].money;
        player_list.append( ss0.str() );
        player_list.append(1,' ');
        //owned units
        for(int j=0;j<_numof_mercs;j++)
        {
            stringstream ss1;
            ss1 << m_Players[i].owned_units[j];
            player_list.append( ss1.str() );
            player_list.append(1,' ');
        }

        //maximum size reached?
        if(++counter>max_size)
        {
            Sleep(10);
            m_NetCom.send_data(player_list,SocReceiver);

            if(i!=(int)m_Players.size()-1)//prepare string for next player_list
            {
                player_list.clear();
                player_list.append("player_data\n");
                counter=0;
            }
            else player_list.clear();
        }
        else player_list.append(1,'\n');//next player

        //cout<<player_list<<endl;//temp
    }
    if( !player_list.empty() )
    {
        Sleep(10);
        m_NetCom.send_data(player_list,SocReceiver);
    }

    return true;
}

SOCKET kaufen::get_socket(void)
{
    return m_NetCom.get_server_socket();
}

bool kaufen::connect_to_server(void) //(for clients)
{
    string sIP=m_Menu.get_IP(); //get ip from tip
    m_my_name=m_Menu.get_player_name(); //get player name

    cout<<"Try to connect to IP: "<<sIP<<endl; //TEMP

    return m_NetCom.connect_to_server(sIP,m_default_port);
}

bool kaufen::connect_to_client(SOCKET new_client)//adding new client (for server)
{
    if( m_NetCom.add_client(new_client) )//will update incomming socket to accepted
    {
        //store in vector
        st_player player_temp;
        player_temp.soc_id = new_client;
        player_temp.name = "unknown";
        player_temp.money= m_money_start;
        player_temp.value_sum= 0;
        player_temp.time_out= 5;
        player_temp.replied_sec_ago=0;
        for(int i=0;i<_numof_mercs;i++) player_temp.owned_units[i]=0;
        m_Players.push_back(player_temp);

        //send start package
        if( send_start_package(new_client) ) cout<<"start package was sent to new client\n";
        else cout<<"ERROR sending start package!\n";


        return true;
    }

    return false;
}

bool kaufen::test_connection(void)
{
    return m_NetCom.test_connection();
}

bool kaufen::send_data(float* data_array)
{
    Sleep(10);
    bool data_sent=m_NetCom.send_data(data_array);

    return data_sent;
}

bool kaufen::recv_data(SOCKET SocSender)
{
    float* data_array = new float[256];

    bool data_recv=false;
    switch ( m_NetCom.get_status() )
    {
        case net_server:
        {
            data_recv=m_NetCom.recv_data(data_array,SocSender);
        }break;

        case net_client:
        {
            data_recv=m_NetCom.recv_data(data_array);
        }break;

        case net_error:
        {
            data_recv=false;
        }break;
    }

    interpret_data(data_array,SocSender);

    delete[] data_array;

    return data_recv;
}

bool kaufen::broadcast_to_server(void)
{
    return m_NetCom.broadcast_my_ip();
}

bool kaufen::check_for_broadcast_reply(void)
{
    return m_NetCom.check_for_broadcast_reply();
}

bool kaufen::check_for_broadcast(void)
{
    return m_NetCom.check_for_broadcast();
}



bool kaufen::start_joined_game(void) //for client
{
    if( m_client_flag_playerlist && m_client_flag_soc_reply && m_client_flag_server_inventory && (float)_version==m_server_version )
    {
        switch (m_server_state)
        {
            case st_lobby:
            {
                cout<<"Joining server Lobby\n";
                m_state=st_lobby;
            }break;

            case st_running:
            {
                cout<<"Joining server in-game\n";
                m_state=st_running;
            }break;

            default:
            {
                cout<<"ERROR problem joining server\n";
                return false;
            }
        }

        return true;
    }
    else
    {
        cout<<"ERROR server data is missing:\n";
        if(!m_client_flag_playerlist) cout<<" Player List not received\n";
        if(!m_client_flag_server_inventory) cout<<" Server Inventory not received\n";
        if(!m_client_flag_soc_reply) cout<<" Client ID not received\n";
        if( (float)_version!=m_server_version) cout<<" Not running same version as server\n"<<m_server_version<<" != "<<_version<<endl;

        //leave server
        clean_up();
        m_server_state=0;
        m_state=st_menu;
    }

    return false;
}

bool kaufen::load_sounds(void)
{
    m_pSoundEngine=new sound();

    m_pSoundEngine->loadWAVE(wav_badsound,"data\\sound\\badsound.wav");
    m_pSoundEngine->loadWAVE(wav_beep1,"data\\sound\\beep1.wav");
    m_pSoundEngine->loadWAVE(wav_button_press,"data\\sound\\button_press.wav");
    m_pSoundEngine->loadWAVE(wav_buyout,"data\\sound\\buyout.wav");
    m_pSoundEngine->loadWAVE(wav_cashregister,"data\\sound\\cashregister.wav");
    m_pSoundEngine->loadWAVE(wav_disconnected,"data\\sound\\disconnected.wav");
    m_pSoundEngine->loadWAVE(wav_exit,"data\\sound\\exit.wav");
    m_pSoundEngine->loadWAVE(wav_joined,"data\\sound\\joined.wav");
    m_pSoundEngine->loadWAVE(wav_left,"data\\sound\\left.wav");
    m_pSoundEngine->loadWAVE(wav_lost,"data\\sound\\lost.wav");
    m_pSoundEngine->loadWAVE(wav_message,"data\\sound\\message.wav");
    m_pSoundEngine->loadWAVE(wav_no,"data\\sound\\no.wav");
    m_pSoundEngine->loadWAVE(wav_typing,"data\\sound\\typing.wav");
    m_pSoundEngine->loadWAVE(wav_win,"data\\sound\\win.wav");

    if(!m_pSoundEngine->m_ready || m_pSoundEngine->get_error()!=0)
    {
        cout<<"ERROR problem loading sound, error id: "<<m_pSoundEngine->get_error()<<endl;
        return false;
    }

    return true;
}

bool kaufen::remove_player(string player_name)//for clients and servers, will not be broadcasted by server
{
    cout<<"Request to remove player: "<<player_name<<endl;
    //find player name
    int player_id=-1;
    for(int i=0;i<(int)m_Players.size();i++)
    {
        if(m_Players[i].name == player_name)
        {
            player_id=i;
            break;
        }
    }
    if(player_id<0) return false;

    //remove from highscore
    m_Panel.remove_player(m_Players[player_id].name);

    if(m_state==st_lobby)//remove from lobby
    {
        m_Lobby.remove_player(m_Players[player_id].name);
    }

    if(m_hosting)
    {
        //removing unsend packages
        for(int i=0;i<(int)m_vPackages.size();i++)
        {
            if(m_vPackages[i].socReceiver==m_Players[player_id].soc_id)
            {
                m_vPackages.erase( m_vPackages.begin() + i );
                //restart loop
                i--;
                continue;
            }
        }
    }

    m_Players.erase( m_Players.begin() + player_id );

    return true;
}

bool kaufen::remove_player(SOCKET SocClient)//for servers and clients
{
    if(m_state==st_menu) return false;//lost connection from old server (ignore)

    cout<<"Request to remove player: ";
    //get name from socket
    int player_id=-1;
    for(int i=0;i<(int)m_Players.size();i++)
    {
        if(m_Players[i].soc_id == SocClient)
        {
            cout<<m_Players[i].name<<endl;
            player_id=i;
            break;
        }
    }
    if(player_id<0)
    {
        cout<<"\nERROR player id not found for removal, OK if a player recently left\n";

        //if client sees this, probably server lost
        if(!m_hosting)//clients
        {
            cout<<"Lost connection to the Server!\n";
            if(m_state==st_gameover)
            {
                //dont turn off game over screen
                m_server_state=0;
                m_NetCom.clean_up();//close connection to server
            }
            else
            {
                //play sound
                m_pSoundEngine->playSimpleSound(wav_disconnected,1.0f);
                //clean up
                m_server_state=0;
                m_state=st_menu;
                clean_up();
            }
        }

        return false;
    }

    //remove from highscore
    m_Panel.remove_player(m_Players[player_id].name);

    if(m_state==st_lobby)//remove from lobby
    {
        m_Lobby.remove_player(m_Players[player_id].name);
    }

    if(m_hosting)
    {
        //removing unsend packages
        for(int i=0;i<(int)m_vPackages.size();i++)
        {
            if(m_vPackages[i].socReceiver==m_Players[player_id].soc_id)
            {
                m_vPackages.erase( m_vPackages.begin() + i );
                //restart loop
                i=-1;
                continue;
            }
        }

        if( m_NetCom.remove_client(SocClient) )//remove client socket
        {
            cout<<"Player with Socket: "<<(int)SocClient<<" was removed\n";
        }
        else
        {
            cout<<"ERROR could not find client's socket for removal\n";
        }
        //broadcast that player left
        float player_left[]={0,1004,SocClient,m_resend_data_id_counter};
        m_resend_data_id_counter++;
        player_left[0]=sizeof(player_left);

        if(m_conf_mode)
        {
            //store for all players
            for(int i=0;i<(int)m_Players.size();i++)
            {
                if( (int)m_Players[i].soc_id==m_my_soc_id) continue; //dont send to yourself
                if( (int)m_Players[i].soc_id==(int)SocClient )  continue; //and dont tell the player that left

                float* pdata = new float[4];
                pdata[0]=sizeof(pdata);
                pdata[1]=1004;
                pdata[2]=SocClient;
                pdata[3]=player_left[3];

                st_package temp_package;
                temp_package.id=player_left[3];
                temp_package.socReceiver=m_Players[i].soc_id;
                temp_package.pData=pdata;
                temp_package.time_left=2;

                m_vPackages.push_back(temp_package);
                m_Log.log("Packtype:",(int)pdata[1],"ID:",(int)temp_package.id);
            }
        }
        Sleep(10);
        m_NetCom.send_data(player_left);//to all clients
    }

    //remove last trace of player
    m_Players.erase( m_Players.begin() + player_id );

    //play sound
    m_pSoundEngine->playSimpleSound(wav_left,1.0f);

    return true;
}

bool kaufen::remove_nonjoining_player(SOCKET SocClient)//for servers to kick player that did not join
{
    cout<<"Joining player never joined, ";
    for(int i=0;i<(int)m_Players.size();i++)
    {
        if(m_Players[i].soc_id == SocClient)
        {
            cout<<"removing that player\n";
            //removing unsend packages
            for(int j=0;j<(int)m_vPackages.size();j++)
            {
                if(m_vPackages[j].socReceiver==m_Players[i].soc_id)
                {
                    m_vPackages.erase( m_vPackages.begin() + j );
                    //restart loop
                    j=-1;
                    continue;
                }
            }
            m_Players.erase( m_Players.begin() + i );
            return true;
        }
    }
    cout<<"could not find that player\n";
    return false;
}

bool kaufen::getInput(bool keys[],bool mouse_but[4],int mouse_pos[2])
{

    return true;
}

//MAIN PROGRAM CYCLE
int kaufen::cycle(void)
{
    if(!m_ready) return -1;

    float time=(float)clock()/CLOCKS_PER_SEC;
    m_time_last_cycle=m_time_this_cycle;
    m_time_this_cycle=time;

    switch (m_state)
    {
        case st_menu:
        {
            int ret_val=m_Menu.update(m_pKeys,m_pMouse_pos,m_pMouse_but,time);
            switch (ret_val)
            {
                case 1://hosted game
                {
                    //check for name
                    string player_name=m_Menu.get_player_name();
                    if( player_name.empty() )
                    {//no name set
                        m_Menu.info_missing("player_name");
                        //play sound
                        m_pSoundEngine->playSimpleSound(wav_no,1.0f);
                        return 0;
                    }
                    //test forbidden names
                    if( player_name=="unknown" )
                    {
                        m_Menu.set_player_name("Forbidden name");
                        //play sound
                        m_pSoundEngine->playSimpleSound(wav_badsound,1.0f);
                        return 0;
                    }

                    //check for server name
                    string server_name=m_Menu.get_server_name();
                    if( server_name.empty() )
                    {//no name set
                        m_Menu.info_missing("server_name");
                        //play sound
                        m_pSoundEngine->playSimpleSound(wav_no,1.0f);
                        return 0;
                    }

                    m_state=st_lobby;//st_running;
                    m_Lobby.set_server_flag(true);//allows changes in game mode settings
                    m_Lobby.add_player(player_name);

                    //play sound
                    m_pSoundEngine->playSimpleSound(wav_button_press,1.0f);

                    return 1;
                }break;

                case 2://joining game
                {
                    //check for name
                    string player_name=m_Menu.get_player_name();
                    if( player_name.empty() )
                    {//no name set
                        m_Menu.info_missing("player_name");
                        //play sound
                        m_pSoundEngine->playSimpleSound(wav_badsound,1.0f);
                        return 0;
                    }
                    //test forbidden names
                    if( player_name=="unknown" )
                    {
                        m_Menu.set_player_name("Forbidden name");
                        //play sound
                        m_pSoundEngine->playSimpleSound(wav_badsound,1.0f);
                        return 0;
                    }

                    string sIP=m_Menu.get_IP(); //get ip from tip
                    char cIP[30]={0};
                    strcpy( cIP,sIP.c_str() );//non constant char* needed
                    if( !ip_is_valid(cIP) )
                    {//invalid IP
                        m_Menu.info_missing("IP_number");
                        //play sound
                        m_pSoundEngine->playSimpleSound(wav_badsound,1.0f);
                        return 0;
                    }

                    //play sound
                    m_pSoundEngine->playSimpleSound(wav_button_press,1.0f);

                    return 2;
                }break;

                case 3://show help
                {
                    //play sound
                    m_pSoundEngine->playSimpleSound(wav_button_press,1.0f);
                    return 3;
                }break;

                case 4://exit game
                {
                    //play sound
                    m_pSoundEngine->playSimpleSound(wav_button_press,1.0f);
                    Sleep(100);
                    return 4;
                }break;

                case 5://search LAN
                {
                    m_NetCom.init("client");
                    m_hosting=false;
                    m_NetCom.broadcast_my_ip();
                    m_check_for_broadcast_reply=true;

                    //play sound
                    m_pSoundEngine->playSimpleSound(wav_button_press,1.0f);

                    return 5;
                }break;
            }

            if(!m_hosting)//for client
            {
                //check for broadcast reply from servers
                if(m_check_for_broadcast_reply)
                {
                    static int counter(0);//timeout counter
                    if( m_NetCom.check_for_broadcast_reply() )
                    {
                        m_Menu.set_IP( m_NetCom.get_server_IP() );
                        m_check_for_broadcast_reply=false;
                    }
                    else if(++counter>5000)//give up
                    {
                        m_check_for_broadcast_reply=false;
                        counter=0;
                    }
                    cout<<counter<<endl;
                }
            }

            m_Menu.draw();
        }break;

        case st_lobby:
        {
            if(m_hosting)
            {
                m_NetCom.check_for_broadcast(); //check for searching clients

                if(time>m_ping_last_time+m_ping_delay)
                {//send ping to clients
                    m_ping_last_time=time;
                    send_lobby_ping();

                }

                //check for lost/inactive players
                lost_player_test();
            }

            string message_to_send("");//will contain message from console
            int ret_val=m_Lobby.update(m_pKeys,m_pMouse_pos,m_pMouse_but,time,message_to_send);
            switch(ret_val)
            {
                case 0://no update required
                {
                    ;
                }break;

                case 1://game mode update
                {
                    //play sound
                    m_pSoundEngine->playSimpleSound(wav_button_press,1.0f);

                    if(m_hosting)
                    {
                        m_game_mode=m_Lobby.get_game_mode();
                        m_game_goal=m_Lobby.get_game_goal();
                        float game_data[]={ 0 , 1014 , m_game_mode , m_game_goal };
                        game_data[0]=sizeof(game_data);
                        Sleep(10);
                        m_NetCom.send_data(game_data);
                    }
                }break;

                case 2://leave
                {
                    //play sound
                    m_pSoundEngine->playSimpleSound(wav_button_press,1.0f);

                    if(m_hosting)
                    {
                        float server_down[]={0,1015};
                        server_down[0]=sizeof(server_down);
                        Sleep(10);
                        m_NetCom.send_data(server_down);//to clients

                        clean_up();
                        m_state=st_menu;
                        m_server_state=0;
                    }
                    else//client
                    {
                        //report that you (client) are leaving
                        float player_left[]={0,1004,m_my_soc_id};
                        player_left[0]=sizeof(player_left);
                        Sleep(10);
                        m_NetCom.send_data(player_left);//to server

                        clean_up();
                        m_state=st_menu;
                        m_server_state=0;
                    }
                }break;

                case 3://ready/start
                {
                    //play sound
                    m_pSoundEngine->playSimpleSound(wav_button_press,1.0f);

                    if(m_hosting)
                    {
                        //start the game
                        server_starts_the_game();//tells the clients to
                    }
                    else//client
                    {
                        //tell server that client is ready
                        float client_data[]={0,1016};
                        client_data[0]=sizeof(client_data);
                        Sleep(10);
                        m_NetCom.send_data(client_data);//to server
                    }
                }break;

            }

            if( message_to_send!="" ) //message was sent
            {
                if(m_hosting)//for server, send to all clients
                {
                    //send text
                    string console_message("tfs ");
                    console_message.append(m_my_name);
                    console_message.append(" - ");
                    console_message.append(message_to_send);
                    Sleep(10);
                    m_NetCom.send_data(console_message);
                    //update console
                    m_Lobby.text_to_console(m_my_name,message_to_send);
                    //play sound
                    m_pSoundEngine->playSimpleSound(wav_message,1.0f);
                }
                else//for clients, send to server
                {
                    //send text
                    string console_message("tts ");
                    console_message.append(m_my_name);
                    console_message.append(" - ");
                    console_message.append(message_to_send);
                    Sleep(10);
                    m_NetCom.send_data(console_message);
                }
            }

            m_Lobby.draw();
        }break;

        case st_running:
        {
            if(m_hosting)
            {
                m_NetCom.check_for_broadcast(); //check for searching clients

                if(time>m_ping_last_time+m_ping_delay)
                {//send ping/price to clients
                    m_ping_last_time=time;
                    ping_clients();//and update prices
                }

                //refresh client's data about their inventory and server inventory
                send_data_refresh();
                send_inventory_refresh();

                //check for lost/inactive players
                lost_player_test();
            }

            show_data();//temp

            //test if packeges that needs to be resended
            if(m_conf_mode) resend_packages_test();

            float sale_data[2]={0};
            string message_to_send("");//will contain message from console
            int ret_val=m_Panel.update2(m_pKeys,m_pMouse_pos,m_pMouse_but,time,sale_data,message_to_send);
            switch(ret_val)
            {
                case 1://sale, send request to server
                {
                    cout<<"Sale request received...\n";

                    if(m_hosting)
                    {//server tests directly
                        if (server_test_sale_and_send(m_my_soc_id,sale_data[0],sale_data[1]) ) //player,type,counter
                         cout<<"Sale OK\n";
                        else
                        {
                            //play sound
                            m_pSoundEngine->playSimpleSound(wav_no,1.0f);
                        }
                    }
                    else
                    {//client sends request to server
                        if( client_test_sale_and_send( (int)sale_data[0], (int)sale_data[1] ) )
                         cout<<"Local sale OK\n";
                        else
                        {
                            //play sound
                            m_pSoundEngine->playSimpleSound(wav_no,1.0f);
                        }
                    }

                }break;

                case 2://game over, winner name in message_to_send
                {
                    if(m_hosting)
                    {
                        //send to clients
                        string winner_data("win ");
                        winner_data.append(message_to_send);

                        int tries=3; //number of times package is sent, to be sure that its not lost
                        for(int i=0;i<tries;i++)
                        {
                            Sleep(30);
                            m_NetCom.send_data(winner_data);
                        }

                        //adjust name length
                        int numof_spaces=( 10-(int)message_to_send.length() )/2;
                        if( numof_spaces>0 ) message_to_send.insert( message_to_send.begin(),numof_spaces,' ' );

                        m_GameOver.set_winner_name(message_to_send);
                        m_state=st_gameover;

                        //play sound
                        if(message_to_send==m_my_name)  m_pSoundEngine->playSimpleSound(wav_win,1.0f);
                        else                            m_pSoundEngine->playSimpleSound(wav_lost,1.0f);
                    }
                    else
                    {
                        cout<<"Error winner can not be tested by clients\n";
                    }

                    return 0;
                }break;

                case 3://buyout request, bought player in message_to_send
                {
                    //play sound
                    m_pSoundEngine->playSimpleSound(wav_button_press,1.0f);

                    int buyer_soc_id=-1;
                    int victim_soc_id=-1;

                    //find bought player
                    float victim_value=0;
                    for(int i=0;i<(int)m_Players.size();i++)
                    {
                        if(m_Players[i].name==message_to_send)
                        {
                            if(m_Players[i].name==m_my_name)
                            {
                                cout<<"Can not buyout yourself\n";
                                return 0;
                            }
                            else//valid player
                            {
                                victim_soc_id=m_Players[i].soc_id;
                                victim_value=m_Players[i].value_sum;
                                break;
                            }
                        }
                    }
                    //get buyer money
                    float buyer_money=0;
                    for(int i=0;i<(int)m_Players.size();i++)
                    {
                        if( (int)m_Players[i].soc_id==m_my_soc_id)
                        {
                            buyer_soc_id=m_Players[i].soc_id;
                            buyer_money=m_Players[i].money;
                            break;
                        }
                    }
                    //error test
                    if(buyer_soc_id==-1 || victim_soc_id==-1) return 0; //error, player not found
                    //test if possible, buyer have to have 2x the money as victims value sum
                    if( buyer_money > victim_value*2 )
                    {
                        float victim_value_x2=victim_value*2;
                        if(m_hosting)//if server, send confirmation to all clients
                        {
                            //update values locally
                            buyout_test(buyer_soc_id,victim_soc_id,victim_value_x2);//will send confirmation to clients if OK
                        }
                        else//if client, send request to server
                        {
                            //send request to server
                            float buyout_data[]={0,1018,buyer_soc_id,victim_soc_id,m_resend_data_id_counter};
                            m_resend_data_id_counter++;
                            buyout_data[0]=sizeof(buyout_data);

                            if(m_conf_mode)
                            {
                                //store package
                                float* pdata = new float[5];
                                pdata[0]=sizeof(pdata);
                                pdata[1]=1018;
                                pdata[2]=buyer_soc_id;
                                pdata[3]=victim_soc_id;
                                pdata[4]=buyout_data[4];

                                st_package temp_package;
                                temp_package.id=buyout_data[4];
                                temp_package.socReceiver=m_server_soc_id;
                                temp_package.pData=pdata;
                                temp_package.time_left=2;
                                m_vPackages.push_back(temp_package);
                                m_Log.log("Packtype:",(int)pdata[1],"ID:",(int)temp_package.id);
                            }

                            Sleep(10);
                            m_NetCom.send_data(buyout_data);
                        }
                    }

                }break;

                case 4://message from console
                {
                    if( message_to_send!="" )
                    {
                        bool ignore_message=false;
                        //test if message is a command
                        if( message_to_send=="/gamemode" )//show gamemode
                        {
                            string console_message;
                            switch(m_game_mode)
                            {
                                case 0: console_message="GameMode: Monopoly"; break;
                                case 1: console_message="GameMode: Buyout"; break;
                                case 2:
                                {
                                    console_message="GameMode: Time - ";
                                    stringstream ss;
                                    ss<<m_game_goal;
                                    console_message.append( ss.str() );
                                    if(m_game_goal==1) console_message.append(" minute");
                                    else               console_message.append(" minutes");
                                }break;
                                case 3:
                                {
                                    console_message="GameMode: Money - ";
                                    stringstream ss;
                                    ss<<m_game_goal;
                                    console_message.append( ss.str() );
                                }break;
                            }
                            m_Panel.text_to_console("SYSTEM",console_message);
                            ignore_message=true;
                        }
                        if( message_to_send=="/time" )//show time
                        {
                            float seconds = time-m_tick_time_at_game_start;
                            float minutes = seconds / 60;
                            minutes=(int)minutes;//cut decimals
                            stringstream ss;
                            ss<<minutes;
                            string console_message("Minutes since start: ");
                            if( minutes==0 ) console_message.append( "< 0" );
                            else             console_message.append( ss.str() );
                            m_Panel.text_to_console("SYSTEM",console_message);
                            ignore_message=true;
                        }

                        if(!ignore_message)//was not a command
                        {
                            if(m_hosting)//for server, send to all clients
                            {
                                //send text
                                string console_message("tfs ");
                                console_message.append(m_my_name);
                                console_message.append(" - ");
                                console_message.append(message_to_send);
                                Sleep(10);
                                m_NetCom.send_data(console_message);
                                //update console
                                m_Panel.text_to_console(m_my_name,message_to_send);
                                //play sound
                                m_pSoundEngine->playSimpleSound(wav_message,1.0f);
                            }
                            else//for clients, send to server
                            {
                                //send text
                                string console_message("tts ");
                                console_message.append(m_my_name);
                                console_message.append(" - ");
                                console_message.append(message_to_send);
                                Sleep(10);
                                m_NetCom.send_data(console_message);
                            }
                        }
                    }
                }break;

                case 5://buy delay
                {
                    //play sound
                    m_pSoundEngine->playSimpleSound(wav_button_press,1.0f);
                    //display message
                    int time_left=(int)sale_data[0]+1;
                    string message_to_display("Sale cooldown for another ");
                    stringstream ss;
                    ss<<time_left;
                    message_to_display.append( ss.str() );
                    message_to_display.append( " seconds" );
                    m_Panel.text_to_console("SYSTEM",message_to_display);
                }break;

                case 10://quit
                {
                    clean_up();
                    return 4;
                }break;
            }

            m_Panel.draw_panel();
        }break;

        case st_gameover:
        {
            int retVal=m_GameOver.update(m_pKeys,m_pMouse_pos,m_pMouse_but,time);
            if(retVal==1)//reset requested
            {
                //play sound
                m_pSoundEngine->playSimpleSound(wav_button_press,1.0f);

                m_state=st_menu;
                clean_up();
                Sleep(100);//dont press exit!
                return 0;
            }
            m_GameOver.draw();
        }break;
    }

    return 0;
}

bool kaufen::set_confirmation_mode(bool flag)
{
    m_conf_mode=flag;
    return true;
}




bool kaufen::load_textures(void)
{
    image* Image;
    //load buttons texture
    Image=loadBMP("data\\texture\\buttons.bmp");
    m_tex_buttons=loadTexture(Image);
    //load decals texture
    Image=loadBMP("data\\texture\\decals.bmp");
    m_tex_decals=loadTexture(Image);
    //load menu texture
    Image=loadBMP("data\\texture\\menu.bmp");
    m_tex_menu=loadTexture(Image);
    //load help texture
    Image=loadBMP("data\\texture\\help.bmp");
    m_tex_help=loadTexture(Image);
    //load game over texture
    Image=loadBMP("data\\texture\\gameover.bmp");
    m_tex_gameover=loadTexture(Image);
    //load fonts texture
    Image=loadBMP("data\\fonts\\default_light.bmp");
    m_tex_font[0]=loadTexture(Image);
    Image=loadBMP("data\\fonts\\default_dark.bmp");
    m_tex_font[1]=loadTexture(Image);
    Image=loadBMP("data\\fonts\\default_mask.bmp");
    m_tex_font[2]=loadTexture(Image);

    m_textures_ready=true;
    return true;
}


GLuint loadTexture(image* Image) //Makes the image into a texture, and returns the id of the texture
{
	GLuint textureId;
	glGenTextures(1, &textureId); //Make room for our texture
	glBindTexture(GL_TEXTURE_2D, textureId); //Tell OpenGL which texture to edit
	//Map the image to the texture
	glTexImage2D(GL_TEXTURE_2D,                //Always GL_TEXTURE_2D
				 0,                            //0 for now
				 GL_RGB,                       //Format OpenGL uses for image
				 Image->width, Image->height,  //Width and height
				 0,                            //The border of the image
				 GL_RGB, //GL_RGB, because pixels are stored in RGB format
				 GL_UNSIGNED_BYTE, //GL_UNSIGNED_BYTE, because pixels are stored
				                   //as unsigned numbers
				 Image->pixels);               //The actual pixel data
	//Set Texture Settings
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
    //GL_NEAREST = pixligt, GL_LINEAR = suddigt

	return textureId; //Returns the id of the texture
}

//NETWORK DATA TRANSLATION
bool kaufen::interpret_data(float* data_array,SOCKET SocSender)
{
    int packet_type=(int)data_array[1];

    //cout<<"interp TYPE: "<<packet_type<<endl;

    switch (packet_type)
    {
        case 1001://ping and prices (for clients)
        {
            //cout<<"Interpret: 1001\n";
            //cout<<"price: "<<data_array[3]<<endl;
            //ping test
            float server_ping_time=data_array[2];
            float ping_reply[]={0,1006,server_ping_time};
            ping_reply[0]=sizeof(ping_reply);
            Sleep(10);
            m_NetCom.send_data(ping_reply);//send back ping confirmation to server

            //store prices
            float prices[_numof_mercs];
            for(int i=0;i<_numof_mercs;i++)
            {
                prices[i]=data_array[i+3];
            }
            m_Panel.set_prices(prices,server_ping_time);

            //recalc value sum for all players
            for(int j=0;j<(int)m_Players.size();j++)
            {
                float value_sum=m_Players[j].money;
                for(int i=0;i<_numof_mercs;i++)
                {
                    value_sum+=m_Players[j].owned_units[i] * m_Panel.get_price(i);
                }
                m_Players[j].value_sum=value_sum;
            }

            //update highscore
            for(int i=0;i<(int)m_Players.size();i++)
            {
                m_Panel.update_player_highscore( m_Players[i].name, m_Players[i].money, m_Players[i].value_sum );
            }

            //update your inventory wallet
            //find your id
            for(int i=0;i<(int)m_Players.size();i++)
            {
                if( (int)m_Players[i].soc_id==m_my_soc_id )
                {
                    m_Panel.update_inventory_wallet( m_Players[i].money, m_Players[i].value_sum );
                    break;
                }
            }

        }break;

        case 1002://server lost X
        {

        }break;

        case 1003://new player X
        {

        }break;

        case 1004://player disconnected (for clients)
        {
            cout<<"Removing lost player\n";
            int player_soc_id=(int)data_array[2];
            remove_player( player_soc_id );

            if(m_conf_mode)
            {
                //send confirmation
                float rep_data[]={0,1104,data_array[3]};
                rep_data[0]=sizeof(rep_data);
                Sleep(10);
                m_NetCom.send_data(rep_data);
            }

        }break;

        case 1104://player disconnected confirmation (for server)
        {
            cout<<"Received package confirmation for lost player\n";
            //remove from package array
            for(int i=0;i<(int)m_vPackages.size();i++)
            {
                if( m_vPackages[i].id==(int)data_array[2] && m_vPackages[i].socReceiver==SocSender )//find correct ID value and sender
                {
                    m_Log.log("RemoType:",(int)m_vPackages[i].pData[1],"ID:",(int)data_array[2]);
                    delete[] m_vPackages[i].pData; //delete data
                    m_vPackages.erase( m_vPackages.begin()+i ); //remove from array
                    cout<<"Removed from package list\n";
                    return true;
                }
            }
            cout<<"ERROR could not find that package, ID: "<<(int)data_array[2]<<endl;

        }break;

        case 1005://text message X
        {

        }break;

        case 1006://ping response from client (for server)
        {
            float time=(float)clock()/CLOCKS_PER_SEC;
            //find player socket
            for(int i=0;i<(int)m_Players.size();i++)
            {
                if(m_Players[i].soc_id==SocSender)
                {
                    m_Players[i].last_ping=time-data_array[2];
                    //cout<<m_Players[i].name<<" ping: "<<m_Players[i].last_ping<<endl;
                    m_Players[i].replied_sec_ago=0;
                    return true;
                }
            }
        }break;

        case 1007://sale request (for server)
        {
            cout<<"Client requests sale, ID: "<<(int)data_array[5]<<endl;
            if( (int)data_array[5]==0 && m_conf_mode )//bad ID
            {
                cout<<"ERROR Bad package ID, will be ignored\n";
                //tell client to clear all unsent packages
                float rep_data[]={0,1021};
                rep_data[0]=sizeof(rep_data);
                Sleep(10);
                m_NetCom.send_data(rep_data,SocSender);
                //clear servers packet list
                m_vPackages.clear();
                m_Log.log("XXX-CLEAR-XXX");
                return false;
            }
            if(m_conf_mode)
            {
                //test if already processed
                for(int i=0;i<(int)m_vPackages_confirmed.size();i++)
                {
                    if( m_vPackages_confirmed[i].socReceiver==SocSender && m_vPackages_confirmed[i].id==data_array[5] )
                    {
                        //already processed, ignore testing, just reply
                        //send package confirmation
                        float rep_data[]={0,1107,data_array[5]};
                        rep_data[0]=sizeof(rep_data);
                        Sleep(10);
                        m_NetCom.send_data(rep_data,SocSender);
                        return false;
                    }
                }
            }

            if ( server_test_sale_and_send( SocSender, (int)data_array[2], (int)data_array[3] ) ) //socket,type,change,wanted price (not used)
            {
                cout<<"Requested Sale OK\n";
            }
            else cout<<"Requested Sale NOT possible\n";

            if(m_conf_mode)
            {
                //add to processed packages list
                st_package_confirmed temp_pack;
                temp_pack.id=data_array[5];
                temp_pack.socReceiver=SocSender;
                temp_pack.time_received=m_time_this_cycle;
                m_vPackages_confirmed.push_back(temp_pack);
                //send package confirmation
                float rep_data[]={0,1107,data_array[5]};
                rep_data[0]=sizeof(rep_data);
                Sleep(10);
                m_NetCom.send_data(rep_data,SocSender);
            }

        }break;

        case 1107://sale request confirmation (for client)
        {
            cout<<"Received package confirmation about sale requests\n";
            //remove from package array
            for(int i=0;i<(int)m_vPackages.size();i++)
            {
                if( m_vPackages[i].id==(int)data_array[2] )//find correct ID value, sender not imortant for client
                {
                    m_Log.log("RemoType:",(int)m_vPackages[i].pData[1],"ID:",(int)data_array[2]);
                    delete[] m_vPackages[i].pData; //delete data
                    m_vPackages.erase( m_vPackages.begin()+i ); //remove from array
                    cout<<"Removed from package list, ID: "<<(int)data_array[2]<<endl;
                    return true;
                }
            }
            cout<<"ERROR could not find that package, ID: "<<(int)data_array[2]<<endl;

        }break;

        case 1008://sale complete (for client)
        {
            cout<<"New sale confirmed, ID: "<<(int)data_array[6]<<endl;
            if( (int)data_array[6]==0 && m_conf_mode )//bad ID
            {
                cout<<"ERROR Bad package ID, will be ignored\n";
                //tell client to clear all unsent packages
                float rep_data[]={0,1021};
                rep_data[0]=sizeof(rep_data);
                Sleep(10);
                m_NetCom.send_data(rep_data,SocSender);
                //clear servers packet list
                m_vPackages.clear();
                m_Log.log("XXX-CLEAR-XXX");
                return false;
            }
            if(m_conf_mode)
            {
                //test if already processed
                for(int i=0;i<(int)m_vPackages_confirmed.size();i++)
                {
                    if( m_vPackages_confirmed[i].socReceiver==SocSender && m_vPackages_confirmed[i].id==data_array[6] )
                    {
                        //already processed, ignore testing, just reply
                        cout<<"Package already processed\n";
                        //send package confirmation
                        cout<<"Sending package confirmation reply, ID: "<<(int)data_array[6]<<endl;
                        float rep_data[]={0,1108,data_array[6]};
                        rep_data[0]=sizeof(rep_data);
                        Sleep(10);
                        m_NetCom.send_data(rep_data);
                        return false;
                    }
                }
            }

            sale_update( (int)data_array[2], (int)data_array[3], (int)data_array[4] ,(float)data_array[5] );

            if(m_conf_mode)
            {
                //add to processed packages list
                st_package_confirmed temp_pack;
                temp_pack.id=data_array[6];
                temp_pack.socReceiver=SocSender;
                temp_pack.time_received=m_time_this_cycle;
                m_vPackages_confirmed.push_back(temp_pack);
                //send package confirmation
                cout<<"Sending package confirmation reply, ID: "<<(int)data_array[6]<<endl;
                float rep_data[]={0,1108,data_array[6]};
                rep_data[0]=sizeof(rep_data);
                Sleep(10);
                m_NetCom.send_data(rep_data);
            }

        }break;

        case 1108://sale complete confirmation (for server)
        {
            cout<<"Received package confirmation about sale confirmed\n";
            //remove from package array
            for(int i=0;i<(int)m_vPackages.size();i++)
            {
                if( m_vPackages[i].id==(int)data_array[2] && m_vPackages[i].socReceiver==SocSender )//find correct ID value and sender
                {
                    m_Log.log("RemoType:",(int)m_vPackages[i].pData[1],"ID:",(int)data_array[2]);
                    delete[] m_vPackages[i].pData; //delete data
                    m_vPackages.erase( m_vPackages.begin()+i ); //remove from array
                    cout<<"Removed from package list, ID: "<<(int)data_array[2]<<endl;
                    return true;
                }
            }
            cout<<"ERROR could not find that package, ID: "<<(int)data_array[2]<<endl;
            cout<<"XXXXXXXXXXXXXXXXXXXXXXXXXXX\n";
            cout<<"size: "<<data_array[0]<<endl;
            cout<<"type: "<<data_array[1]<<endl;
            cout<<"XXXXXXXXXXXXXXXXXXXXXXXXXXX\n";
            cout<<"all other unconfirmed packages:\n";
            for (int i=0;i<(int)m_vPackages.size();i++)
            {
                cout<<i<<", ID: "<<m_vPackages[i].id<<", socReciver: "<<m_vPackages[i].socReceiver<<", type: "<<m_vPackages[i].pData[1]<<endl;
            }
            cout<<"XXXXXXXXXXXXXXXXXXXXXXXXXXX\n";
        }break;

        case 1009://reply from server with client's soc_id (for client)
        {
            cout<<"Received socket ID\n";
            m_my_soc_id=data_array[2];

            for(int i=0;i<(int)m_Players.size();i++)
            {//update players vector
                if( (int)m_Players[i].soc_id==m_my_soc_id )
                {
                    m_client_flag_soc_reply=true;

                    m_Players[i].name=m_my_name;
                    break;
                }
            }

            start_joined_game();//client is now ready to start
        }break;

        case 1010://start package (server inventory) (for client)
        {
            cout<<"Received server inventory\n";
            m_client_flag_server_inventory=true;
            for(int i=0;i<_numof_mercs;i++)//update merc's unit counters
            {
                m_Panel.set_unit_counter_init( i, data_array[i+8] );
                m_Panel.set_merc_price_init( i, data_array[i+_numof_mercs+8] );
            }
            m_server_state=data_array[2];//3 = lobby, 4 = in game
            m_game_mode=data_array[3];
            m_game_goal=data_array[4];
            m_server_soc_id=SocSender; //data_array[5] is only the soc_id, and the socket for the server viewed as the server;
            m_tick_time_at_game_start=data_array[6]; //remember time if joining started game
            m_server_version=data_array[7];
            m_Panel.set_start_time_sec(m_tick_time_at_game_start);
            //cout<<"TIME at start: "<<m_tick_time_at_game_start<<endl;
            //mark buttons if in lobby
            if(m_server_state==st_lobby)
            {
                m_Lobby.set_game_mode(m_game_mode);
                m_Lobby.set_game_goal(m_game_mode,m_game_goal);

            }

            //reply to server for name/socket confirmation
            string sReply("clr ");
            sReply.append(m_my_name);
            Sleep(10);
            m_NetCom.send_data(sReply);

        }break;

        case 1011://lobby ping (for clients)
        {
            float server_ping_time=data_array[2];
            float ping_reply[]={0,1012,server_ping_time};
            ping_reply[0]=sizeof(ping_reply);
            Sleep(10);
            m_NetCom.send_data(ping_reply);//send back ping confirmation to server
        }break;

        case 1012://lobby ping reply (for servers)
        {
            float time=(float)clock()/CLOCKS_PER_SEC;
            //find player socket
            for(int i=0;i<(int)m_Players.size();i++)
            {
                if(m_Players[i].soc_id==SocSender)
                {
                    m_Players[i].last_ping=time-data_array[2];
                    //cout<<m_Players[i].name<<" ping: "<<m_Players[i].last_ping<<endl;
                    m_Players[i].replied_sec_ago=0;
                    return true;
                }
            }
        }break;

        case 1013://game is starting (for clients)
        {
            cout<<"Game is starting\n";
            m_tick_time_at_game_start=(float)clock()/CLOCKS_PER_SEC; //remember time, not exact but only server will test for game over
            m_Panel.set_start_time_sec(m_tick_time_at_game_start);
            //cout<<"TIME at start: "<<m_tick_time_at_game_start<<endl;
            //store gamemode type and goal
            m_game_mode=(int)data_array[2];
            m_game_goal=(int)data_array[3];
            //tell Panel about goal
            m_Panel.set_win_conditions(m_game_mode,m_game_goal);
            //start the game
            m_server_state=st_running;
            m_state=st_running;
        }break;

        case 1014://game mode update in lobby (for clients)
        {
            cout<<"Game settings updated\n";
            m_game_mode=(int)data_array[2];
            m_game_goal=(int)data_array[3];
            m_Lobby.set_game_mode(m_game_mode);
            m_Lobby.set_game_goal(m_game_mode,m_game_goal);
            cout<<"Set mode: "<<m_game_mode<<" , Set goal: "<<m_game_goal<<endl;

            //play sound
            m_pSoundEngine->playSimpleSound(wav_button_press,1.0f);

        }break;

        case 1015://server goes down (for clients)
        {
            cout<<"Server is shutting down\n";
            clean_up();
            m_state=st_menu;
            m_server_state=0;

            //play sound
            m_pSoundEngine->playSimpleSound(wav_disconnected,1.0f);
        }break;

        case 1016://client is ready in lobby (for server)
        {
            cout<<"Client is ready\n";
            for(int i=0;i<(int)m_Players.size();i++)
            {
                if(m_Players[i].soc_id==SocSender)
                {
                    m_Players[i].ready=true;
                    //send to console
                    string console_message=m_Players[i].name;
                    console_message.append(" is Ready");
                    m_Lobby.text_to_console("SYSTEM",console_message);
                    //tell the clients
                    string send_message("tfs SYSTEM - ");
                    send_message.append(m_Players[i].name);
                    send_message.append(" is Ready");
                    Sleep(10);
                    m_NetCom.send_data(send_message);//to clients
                    break;
                }
            }
        }break;

        case 1017://you are kicked due to non-unique name (for clients)
        {
            cout<<"Kicked to non-unique name\n";
            m_state=st_menu;
            m_server_state=0;
            clean_up();
            m_Menu.set_player_name("That name was taken");

            //play sound
            m_pSoundEngine->playSimpleSound(wav_disconnected,1.0f);

            Sleep(100);//avoid clicking join
        }break;

        case 1018://buyout request (for server)
        {
            cout<<"Buyout request received\n";
            if( (int)data_array[5]==0 && m_conf_mode )//bad ID
            {
                cout<<"ERROR Bad package ID, will be ignored\n";
                //tell client to clear all unsent packages
                float rep_data[]={0,1021};
                rep_data[0]=sizeof(rep_data);
                Sleep(10);
                m_NetCom.send_data(rep_data,SocSender);
                //clear servers packet list
                m_vPackages.clear();
                m_Log.log("XXX-CLEAR-XXX");
                return false;
            }
            if(m_conf_mode)
            {
                //test if already processed
                for(int i=0;i<(int)m_vPackages_confirmed.size();i++)
                {
                    if( m_vPackages_confirmed[i].socReceiver==SocSender && m_vPackages_confirmed[i].id==data_array[5] )
                    {
                        //already processed, ignore testing, just reply
                        //send package confirmation
                        float rep_data[]={0,1118,data_array[4]};
                        rep_data[0]=sizeof(rep_data);
                        Sleep(10);
                        m_NetCom.send_data(rep_data,SocSender);
                        return false;
                    }
                }
            }
            int buyer_soc_id=(int)data_array[2];
            int victim_soc_id=(int)data_array[3];
            float victim_value_x2;//not used atm
            buyout_test(buyer_soc_id,victim_soc_id,victim_value_x2);//buyout OK, will send confirmation to clients, and values will be updated locally

            if(m_conf_mode)
            {
                //add to processed packages list
                st_package_confirmed temp_pack;
                temp_pack.id=data_array[4];
                temp_pack.socReceiver=SocSender;
                temp_pack.time_received=m_time_this_cycle;
                m_vPackages_confirmed.push_back(temp_pack);
                //send package confirmation
                float rep_data[]={0,1118,data_array[4]};
                rep_data[0]=sizeof(rep_data);
                Sleep(10);
                m_NetCom.send_data(rep_data,SocSender);
            }

        }break;

        case 1118://buyout request confirmation (for client)
        {
            cout<<"Received package confirmation about Buyout request\n";
            //remove from package array
            for(int i=0;i<(int)m_vPackages.size();i++)
            {
                if( m_vPackages[i].id==(int)data_array[2] )//find correct ID value
                {
                    m_Log.log("RemoType:",(int)m_vPackages[i].pData[1],"ID:",(int)data_array[2]);
                    delete[] m_vPackages[i].pData; //delete data
                    m_vPackages.erase( m_vPackages.begin()+i ); //remove from array
                    cout<<"Removed from package list, ID: "<<(int)data_array[2]<<endl;
                    return true;
                }
            }
            cout<<"ERROR could not find that package, ID: "<<(int)data_array[2]<<endl;

        }break;

        case 1019://buyout confirmation (for clients)
        {
            cout<<"Buyout confirmed\n";
            if( (int)data_array[5]==0 && m_conf_mode )//bad ID
            {
                cout<<"ERROR Bad package ID, will be ignored\n";
                //tell client to clear all unsent packages
                float rep_data[]={0,1021};
                rep_data[0]=sizeof(rep_data);
                Sleep(10);
                m_NetCom.send_data(rep_data,SocSender);
                //clear servers packet list
                m_vPackages.clear();
                m_Log.log("XXX-CLEAR-XXX");
                return false;
            }
            if(m_conf_mode)
            {
                //test if already processed
                for(int i=0;i<(int)m_vPackages_confirmed.size();i++)
                {
                    if( m_vPackages_confirmed[i].socReceiver==SocSender && m_vPackages_confirmed[i].id==data_array[5] )
                    {
                        //already processed, ignore testing, just reply
                        //send package confirmation
                        float rep_data[]={0,1119,data_array[5]};
                        rep_data[0]=sizeof(rep_data);
                        Sleep(10);
                        m_NetCom.send_data(rep_data);
                        return false;
                    }
                }
            }
            int buyer_soc_id=(int)data_array[2];
            int victim_soc_id=(int)data_array[3];
            float victim_value_x2=data_array[4];
            //update values
            if( !buyout_update(buyer_soc_id,victim_soc_id,victim_value_x2) )//you are the victim client
            {
                //send package confirmation
                float rep_data[]={0,1119,data_array[5]};
                rep_data[0]=sizeof(rep_data);
                Sleep(10);
                m_NetCom.send_data(rep_data);

                m_state=st_menu;
                m_server_state=0;
                clean_up();
                m_Menu.set_player_name("You were bought");
                //play sound
                m_pSoundEngine->playSimpleSound(wav_disconnected,1.0f);
                return false;
            }
            //ELSE you are not the victim, just reply
            if(m_conf_mode)
            {
                //add to processed packages list
                st_package_confirmed temp_pack;
                temp_pack.id=data_array[5];
                temp_pack.socReceiver=SocSender;
                temp_pack.time_received=m_time_this_cycle;
                m_vPackages_confirmed.push_back(temp_pack);
                //send package confirmation
                float rep_data[]={0,1119,data_array[5]};
                rep_data[0]=sizeof(rep_data);
                Sleep(10);
                m_NetCom.send_data(rep_data);
            }

        }break;

        case 1119://buyout confirmation confirmation (for server)
        {
            cout<<"Received package confirmation about Buyout confirmation\n";
            //remove from package array
            for(int i=0;i<(int)m_vPackages.size();i++)
            {
                if( m_vPackages[i].id==(int)data_array[2] && m_vPackages[i].socReceiver==SocSender )//find correct ID value and sender
                {
                    m_Log.log("RemoType:",(int)m_vPackages[i].pData[1],"ID:",(int)data_array[2]);
                    delete[] m_vPackages[i].pData; //delete data
                    m_vPackages.erase( m_vPackages.begin()+i ); //remove from array
                    cout<<"Removed from package list, ID: "<<(int)data_array[2]<<endl;
                    return true;
                }
            }
            cout<<"ERROR could not find that package, ID: "<<(int)data_array[2]<<endl;

        }break;

        case 1020://update server and your inventory (for client)
        {
            //cout<<"Received package with server and my inventory\n";
            int my_id=-1;
            for(int i=0;i<(int)m_Players.size();i++)
            {
                if( (int)m_Players[i].soc_id==m_my_soc_id)
                {
                    my_id=i;
                    break;
                }
            }
            if(my_id==-1)
            {
                cout<<"ERROR that player does not exist, for inventory update\n";
                return false;
            }

            //set server inventory
            //update money,valuesum
            m_Panel.update_inventory_wallet(data_array[2],data_array[3]);
            m_my_money_count=data_array[2];
            m_my_value_sum=data_array[3];
            m_Players[my_id].money=m_my_money_count;
            m_Players[my_id].value_sum=m_my_value_sum;
            for(int i=0;i<_numof_mercs;i++)
            {
                //update server inventory
                m_Panel.refresh_unit_counter( i,(int)data_array[i+4] );
                //update your inventory
                m_Panel.refresh_inventory( i, (int)data_array[i+_numof_mercs+4] );

                m_Players[my_id].owned_units[i]=(int)data_array[i+_numof_mercs+4];
            }

        }break;

        case 1021://server or client response of bad package ID, you will clear unsent packages (for client and server)
        {
            cout<<"Bad package was sent, unsent packages will be deleted\n";
            m_Log.log("XXX-CLEANUP-XXX");
            m_vPackages.clear();

        }break;

        case 1022://inventory update (for clients)
        {
            //cout<<"Received package with a client's inventory\n";
            //find client
            string name;
            int player_id=-1;
            for(int i=0;i<(int)m_Players.size();i++)
            {
                if( (int)m_Players[i].soc_id==(int)data_array[2] )
                {
                    name=m_Players[i].name;
                    player_id=i;
                    break;
                }
            }
            if(player_id==-1)
            {
                cout<<"ERROR could not find that player\n";
                return false;
            }

            //copy data
            float units_counter[_numof_mercs];
            for(int i=0;i<_numof_mercs;i++)
            {
                int counter=(int)data_array[i+3];
                units_counter[i]=counter;
                m_Players[player_id].owned_units[i]=counter;
            }
            //set unit counter
            m_Panel.set_owned_unit_counter_for_player(name,units_counter);

        }break;


        default://text info
        {
            //translate float to char
            char c_array[1024];
            memcpy(c_array,data_array,1024);
            //cout<<"arrayen: ";
            //cout<<c_array[0]<<c_array[1]<<c_array[2]<<endl;

            //player list (for client)
            if(c_array[0]=='p' && c_array[1]=='l' && c_array[2]=='d')//player data
            {
                cout<<"Received player list\n";
                m_client_flag_playerlist=true;
                string player_list(c_array);
                stringstream ss0(player_list);
                string line;//one line per player
                getline(ss0,line);//skip first line
                while ( getline(ss0,line) )//name,soc_id,money,owned_units[16]
                {
                    st_player player_temp;
                    string word;
                    stringstream ss(line);
                    ss>>word;
                    //cout<<word<<' ';
                    player_temp.name=word;

                    ss>>word;
                    //cout<<word<<' ';
                    player_temp.soc_id=atoi( word.c_str() );

                    ss>>word;
                    //cout<<word<<' ';
                    player_temp.money=atoi( word.c_str() );

                    player_temp.value_sum=player_temp.money;

                    for(int i=0;i<_numof_mercs;i++)
                    {
                        ss>>word;
                        //cout<<word<<' ';
                        player_temp.owned_units[i]=atoi( word.c_str() );

                        player_temp.value_sum+=player_temp.owned_units[i] * m_Panel.get_price(i);
                    }
                    //cout<<endl;
                    m_Players.push_back(player_temp);

                    //update highscore
                    if( player_temp.name=="unknown" ) m_Panel.add_new_player(m_my_name,m_money_start,player_temp.value_sum); //yourself (server dont know your name jet)
                    else m_Panel.add_new_player(player_temp.name,m_money_start,player_temp.value_sum);

                    if(m_server_state==st_lobby)//update player list
                    {
                        if( player_temp.name=="unknown" ) m_Lobby.add_player(m_my_name); //yourself (server dont know your name jet)
                        else m_Lobby.add_player(player_temp.name);
                    }
                }


                return true;
            }

            //reply from joining client (for server)
            if(c_array[0]=='c' && c_array[1]=='l' && c_array[2]=='r')//client name reply
            {
                string sPlayer_name(c_array,4,20);//cut header
                cout<<"Player joining: "<<sPlayer_name<<endl;
                //find player socket
                //cout<<"Orginal socid: "<<SocSender<<endl; //temp
                for(int i=0;i<(int)m_Players.size();i++)
                {
                    //cout<<m_Players[i].soc_id<<endl;//temp
                    if(m_Players[i].soc_id==SocSender)
                    {
                        //test if name is unique
                        for(int j=0;j<(int)m_Players.size();j++)
                        {
                            if( m_Players[j].name==sPlayer_name )
                            {
                                cout<<"Player with non-unique name tried to join\n";
                                //tell client to change name
                                float name_change_data[]={0,1017};
                                name_change_data[0]=sizeof(name_change_data);
                                Sleep(10);
                                m_NetCom.send_data(name_change_data,SocSender);
                                //erase player (other clients dont know the new player yet anyway)
                                remove_player(SocSender);
                                return false;
                            }
                        }

                        //play sound
                        m_pSoundEngine->playSimpleSound(wav_joined,1.0f);

                        m_Players[i].name=sPlayer_name;
                        cout<<"Player name bound to Socket ID\n";

                        //reply with client's socket ID
                        float sock_id_reply[]={0,1009,SocSender};
                        sock_id_reply[0]=sizeof(sock_id_reply);
                        Sleep(10);
                        m_NetCom.send_data(sock_id_reply,SocSender);
                        //send server name to client
                        string welcome_message("tfs ");
                        welcome_message.append("SYSTEM - Welcome to the ");
                        welcome_message.append(m_server_name);
                        welcome_message.append(" Server");
                        Sleep(10);
                        m_NetCom.send_data(welcome_message,SocSender);

                        //broadcast other clients about new player (name and sock_id)
                        string new_client_data("ncd ");
                        new_client_data.append(sPlayer_name);
                        new_client_data.append(1,' ');
                        stringstream ss;
                        ss<<SocSender;
                        new_client_data.append( ss.str() );
                        Sleep(10);
                        m_NetCom.send_data(new_client_data);

                        //add player to highscore
                        m_Panel.add_new_player(sPlayer_name,m_money_start,m_money_start);

                        if(m_state==st_lobby)//add to player list in lobby view
                        {
                            m_Lobby.add_player(sPlayer_name);
                        }

                        return true;
                    }
                }
                return false;//player not found
            }

            //info about new joining player for other clients (for clients)
            if(c_array[0]=='n' && c_array[1]=='c' && c_array[2]=='d')//new client data
            {
                string ncd_string(c_array);
                //cout<<ncd_string<<endl;//temp
                string word;
                stringstream ss(ncd_string);
                ss>>word;//skip header
                ss>>word;//get name
                string client_name=word;
                cout<<"Player joined: "<<client_name<<endl;
                ss>>word;//get soc_id
                int soc_id=atof( word.c_str() );

                if(soc_id!=m_my_soc_id)
                {
                    client_add_new_client( client_name, soc_id );
                }

                //play sound
                m_pSoundEngine->playSimpleSound(wav_joined,1.0f);

                return true;
            }

            //text message from client to server for broadcasting (for server)
            if(c_array[0]=='t' && c_array[1]=='t' && c_array[2]=='s') //text to server
            {
                cout<<"Received text message\n";
                //swap header and send to all clients
                c_array[1]='f';
                string message_to_send(c_array);
                Sleep(10);
                m_NetCom.send_data(message_to_send);
                //(message have already been shown on server's console)

                //play sound
                m_pSoundEngine->playSimpleSound(wav_message,1.0f);
            }

            //text message from server to client (for client)
            if(c_array[0]=='t' && c_array[1]=='f' && c_array[2]=='s') //text from server
            {
                cout<<"Received text message\n";
                //send to console
                string message_to_console(c_array,4,40);//cut header
                switch(m_state)
                {
                    case st_lobby: m_Lobby.text_to_console("",message_to_console); break;
                    case st_running: m_Panel.text_to_console("",message_to_console); break;
                }
                //play sound
                m_pSoundEngine->playSimpleSound(wav_message,1.0f);
            }

            //game over, winner sent (for clients)
            if(c_array[0]=='w' && c_array[1]=='i' && c_array[2]=='n') //winner name
            {
                if(m_state==st_gameover) return false;//ignore message

                string winner_name(c_array,4,40);//cut header
                cout<<"Game over, winner: "<<winner_name<<endl;

                //adjust name length
                int numof_spaces=( 10-(int)winner_name.length() )/2;
                if( numof_spaces>0 ) winner_name.insert( winner_name.begin(),' ',numof_spaces );

                m_GameOver.set_winner_name(winner_name);
                m_state=st_gameover;
                //play sound
                if(winner_name==m_my_name)  m_pSoundEngine->playSimpleSound(wav_win,1.0f);
                else                        m_pSoundEngine->playSimpleSound(wav_lost,1.0f);
            }

            return false;
        }
    }

    return true;
}

bool kaufen::client_add_new_client(string name,int soc_id)//for clients
{
    cout<<"adding new player: "<<name<<", ID: "<<soc_id<<endl;
    st_player player_temp;
    player_temp.soc_id = soc_id;
    player_temp.name = name;
    player_temp.money= m_money_start;
    player_temp.value_sum= m_money_start;
    for(int i=0;i<_numof_mercs;i++) player_temp.owned_units[i]=0;

    m_Players.push_back(player_temp);

    m_Panel.add_new_player(name,m_money_start,m_money_start); //update highscore

    if(m_server_state==st_lobby)//update lobby player list
    {
        m_Lobby.add_player(name);
    }

    return true;
}

bool kaufen::ip_is_valid(char *str)
{
    int segs = 0;   //Segment count
    int chcnt = 0;  // Character count within segment
    int accum = 0;  // Accumulator for segment

    //Catch NULL pointer
    if (str == NULL)
        return 0;

    //Process every character in string

    while(*str != '\0')
    {
        //Segment changeover
        if (*str == '.')
        {
            //Must have some digits in segment
            if (chcnt == 0) return false;
            //Limit number of segments
            if (++segs == 4) return false;
            //Reset segment values and restart loop
            chcnt = accum = 0;
            str++;
            continue;
        }

        //Check numeric
        if ((*str < '0') || (*str > '9')) return false;

        //Accumulate and check segment
        if ((accum = accum * 10 + *str - '0') > 255) return false;

        //Advance other segment specific stuff and continue loop
        chcnt++;
        str++;
    }

    //Check enough segments and enough characters in last segment
    if (segs != 3) return false;

    if (chcnt == 0) return false;

    //Address okay
    return true;
}

bool kaufen::send_lobby_ping(void)//for server
{
    float time=(float)clock()/CLOCKS_PER_SEC;

    //ping
    float ping_data[3];
    ping_data[0]=sizeof(ping_data);
    ping_data[1]=1011;
    ping_data[2]=time;

    Sleep(10);
    m_NetCom.send_data(ping_data); //send to all

    return true;
}

bool kaufen::ping_clients(void)//for server
{
    float time=(float)clock()/CLOCKS_PER_SEC;

    //update merc prices
    m_Panel.update_prices(time);

    //ping
    float ping_price_data[_numof_mercs+3];
    m_Panel.get_prices(ping_price_data);
    ping_price_data[0]=sizeof(ping_price_data);
    ping_price_data[1]=1001;
    ping_price_data[2]=time;

    Sleep(10);
    m_NetCom.send_data(ping_price_data); //send to all

    //recalc value sum for all players
    for(int j=0;j<(int)m_Players.size();j++)
    {
        float value_sum=m_Players[j].money;
        for(int i=0;i<_numof_mercs;i++)
        {
            value_sum+=m_Players[j].owned_units[i] * m_Panel.get_price(i);
        }
        m_Players[j].value_sum = value_sum;
    }

    //update highscore
    for(int i=0;i<(int)m_Players.size();i++)
    {
        m_Panel.update_player_highscore( m_Players[i].name, m_Players[i].money, m_Players[i].value_sum );
    }

    //update your inventory wallet
    //find your id
    for(int i=0;i<(int)m_Players.size();i++)
    {
        if( (int)m_Players[i].soc_id==m_my_soc_id )
        {
            m_Panel.update_inventory_wallet( m_Players[i].money, m_Players[i].value_sum );
            break;
        }
    }

    return true;
}

bool kaufen::client_test_sale_and_send(int unit_type,int unit_counter)//for client
{
    cout<<"Testing sale locally\n";

    if(unit_counter>0)//buy test (money+units left)
    {
        int units_left=m_Panel.get_unit_counter(unit_type);
        if(unit_counter>units_left) unit_counter=units_left;

        float price_total=(float)unit_counter*m_Panel.get_price(unit_type);
        //find my player to get money count
        int player_id=-1;
        for(int i=0;i<(int)m_Players.size();i++)
        {
            if( (int)m_Players[i].soc_id==m_my_soc_id)
            {
                player_id=i;
                break;
            }
        }
        if(player_id==-1) return false;

        if(m_Players[player_id].money >= price_total && unit_counter>0)
        {//buy OK
            //send request to server
            float sale_request[]={0,1007,(float)unit_type,(float)unit_counter,(float)price_total,m_resend_data_id_counter};
            m_resend_data_id_counter++;
            sale_request[0]=sizeof(sale_request);

            if(m_conf_mode)
            {
                //store package
                float* pdata = new float[6];
                pdata[0]=sizeof(pdata);
                pdata[1]=1007;
                pdata[2]=(float)unit_type;
                pdata[3]=(float)unit_counter;
                pdata[4]=(float)price_total;
                pdata[5]=sale_request[5];

                st_package temp_package;
                temp_package.id=sale_request[5];
                temp_package.socReceiver=m_server_soc_id;
                temp_package.pData=pdata;
                temp_package.time_left=2;
                m_vPackages.push_back(temp_package);
                m_Log.log("Packtype:",(int)pdata[1],"ID:",(int)temp_package.id);
            }

            Sleep(10);
            m_NetCom.send_data(sale_request);//to server
            return true;
        }
        else return false;
    }

    if(unit_counter<0)//sell test (units owned)
    {
        //find my player id
        int my_id=-1;
        for(int i=0;i<(int)m_Players.size();i++)
        {
            if( (int)m_Players[i].soc_id==m_my_soc_id )
            {
                my_id=i;
                break;
            }
        }
        if(my_id==-1) return false; //Error

        int units_left=m_Players[my_id].owned_units[unit_type];
        if(-unit_counter>units_left) unit_counter=-units_left;

        float price_total=unit_counter*m_Panel.get_price(unit_type);

        if(unit_counter<0)
        {//sell OK
            //send request to server
            float sale_request[]={0,1007,(float)unit_type,(float)unit_counter,(float)price_total,m_resend_data_id_counter};
            m_resend_data_id_counter++;
            sale_request[0]=sizeof(sale_request);

            if(m_conf_mode)
            {
                //store package
                float* pdata = new float[6];
                pdata[0]=sizeof(pdata);
                pdata[1]=1007;
                pdata[2]=(float)unit_type;
                pdata[3]=(float)unit_counter;
                pdata[4]=(float)price_total;
                pdata[5]=sale_request[5];

                st_package temp_package;
                temp_package.id=sale_request[5];
                temp_package.socReceiver=m_server_soc_id;
                temp_package.pData=pdata;
                temp_package.time_left=2;
                m_vPackages.push_back(temp_package);
                m_Log.log("Packtype:",(int)pdata[1],"ID:",(int)temp_package.id);
            }

            Sleep(10);
            m_NetCom.send_data(sale_request);//to server
            return true;
        }
        else return false;
    }

    return false;
}

bool kaufen::server_test_sale_and_send(SOCKET soc_player,int unit_type,int unit_counter)//for server
{
    for(int i=0;i<(int)m_Players.size();i++)
    {
        if(m_Players[i].soc_id==soc_player)
        {
            if(unit_counter>0)//buy test (money+units left)
            {
                int units_left=m_Panel.get_unit_counter(unit_type);
                if(unit_counter>units_left) unit_counter=units_left;

                float price_total=(float)unit_counter*m_Panel.get_price(unit_type);
                if(m_Players[i].money>=price_total && unit_counter>0)
                {//buy OK
                    //change val
                    sale_update(soc_player,unit_type,unit_counter,price_total);
                    //send
                    float sale_complete[]={0,1008,(float)soc_player,(float)unit_type,(float)unit_counter,(float)price_total,(float)m_resend_data_id_counter};
                    m_resend_data_id_counter++;
                    sale_complete[0]=sizeof(sale_complete);

                    if(m_conf_mode)
                    {
                        //store for all players
                        for(int j=0;j<(int)m_Players.size();j++)
                        {
                            if( (int)m_Players[j].soc_id==m_my_soc_id) continue;//dont send to yourself

                            float* pdata = new float[7];
                            pdata[0]=sizeof(pdata);
                            pdata[1]=1008;
                            pdata[2]=(float)soc_player;
                            pdata[3]=(float)unit_type;
                            pdata[4]=(float)unit_counter;
                            pdata[5]=(float)price_total;
                            pdata[6]=sale_complete[6];

                            st_package temp_package;
                            temp_package.id=sale_complete[6];
                            temp_package.socReceiver=m_Players[j].soc_id;
                            temp_package.pData=pdata;
                            temp_package.time_left=2;

                            m_vPackages.push_back(temp_package);
                            m_Log.log("Packtype:",(int)pdata[1],"ID:",(int)temp_package.id);
                        }
                    }

                    Sleep(10);
                    m_NetCom.send_data(sale_complete);//to all clients
                    return true;
                }
                else return false;
            }

            if(unit_counter<0)//sell test (units owned)
            {
                int units_left=m_Players[i].owned_units[unit_type];
                if(-unit_counter>units_left) unit_counter=-units_left;

                float price_total=unit_counter*m_Panel.get_price(unit_type);

                if(unit_counter<0)
                {//sell OK
                    //change val
                    sale_update(soc_player,unit_type,unit_counter,price_total);
                    //send
                    float sale_complete[]={0,1008,(float)soc_player,(float)unit_type,(float)unit_counter,(float)price_total,(float)m_resend_data_id_counter};
                    m_resend_data_id_counter++;
                    sale_complete[0]=sizeof(sale_complete);

                    if(m_conf_mode)
                    {
                        //store for all players
                        for(int j=0;j<(int)m_Players.size();j++)
                        {
                            if( (int)m_Players[j].soc_id==m_my_soc_id) continue;//dont send to yourself

                            float* pdata = new float[7];
                            pdata[0]=sizeof(pdata);
                            pdata[1]=1008;
                            pdata[2]=(float)soc_player;
                            pdata[3]=(float)unit_type;
                            pdata[4]=(float)unit_counter;
                            pdata[5]=(float)price_total;
                            pdata[6]=sale_complete[6];

                            st_package temp_package;
                            temp_package.id=sale_complete[6];
                            temp_package.socReceiver=m_Players[j].soc_id;
                            temp_package.pData=pdata;
                            temp_package.time_left=2;

                            m_vPackages.push_back(temp_package);
                            m_Log.log("Packtype:",(int)pdata[1],"ID:",(int)temp_package.id);
                        }
                    }

                    Sleep(10);
                    m_NetCom.send_data(sale_complete);//to all clients
                    return true;
                }
                else return false;
            }
        }
    }

    return false;
}

bool kaufen::sale_update(SOCKET soc_player,int unit_type,int unit_counter_change,float price_total)//for server and clients
{//update unit count and player money

    cout<<"Updating unit counters\n";

    //update players vector
    int player_id=-1;
    for(int i=0;i<(int)m_Players.size();i++)
    {//find player id
        if( m_Players[i].soc_id==soc_player )
        {
            player_id=i;
            m_Players[i].owned_units[unit_type]+=unit_counter_change; //unit update
            m_Players[i].money-=price_total; //money update
            m_Players[i].value_sum-=price_total; //value update

            break;
        }
    }
    if(player_id==-1)
    {
        cout<<"\n\nERROR Socket for current player not found\n\n";
        return false;
    }

    //update mercs (unit count)
    m_Panel.set_unit_counter(m_Players[player_id].name,unit_type,unit_counter_change);

    //recalc value sum for player
    float value_sum=m_Players[player_id].money;
    for(int i=0;i<_numof_mercs;i++)
    {
        value_sum+=m_Players[player_id].owned_units[i] * m_Panel.get_price(i);
    }
    m_Players[player_id].value_sum = value_sum;

    //update highscore (player value+money)
    m_Panel.update_player_highscore( m_Players[player_id].name, m_Players[player_id].money, m_Players[player_id].value_sum );

    //update inventory (if change involves you)
    if( (int)soc_player==m_my_soc_id )
    {
        m_Panel.update_inventory( unit_type, unit_counter_change );
        m_Panel.update_inventory_wallet( m_Players[player_id].money, m_Players[player_id].value_sum );
    }

    //show message in console
    string message_to_console;
    message_to_console.append(m_Players[player_id].name);
    if(unit_counter_change>0) message_to_console.append(" bought ");
    else                      message_to_console.append(" sold ");
    stringstream ss;
    ss<< int( fabs(unit_counter_change) );
    message_to_console.append( ss.str() );

    if(unit_counter_change==1 || unit_counter_change==-1)//singular
    {
        message_to_console.append(" unit of ");
    }
    else message_to_console.append(" units of ");//plural

    string unit_name=m_Panel.get_merc_name(unit_type);

    message_to_console.append( unit_name );
    m_Panel.text_to_console("SYSTEM",message_to_console);

    //play sound
    if( (int)soc_player==m_my_soc_id )  m_pSoundEngine->playSimpleSound(wav_cashregister,1.0f);
    else                                m_pSoundEngine->playSimpleSound(wav_beep1,1.0f);

    return true;
}

bool kaufen::server_starts_the_game(void)//for server
{
    if(m_hosting)//for server
    {
        cout<<"Server have started the game\n";

        m_tick_time_at_game_start=(float)clock()/CLOCKS_PER_SEC; //remember time
        m_Panel.set_start_time_sec(m_tick_time_at_game_start);
        //cout<<"TIME at start: "<<m_tick_time_at_game_start<<endl;

        m_game_mode=m_Lobby.get_game_mode();
        m_game_goal=m_Lobby.get_game_goal();
        //tell Panel about goal
        m_Panel.set_win_conditions(m_game_mode,m_game_goal);
        //send message to clients, gametype, goal
        float game_data[]={0,1013,m_game_mode,m_game_goal};
        game_data[0]=sizeof(game_data);
        Sleep(10);
        m_NetCom.send_data(game_data);

        m_state=st_running;
    }
    else
    {
        cout<<"ERROR client can not start game\n";
        return false;
    }

    return true;
}

bool kaufen::clean_up(void)
{
    //resetting
    cout<<"Resetting data\n";
    m_game_mode=m_game_goal=0;
    m_NetCom.clean_up();
    m_Players.clear();
    m_Lobby.init(m_tex_font,m_tex_buttons,m_tex_decals,m_window_width,m_window_height,m_pSoundEngine);
    m_Panel.init(m_tex_font,m_tex_buttons,m_tex_decals,m_pSoundEngine);
    m_Menu.init(m_window_width,m_window_height,m_tex_menu,m_tex_font,m_tex_help,m_pSoundEngine,(float)clock()/CLOCKS_PER_SEC,_version);

    return true;
}

bool kaufen::buyout_test(int buyer_soc_id,int victim_soc_id,float& victim_value_x2) //for server and clients
{
    //find bought player
    float victim_value=0;
    for(int i=0;i<(int)m_Players.size();i++)
    {
        if( (int)m_Players[i].soc_id==victim_soc_id)
        {
            victim_value=m_Players[i].value_sum;
            break;
        }
    }
    //get buyer money
    float buyer_money=0;
    for(int i=0;i<(int)m_Players.size();i++)
    {
        if( (int)m_Players[i].soc_id==buyer_soc_id)
        {
            buyer_money=m_Players[i].money;
            break;
        }
    }

    //test if possible, buyer have to have 2x the money as victims value sum
    if( buyer_money > victim_value*2 )
    {
        victim_value_x2=victim_value*2;
        if(m_hosting)//if server, send confirmation to all clients
        {
            //send confirmation to clients
            float buyout_data[]={0,1019,buyer_soc_id,victim_soc_id,victim_value_x2,m_resend_data_id_counter};
            m_resend_data_id_counter++;
            buyout_data[0]=sizeof(buyout_data);

            if(m_conf_mode)
            {
                //store for all players
                for(int i=0;i<(int)m_Players.size();i++)
                {
                    if( (int)m_Players[i].soc_id==m_my_soc_id) continue;//dont send to yourself

                    float* pdata = new float[6];
                    pdata[0]=sizeof(pdata);
                    pdata[1]=1019;
                    pdata[2]=buyer_soc_id;
                    pdata[3]=victim_soc_id;
                    pdata[4]=victim_value_x2;
                    pdata[5]=buyout_data[5];

                    st_package temp_package;
                    temp_package.id=buyout_data[5];
                    temp_package.socReceiver=m_Players[i].soc_id;
                    temp_package.pData=pdata;
                    temp_package.time_left=2;

                    m_vPackages.push_back(temp_package);
                    m_Log.log("Packtype:",(int)pdata[1],"ID:",(int)temp_package.id);
                }
            }

            Sleep(10);
            m_NetCom.send_data(buyout_data);
            //update values locally
            buyout_update(buyer_soc_id,victim_soc_id,victim_value_x2);
            return true;
        }
        else//client
        {
            //send request to server
            float buyout_data[]={0,1018,buyer_soc_id,victim_soc_id,m_resend_data_id_counter};
            m_resend_data_id_counter++;
            buyout_data[0]=sizeof(buyout_data);

            if(m_conf_mode)
            {
                //store package
                float* pdata = new float[5];
                pdata[0]=sizeof(pdata);
                pdata[1]=1018;
                pdata[2]=buyer_soc_id;
                pdata[3]=victim_soc_id;
                pdata[4]=buyout_data[4];

                st_package temp_package;
                temp_package.id=buyout_data[4];
                temp_package.socReceiver=m_server_soc_id;//will not be used, clients can only send to server anyway
                temp_package.pData=pdata;
                temp_package.time_left=2;
                m_vPackages.push_back(temp_package);
                m_Log.log("Packtype:",(int)pdata[1],"ID:",(int)temp_package.id);
            }

            Sleep(10);
            m_NetCom.send_data(buyout_data);
            return true;
        }
    }

    return false;
}

bool kaufen::buyout_update(int buyer_soc_id,int victim_soc_id,float victim_value_x2)//for server and clients
{
    bool i_the_server_was_bought_flag=false;
    if(victim_soc_id==m_my_soc_id)//you were bought,reset
    {
        if(m_hosting)//if kicked, game will end
        {
            i_the_server_was_bought_flag=true;
            //reset money , value_sum (and all owned units) happens later
            for(int i=0;i<(int)m_Players.size();i++)//find player
            {
                if( (int)m_Players[i].soc_id==m_my_soc_id )
                {
                    m_Players[i].value_sum=m_Players[i].money=m_money_start;

                }
            }
            //update highscore
            m_Panel.update_player_highscore( m_my_name, m_money_start, m_money_start );
            //update inventory wallet
            m_Panel.update_inventory_wallet(  m_money_start, m_money_start );

            //tell clients to add a new player with my name
            string player_data("ncd ");
            player_data.append(m_my_name);
            player_data.append(" ");
            stringstream ss;
            ss<<m_my_soc_id;
            player_data.append( ss.str() );
            Sleep(10);
            m_NetCom.send_data(player_data);
            //write message in console for server
            m_Panel.text_to_console("SYSTEM","You were bought and rejoined the game to avoid game ending");
            //play sound
            m_pSoundEngine->playSimpleSound(wav_no,1.0f);
        }
        else//kick client
        {
            return false;
        }
    }

    //give back victims units
    string victim_name;
    for(int i=0;i<(int)m_Players.size();i++)//find player
    {
        if( (int)m_Players[i].soc_id==victim_soc_id)
        {
            victim_name=m_Players[i].name;
            for(int j=0;j<_numof_mercs;j++)
            {
                if(m_Players[i].owned_units[j]>0)
                {
                    m_Panel.set_unit_counter( m_Players[i].name, j, -m_Players[i].owned_units[j] );
                }
            }
            break;
        }
    }
    if(!i_the_server_was_bought_flag) remove_player(victim_name);


    //take money from buyer
    string buyer_name;
    for(int i=0;i<(int)m_Players.size();i++)
    {
        if( (int)m_Players[i].soc_id==buyer_soc_id )
        {
            buyer_name=m_Players[i].name;
            m_Players[i].money-=victim_value_x2;
            m_Players[i].value_sum-=victim_value_x2;
            //update highscore
            m_Panel.update_player_highscore( m_Players[i].name, m_Players[i].money, m_Players[i].value_sum );
            //update inventory wallet, if buyer is you
            if(buyer_soc_id==m_my_soc_id)
            {
                m_Panel.update_inventory_wallet(  m_Players[i].money, m_Players[i].value_sum );
            }
            break;
        }
    }

    //display message in console
    string message_to_console=victim_name;
    message_to_console.append(" was bought by ");
    message_to_console.append(buyer_name);
    m_Panel.text_to_console("SYSTEM",message_to_console);

    //play sound
    m_pSoundEngine->playSimpleSound(wav_buyout,1.0f);

    return true;
}

bool kaufen::resend_packages_test(void)
{
    float time=(float)clock()/CLOCKS_PER_SEC;
    if( time > m_time_next_resend )//test if time to send
    {
        m_time_next_resend=time+_resend_delay;
        if( m_vPackages.empty() ) return false;//nothing to send

        for(int i=0;i<(int)m_vPackages.size();i++)//send all
        {
            //time out test
            m_vPackages[i].time_left-=time-m_time_last_cycle;
            if( m_vPackages[i].time_left<=0 )
            {
                cout<<"Give up resending a package\n";
                m_Log.log("GivUP",m_vPackages[i].pData[1]);
                m_vPackages.erase( m_vPackages.begin()+i );
                i--;
                continue;
            }

            cout<<"Resending unconfirmed package to Socket: "<<m_vPackages[i].socReceiver<<" , Regarding: "<<m_vPackages[i].pData[1]<<endl;
            m_Log.log("ResendType:",m_vPackages[i].pData[1],"ID:",m_vPackages[i].pData[5]);

            //TEMP
            //test if socReeceiver is among players
            bool found=false;
            for(int j=0;j<(int)m_Players.size();j++)
            {
                if( m_Players[j].soc_id==m_vPackages[i].socReceiver )
                {
                    cout<<"Receiver Socket is among players\n";
                    found=true;
                    break;
                }
            }
            if(!found)
            {
                if( (int)m_vPackages[i].socReceiver==m_server_soc_id) cout<<"Receiver Socket is among players\n";
                else
                {
                    cout<<"WARNING This packages does not match any players\n";
                }
            }
            //xxxxx

            if(m_hosting) m_NetCom.send_data( m_vPackages[i].pData, m_vPackages[i].socReceiver );
            else m_NetCom.send_data( m_vPackages[i].pData );//socket not important for client, only server

            Sleep(10);//rest
        }

        //remove old confirmed packages
        for(int i=0;i<(int)m_vPackages_confirmed.size();i++)
        {
            if( time > m_vPackages_confirmed[i].time_received + _package_timeout )//time to remove
            {
                m_vPackages_confirmed.erase( m_vPackages_confirmed.begin() + i );
                i--;
                continue;
            }
        }
    }

    return true;//something was sent
}

bool kaufen::lost_player_test(void)
{
    //test if player dont respond
    for(int i=0;i<(int)m_Players.size();i++)
    {
        if( (int)m_Players[i].soc_id==m_my_soc_id) continue;//skip testing yourself (the server)
        //test if joining player aborted
        if(m_Players[i].name=="unknown")
        {
            m_Players[i].time_out-=m_time_this_cycle-m_time_last_cycle;
            if( m_Players[i].time_out<=0 )//kick
            {
                remove_nonjoining_player( m_Players[i].soc_id );
            }
        }
        else //test if joined player stopped replying
        {
            m_Players[i].replied_sec_ago+=m_time_this_cycle-m_time_last_cycle;
            if(m_Players[i].replied_sec_ago>=15)//kick
            {
                cout<<"Removing inactive player: "<<m_Players[i].name;
                remove_player(m_Players[i].soc_id);
            }
        }
    }

    return true;
}

bool kaufen::show_data(void)//only for debugging
{

    if(m_time_last_show_data+5<m_time_this_cycle)//every 5 sec
    {
        m_time_last_show_data=m_time_this_cycle;

        /*for(int i=0;i<(int)m_Players.size();i++)
        {
            cout<<m_Players[i].name<<": "<<m_Players[i].soc_id<<endl;
        }
        cout<<m_server_soc_id;
        cout<<endl;
        cout<<"Unconfirmed packages:\n";
        for(int i=0;i<(int)m_vPackages.size();i++)
        {
            cout<<"ID: "<<m_vPackages[i].id<<" , To Soc_id: "<<m_Players[i].soc_id<<endl;
        }
        cout<<endl;*/
    }

    return true;
}

bool kaufen::send_data_refresh(void)//send client's data to all clients (for server)
{
    if(m_time_last_data_refresh_sent+3<m_time_this_cycle)//every 3 sec
    {
        //cout<<"Sending update about Client's and Server's inventory\n";
        m_time_last_data_refresh_sent=m_time_this_cycle;

        for(int i=0;i<(int)m_Players.size();i++)
        {
            if( (int)m_Players[i].soc_id==m_my_soc_id) continue;//dont send to yourself (server)

            float server_client_inventory[_numof_mercs*2+4];
            for(int j=0;j<_numof_mercs;j++)
            {
                server_client_inventory[j+4]=m_Panel.get_unit_counter(j);//units left
                server_client_inventory[j+_numof_mercs+4]=m_Players[i].owned_units[j];//client inventory
            }
            server_client_inventory[0]=sizeof(server_client_inventory);
            server_client_inventory[1]=1020;
            server_client_inventory[2]=m_Players[i].money;
            server_client_inventory[3]=m_Players[i].value_sum;
            Sleep(10);
            m_NetCom.send_data(server_client_inventory,m_Players[i].soc_id);
        }
    }

    return true;
}

bool kaufen::send_inventory_refresh(void)
{
    if(m_time_last_inv_refresh_sent+0.5<m_time_this_cycle)//every 0.5 sec
    {
        //cout<<"Sending update about Clients' inventories\n";
        m_time_last_inv_refresh_sent=m_time_this_cycle;

        float client_inventory[ _numof_mercs+3 ];
        for(int j=0;j<_numof_mercs;j++)
        {
            client_inventory[j+3]=m_Players[m_curr_player_for_inv_broadcast].owned_units[j];//client inventory
        }
        client_inventory[0]=sizeof(client_inventory);
        client_inventory[1]=1022;
        client_inventory[2]=(int)m_Players[m_curr_player_for_inv_broadcast].soc_id;

        //send to all clients
        m_NetCom.send_data(client_inventory);
        /*
        for(int i=0;i<(int)m_Players.size();i++)
        {
            if( (int)m_Players[i].soc_id==m_my_soc_id) continue;//dont send to yourself (server)

            Sleep(10);
            m_NetCom.send_data(client_inventory,m_Players[i].soc_id);
        }*/

        //go to next player
        m_curr_player_for_inv_broadcast++;
        if( m_curr_player_for_inv_broadcast>=(int)m_Players.size() ) m_curr_player_for_inv_broadcast=0;//reset counter
    }

    return true;
}
