#ifndef COIN_ANIM_H
#define COIN_ANIM_H

#include <stdlib.h>
#include "decal.h"

class coin_anim
{
    public:
        coin_anim(int x_pos,int y_pos,int width,int height,int texture,
                  float tex_offset_x_min,float tex_offset_x_max,float tex_offset_y_min,float tex_offset_y_max,
                  float rot_speed,float fall_speed,int screen_width,int screen_height);

        bool m_ready;

        bool update(float time);
        bool draw(void);

    private:

        int   m_screen_height,m_screen_width,m_size,m_current_pic; //0-7
        float m_x_pos,m_y_pos,m_rot_speed,m_fall_speed,m_last_time,m_rot_progress;

        decal m_deCoin_pic;

};

#endif // COIN_ANIM_H
