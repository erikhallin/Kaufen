#ifndef KAUFEN_H
#define KAUFEN_H

#include "panel.h"
#include "perlinnoise.h"
#include "networkCom.h"
#include "menu.h"
#include "imageloader.h"
#include "lobby.h"
#include "win_window.h"
#include "sound.h"
#include "logfile.h"//temp
#include <sstream>
#include <time.h>
#include <winsock2.h>
//log?

#define _version 1.32
#define _numof_mercs 16
#define _start_money 100

#define _resend_delay 0.2
#define _package_timeout 10

enum states
{
    st_error=0,
    st_init,
    st_menu,
    st_lobby,
    st_running,
    st_gameover
};

struct st_player
{
    SOCKET soc_id;//the SOCKET viewed by the SERVER, for clients the server socket will not be correct (use only as ID)
    string name;
    int    owned_units[_numof_mercs];
    float  money;
    float  value_sum;
    float  last_ping;
    float  replied_sec_ago;
    float  time_out;
    bool   ready;
};

struct st_package
{
    int    id;
    float  time_left;//when 0 give up resending
    SOCKET socReceiver;
    float* pData;
};

struct st_package_confirmed
{
    int    id;
    SOCKET socReceiver;
    float  time_received;
};

class kaufen
{
    public:
        kaufen();
        ~kaufen();

        bool init(bool keys[],bool mouse_but[4],int mouse_pos[2],int window_width,int window_height);

        //Network
        bool init_network(string net_status);
        bool start_hosting(void);
        bool send_start_package(SOCKET SocReceiver);
        SOCKET get_socket(void);
        bool connect_to_server(void);
        bool connect_to_client(SOCKET new_client);
        bool test_connection(void);
        bool send_data(float* data_array);
        bool recv_data(SOCKET SocSender);
        bool broadcast_to_server(void);
        bool check_for_broadcast_reply(void);
        bool check_for_broadcast(void);
        bool remove_player(string player_name);
        bool remove_player(SOCKET SocClient);
        bool remove_nonjoining_player(SOCKET SocClient);

        bool getInput(bool keys[],bool mouse[4],int mouse_pos[2]);
        int  cycle(void);
        bool set_confirmation_mode(bool flag);

    private:
        float m_time_this_cycle,m_time_last_cycle;
        int   m_window_width,m_window_height;
        bool  m_ready,m_textures_ready,m_hosting,m_check_for_broadcast_reply;//init flag
        bool* m_pKeys;
        bool* m_pMouse_but;
        int*  m_pMouse_pos;
        int   m_state,m_server_state,m_default_port;
        int   m_game_mode,m_game_goal;
        float m_key_delay,m_ping_delay,m_ping_last_time,m_tick_time_at_game_start,m_time_next_resend,m_time_last_show_data,m_time_last_data_refresh_sent,m_time_last_inv_refresh_sent;
        int   m_resend_data_id_counter,m_curr_player_for_inv_broadcast;
        int   m_numof_players;//?
        float m_my_money_count,m_my_value_sum,m_money_start,m_server_version;
        string m_my_name,m_server_name;
        int   m_my_soc_id,m_server_soc_id;
        bool  m_client_flag_playerlist,m_client_flag_soc_reply,m_client_flag_server_inventory;

        int   m_tex_font[3];
        int   m_tex_buttons,m_tex_decals,m_tex_menu,m_tex_help,m_tex_gameover;

        lobby      m_Lobby;
        panel      m_Panel;
        menu       m_Menu;
        win_window m_GameOver;
        networkCom m_NetCom;
        sound*     m_pSoundEngine;

        bool m_conf_mode;//temp
        logfile m_Log;//temp

        vector<st_player>            m_Players;
        vector<st_package>           m_vPackages;
        vector<st_package_confirmed> m_vPackages_confirmed;

        //Functions
        bool start_joined_game(void);
        bool load_textures(void);
        bool load_sounds(void);
        bool interpret_data(float* data_array,SOCKET SocSender);
        bool client_add_new_client(string name,int soc_id);
        bool ip_is_valid(char *str);
        bool send_lobby_ping(void);
        bool ping_clients(void);
        bool client_test_sale_and_send(int unit_type,int unit_counter);
        bool server_test_sale_and_send(SOCKET soc_player,int unit_type,int unit_counter);//for server
        bool sale_update(SOCKET soc_player,int unit_type,int unit_counter,float price_total);
        bool server_starts_the_game(void);
        bool clean_up(void);
        bool buyout_test(int buyer_soc_id,int victim_soc_id,float& victim_value_x2);
        bool buyout_update(int buyer_soc_id,int victim_soc_id,float victim_value_x2);
        bool resend_packages_test(void);
        bool lost_player_test(void);
        bool show_data(void);
        bool send_data_refresh(void);
        bool send_inventory_refresh(void);
};

GLuint loadTexture(image* Image);

#endif // KAUFEN_H
