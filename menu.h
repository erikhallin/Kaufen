#ifndef MENU_H
#define MENU_H

#include "decal.h"
#include "button.h"
#include "text_input_box.h"
#include "graph.h"
#include "noise_generator.h"
#include <vector>

using namespace std;

class menu
{
    public:
        //Constructors
        menu();

        //Variables
        bool m_ready;

        //Functions
        bool   init(int window_width,int window_height,int texture_buttons,int texture_font[3],int texture_help,sound* pSoundEngine,float time,float version);
        int    update(bool keys[],int mouse_pos[],bool mouse_but[],float time);
        bool   draw();
        bool   set_IP(string sIP);
        string get_IP(void);
        string get_player_name(void);
        bool   set_player_name(string name);
        string get_server_name(void);
        bool   info_missing(string type);

    private:
        //Variables
        bool            m_show_help;
        float           m_LMB_delay,m_last_time,m_add_graph_point_time;
        decal           m_deTitle,m_deHelp,m_deCred;
        button          m_buHowTo,m_buHost,m_buJoin,m_buExit,m_buSearchLAN;
        text_input_box  m_tibServer_name,m_tibPlayer_name,m_tibIP_number;
        display         m_diVersion;
        vector<string>  m_vServers;
        vector<graph>   m_vBackgroundGraphs;
        vector<noise_generator>  m_vNoise_gens,m_vNoise_gens_sec;

        //Functions
};

#endif // MENU_H
