#include "console.h"

#define _key_delay 0.2

console::console()
{
    m_ready=false;
}

console::console(int x_pos,int y_pos,int width,int height,int max_char,int numof_displayed_messages,int font_texture[3],int texture,
                 float tex_offset_x_min,float tex_offset_x_max,float tex_offset_y_min,float tex_offset_y_max,sound* pSoundEngine)
{
    m_handle_message_locally=false;

    m_pSoundEngine=pSoundEngine;

    for(int i=0;i<256;i++) m_key_delay[i]=0;
    m_caps_lock=false;//get state first
    m_time_last_cycle=0;
    m_x_pos=x_pos;
    m_y_pos=y_pos;
    m_width=width;
    m_height=height;
    m_numof_displayed_messages=numof_displayed_messages;
    m_texture=texture;
    m_max_char=max_char;

    //create decal (background)
    m_deBackground=decal(x_pos,y_pos,width,height,texture,tex_offset_x_min,tex_offset_x_max,tex_offset_y_min,tex_offset_y_max);

    //create displays
    float di_height=float(height)/float(m_numof_displayed_messages+1);// +1 for owner temp string

    for(int i=0;i<m_numof_displayed_messages;i++)
    {
        m_vDisplays.push_back(display( x_pos,int(y_pos+di_height*(float)i),width,int(di_height),max_char,font_texture,">" ));
    }
    m_diOwner_temp_string=display( x_pos,int(y_pos+di_height*(float)m_numof_displayed_messages),width,int(di_height),max_char,font_texture,"> Type text to send to other players" );
    //turn off borders
    for(int i=0;i<(int)m_vDisplays.size();i++)
    {
        bool border=false;
        m_vDisplays[i].setting_flags(border);
    }
    m_diOwner_temp_string.setting_flags(false);

    m_ready=true;
}

bool console::input_text(string text_string)
{
    //update displays
    for(int i=0;i<(int)m_vDisplays.size()-1;i++)
    {
        m_vDisplays[i].set_text( m_vDisplays[i+1].get_text() );
    }
    m_vDisplays[(int)m_vDisplays.size()-1].set_text(text_string);

    return true;
}

bool console::input_text(string sender_tag,string text_string)
{
    //convert to message
    message temp_message;
    temp_message.sender_tag=sender_tag;
    temp_message.text_string=text_string;
    m_vMessages.push_back(temp_message);
    //convert to full string
    string message_string_full=sender_tag;
    message_string_full.append(" - ");
    message_string_full.append(text_string);
    //update displays
    for(int i=0;i<(int)m_vDisplays.size()-1;i++)
    {
        m_vDisplays[i].set_text( m_vDisplays[i+1].get_text() );
    }
    m_vDisplays[(int)m_vDisplays.size()-1].set_text(message_string_full);

    return true;
}

int console::update(bool keys[],float time,string& message_to_send)
{
    //timing
    for(int i=0;i<256;i++)
    {
        if(m_key_delay[i]>0)
        {
            m_key_delay[i]-=time-m_time_last_cycle;
        }
    }
    m_time_last_cycle=time;

    //check for function key input
    bool key_shift=false;
    //bool key_alt_left=false; XXXXX
    //bool key_alt_right=false; XXXXX
    if(keys[16])//shift
    {
        key_shift=true;
    }
    else key_shift=false;

    if(keys[20]&&m_key_delay[20]<=0)//caps toogle
    {
        m_caps_lock=!m_caps_lock;
        m_key_delay[13]=_key_delay;
    }
    if(keys[13]&&m_key_delay[13]<=0)//enter
    {
        if( !m_owner_temp_string.empty() )
        {
            message_to_send=m_owner_temp_string;
            //if message should be displayed now?
            if(m_handle_message_locally)
            {
                send_owner_string();
                return 1;
            }
            else
            {
                m_owner_temp_string.clear();
                m_diOwner_temp_string.set_text("");
                return 1;
            }
        }
        m_key_delay[13]=_key_delay;
    }
    if(keys[8] &&m_key_delay[8] <=0)//backspace
    {
        erase_char_from_owner_string();
        m_key_delay[8]=_key_delay;
        m_diOwner_temp_string.set_text(m_owner_temp_string);
    }

    //special keys
    if(keys[190] &&m_key_delay[190] <=0)//.
    {
        m_key_delay[190]=_key_delay;
        if(key_shift) add_char_to_owner_string(':');
        else add_char_to_owner_string('.');
        m_diOwner_temp_string.set_text(m_owner_temp_string);
    }
    if(keys[188] &&m_key_delay[188] <=0)//,
    {
        m_key_delay[188]=_key_delay;
        if(key_shift) add_char_to_owner_string(';');
        else add_char_to_owner_string(',');
        m_diOwner_temp_string.set_text(m_owner_temp_string);
    }
    if(keys[189] &&m_key_delay[189] <=0)//-
    {
        m_key_delay[189]=_key_delay;
        if(key_shift) add_char_to_owner_string('_');
        else add_char_to_owner_string('-');
        m_diOwner_temp_string.set_text(m_owner_temp_string);
    }
    if(keys[189] &&m_key_delay[189] <=0)//-
    {
        m_key_delay[189]=_key_delay;
        if(key_shift) add_char_to_owner_string('_');
        else add_char_to_owner_string('-');
        m_diOwner_temp_string.set_text(m_owner_temp_string);
    }
    if(keys[187] &&m_key_delay[187] <=0)//+/?
    {
        m_key_delay[187]=_key_delay;
        if(key_shift) add_char_to_owner_string('?');
        else add_char_to_owner_string('+');
        m_diOwner_temp_string.set_text(m_owner_temp_string);
    }

    if(keys[221] &&m_key_delay[221] <=0)//å
    {
        m_key_delay[221]=_key_delay;
        if(key_shift)
        {
            if(m_caps_lock) add_char_to_owner_string('\x86');
            else            add_char_to_owner_string('\x8F');
        }
        else
        {
            if(m_caps_lock) add_char_to_owner_string('\x8F');
            else            add_char_to_owner_string('\x86');
        }
        m_diOwner_temp_string.set_text(m_owner_temp_string);
    }
    if(keys[222] &&m_key_delay[222] <=0)//ä
    {
        m_key_delay[222]=_key_delay;
        if(key_shift)
        {
            if(m_caps_lock) add_char_to_owner_string('\x84');
            else            add_char_to_owner_string('\x8E');
        }
        else
        {
            if(m_caps_lock) add_char_to_owner_string('\x8E');
            else            add_char_to_owner_string('\x84');
        }
        m_diOwner_temp_string.set_text(m_owner_temp_string);
    }
    if(keys[192] &&m_key_delay[192] <=0)//ö
    {
        m_key_delay[192]=_key_delay;
        if(key_shift)
        {
            if(m_caps_lock) add_char_to_owner_string('\x94');
            else            add_char_to_owner_string('\x99');
        }
        else
        {
            if(m_caps_lock) add_char_to_owner_string('\x99');
            else            add_char_to_owner_string('\x94');
        }
        m_diOwner_temp_string.set_text(m_owner_temp_string);
    }
    //Numpad
    if(keys[96] &&m_key_delay[96] <=0)//0
    {
        m_key_delay[96]=_key_delay;
        add_char_to_owner_string('0');
        m_diOwner_temp_string.set_text(m_owner_temp_string);
    }
    if(keys[97] &&m_key_delay[97] <=0)//1
    {
        m_key_delay[97]=_key_delay;
        add_char_to_owner_string('1');
        m_diOwner_temp_string.set_text(m_owner_temp_string);
    }
    if(keys[98] &&m_key_delay[98] <=0)//2
    {
        m_key_delay[98]=_key_delay;
        add_char_to_owner_string('2');
        m_diOwner_temp_string.set_text(m_owner_temp_string);
    }
    if(keys[99] &&m_key_delay[99] <=0)//3
    {
        m_key_delay[99]=_key_delay;
        add_char_to_owner_string('3');
        m_diOwner_temp_string.set_text(m_owner_temp_string);
    }
    if(keys[100] &&m_key_delay[100] <=0)//4
    {
        m_key_delay[100]=_key_delay;
        add_char_to_owner_string('4');
        m_diOwner_temp_string.set_text(m_owner_temp_string);
    }
    if(keys[101] &&m_key_delay[101] <=0)//5
    {
        m_key_delay[101]=_key_delay;
        add_char_to_owner_string('5');
        m_diOwner_temp_string.set_text(m_owner_temp_string);
    }
    if(keys[102] &&m_key_delay[102] <=0)//6
    {
        m_key_delay[102]=_key_delay;
        add_char_to_owner_string('6');
        m_diOwner_temp_string.set_text(m_owner_temp_string);
    }
    if(keys[103] &&m_key_delay[103] <=0)//7
    {
        m_key_delay[103]=_key_delay;
        add_char_to_owner_string('7');
        m_diOwner_temp_string.set_text(m_owner_temp_string);
    }
    if(keys[104] &&m_key_delay[104] <=0)//8
    {
        m_key_delay[104]=_key_delay;
        add_char_to_owner_string('8');
        m_diOwner_temp_string.set_text(m_owner_temp_string);
    }
    if(keys[105] &&m_key_delay[105] <=0)//9
    {
        m_key_delay[105]=_key_delay;
        add_char_to_owner_string('9');
        m_diOwner_temp_string.set_text(m_owner_temp_string);
    }
    if(keys[111] &&m_key_delay[111] <=0)///
    {
        m_key_delay[111]=_key_delay;
        add_char_to_owner_string('/');
        m_diOwner_temp_string.set_text(m_owner_temp_string);
    }
    if(keys[106] &&m_key_delay[106] <=0)//*
    {
        m_key_delay[106]=_key_delay;
        add_char_to_owner_string('*');
        m_diOwner_temp_string.set_text(m_owner_temp_string);
    }
    if(keys[109] &&m_key_delay[109] <=0)//-
    {
        m_key_delay[109]=_key_delay;
        add_char_to_owner_string('-');
        m_diOwner_temp_string.set_text(m_owner_temp_string);
    }
    if(keys[107] &&m_key_delay[107] <=0)//+
    {
        m_key_delay[107]=_key_delay;
        add_char_to_owner_string('+');
        m_diOwner_temp_string.set_text(m_owner_temp_string);
    }
    if(keys[110] &&m_key_delay[110] <=0)//,
    {
        m_key_delay[110]=_key_delay;
        add_char_to_owner_string(',');
        m_diOwner_temp_string.set_text(m_owner_temp_string);
    }

    //check for char input
    for(int i=32;i<127;i++)
    {
        if(keys[i]&&m_key_delay[i]<=0)
        {
            if(key_shift) //caps or symbol
            {
                if(isalpha(int_to_char(i)))//alphabetic
                {
                    if(m_caps_lock)
                    {
                        add_char_to_owner_string(tolower(int_to_char(i)));//small
                    }
                    else add_char_to_owner_string(int_to_char(i));//big
                }
                else //convert number to symbol
                {
                    if(i==48) add_char_to_owner_string(int_to_char(61));//shift+0 to =
                    else      add_char_to_owner_string(int_to_char(i-16));
                }
            }
            else //not shift
            {
                if(m_caps_lock)
                {
                    add_char_to_owner_string(int_to_char(i));//big
                }
                else add_char_to_owner_string(tolower(int_to_char(i)));//small
            }
            m_key_delay[i]=_key_delay;
            m_diOwner_temp_string.set_text(m_owner_temp_string);
        }
    }

    return 0;
}

bool console::draw(void)
{
    //m_deBackground.draw();
    for(int i=0;i<(int)m_vDisplays.size();i++)
    {
        m_vDisplays[i].draw_display();
    }
    m_diOwner_temp_string.draw_display();

    return true;
}

bool console::add_char_to_owner_string(char letter)
{
    if( (int)m_owner_temp_string.size() < m_max_char)
    {
        m_owner_temp_string.append(1,letter);//add char
        //play sound
        m_pSoundEngine->playSimpleSound(wav_typing,0.6f);
    }

    return true;
}

bool console::erase_char_from_owner_string(void)
{
    if((int)m_owner_temp_string.size()>0)
    {
        m_owner_temp_string.erase( m_owner_temp_string.end()-1 );
        m_diOwner_temp_string.set_text(m_owner_temp_string);
        //play sound
        m_pSoundEngine->playSimpleSound(wav_typing,0.6f);
    }

    return true;
}

bool console::send_owner_string(void)
{
    input_text("You",m_owner_temp_string);
    m_owner_temp_string.clear();
    m_diOwner_temp_string.set_text(m_owner_temp_string);

    return true;
}

char console::int_to_char(int key_id)
{
    //int to char conversion
    switch(key_id)
    {
        case 32: return ' ';
        case 33: return '!';
        case 34: return '"';
        case 35: return '#';
        case 36: return '$';
        case 37: return '%';
        case 38: return '&';
        case 39: return '/';
        case 40: return '(';
        case 41: return ')';
        case 42: return '*';
        case 43: return '+';
        case 44: return ',';
        case 45: return '-';
        case 46: return '.';
        case 47: return '/';

        case 48: return '0';
        case 49: return '1';
        case 50: return '2';
        case 51: return '3';
        case 52: return '4';
        case 53: return '5';
        case 54: return '6';
        case 55: return '7';
        case 56: return '8';
        case 57: return '9';

        case 58: return ':';
        case 59: return ';';
        case 60: return '<';
        case 61: return '=';
        case 62: return '>';
        case 63: return '?';
        case 64: return '@';

        case 65: return 'A';
        case 66: return 'B';
        case 67: return 'C';
        case 68: return 'D';
        case 69: return 'E';
        case 70: return 'F';
        case 71: return 'G';
        case 72: return 'H';
        case 73: return 'I';
        case 74: return 'J';
        case 75: return 'K';
        case 76: return 'L';
        case 77: return 'M';
        case 78: return 'N';
        case 79: return 'O';
        case 80: return 'P';
        case 81: return 'Q';
        case 82: return 'R';
        case 83: return 'S';
        case 84: return 'T';
        case 85: return 'U';
        case 86: return 'V';
        case 87: return 'W';
        case 88: return 'X';
        case 89: return 'Y';
        case 90: return 'Z';

        case 91: return '[';
        case 92: return '\\';
        case 93: return ']';
        case 94: return '^';
        case 95: return '_';
        case 96: return '`';

        case 97: return 'a';
        case 98: return 'b';
        case 99: return 'c';
        case 100: return 'd';
        case 101: return 'e';
        case 102: return 'f';
        case 103: return 'g';
        case 104: return 'h';
        case 105: return 'i';
        case 106: return 'j';
        case 107: return 'k';
        case 108: return 'l';
        case 109: return 'm';
        case 110: return 'n';
        case 111: return 'o';
        case 112: return 'p';
        case 113: return 'q';
        case 114: return 'r';
        case 115: return 's';
        case 116: return 't';
        case 117: return 'u';
        case 118: return 'v';
        case 119: return 'w';
        case 120: return 'x';
        case 121: return 'y';
        case 122: return 'z';

        case 123: return '{';
        case 124: return '|';
        case 125: return '}';
        case 126: return '~';

    }
    return 'x';//error
}
