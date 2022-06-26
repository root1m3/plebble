//===-                           P L E B B L E
//===-                         https://plebble.us
//===-
//===-              Copyright (C) 2017-2022 root1m3@plebble.us
//===-
//===-                      GNU GENERAL PUBLIC LICENSE
//===-                       Version 3, 29 June 2007
//===-
//===-    This program is free software: you can redistribute it and/or modify
//===-    it under the terms of the GPLv3 License as published by the Free
//===-    Software Foundation.
//===-
//===-    This program is distributed in the hope that it will be useful,
//===-    but WITHOUT ANY WARRANTY; without even the implied warranty of
//===-    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
//===-
//===-    You should have received a copy of the General Public License
//===-    along with this program, see LICENCE file.
//===-    see https://www.gnu.org/licenses
//===-
//===----------------------------------------------------------------------------
//===-
package com.jstyle.blesdk2025.model;

/**
 * Created by Administrator on 2018/8/2.
 */

public class ExerciseMode extends SendData {
    public static final int Status_START=1;
    public static final int Status_PAUSE=2;
    public static final int Status_CONTUINE=3;
    public static final int Status_FINISH=4;

    public static final int Mode_RUN=0;
    public static final int Mode_CYCLING=1;
    public static final int Mode_BADMINTON=2;
    public static final int Mode_FOOTBALL=3;
    public static final int Mode_TENNIS=4;
    public static final int Mode_YOGA=5;
    public static final int Mode_BREATH=6;
    public static final int Mode_DANCE=7;
    public static final int Mode_BASKETBALL=8;
    public static final int Mode_CLIMB=9;
    public static final int Mode_workout=10;
    public static int[]modes=new int[]{Mode_RUN,Mode_CYCLING,Mode_BADMINTON,Mode_FOOTBALL,
            Mode_TENNIS,Mode_YOGA,Mode_BREATH,Mode_DANCE,Mode_BASKETBALL,Mode_CLIMB,Mode_workout};
    int exerciseMode;
    int enableStatus;
    public int getExerciseMode(int position){
        return modes[position];
    }
    public int getExerciseMode() {
        return exerciseMode;
    }

    public void setExerciseMode(int exerciseMode) {
        this.exerciseMode = exerciseMode;
    }

    public int getEnableStatus() {
        return enableStatus;
    }

    public void setEnableStatus(int enableStatus) {
        this.enableStatus = enableStatus;
    }
}
