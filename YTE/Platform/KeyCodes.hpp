//////////////////////////////////////////////
// Author: Joshua T. Fisher
//////////////////////////////////////////////
#pragma once

#ifndef KeyCodes_hpp
#define KeyCodes_hpp

#include "YTE/Core/TargetDefinitions.hpp"
#include "YTE/Core/Types.hpp"

#ifdef Windows
  #include "YTE/Platform/Windows/KeyCodes_Windows.hpp"
#endif

namespace YTE
{
  enum class KeyCode : byte
  {
    Backspace = Defined_Key_Backspace,
    Tab = Defined_Key_Tab,
    Enter = Defined_Key_Enter,
    Shift = Defined_Key_Shift,
    Control = Defined_Key_Control,
    Menu = Defined_Key_Menu,
    CapsLock = Defined_Key_CapsLock,
    Escape = Defined_Key_Escape,
    Space = Defined_Key_Space,
    PageUp = Defined_Key_PageUp,
    PageDown = Defined_Key_PageDown,
    End = Defined_Key_End,
    Home = Defined_Key_Home,
    Left = Defined_Key_Left,
    Up = Defined_Key_Up,
    Right = Defined_Key_Right,
    Down = Defined_Key_Down,
    Insert = Defined_Key_Insert,
    Delete = Defined_Key_Delete,
    N0 = Defined_Key_0,
    N1 = Defined_Key_1,
    N2 = Defined_Key_2,
    N3 = Defined_Key_3,
    N4 = Defined_Key_4,
    N5 = Defined_Key_5,
    N6 = Defined_Key_6,
    N7 = Defined_Key_7,
    N8 = Defined_Key_8,
    N9 = Defined_Key_9,
    A = Defined_Key_A,
    B = Defined_Key_B,
    C = Defined_Key_C,
    D = Defined_Key_D,
    E = Defined_Key_E,
    F = Defined_Key_F,
    G = Defined_Key_G,
    H = Defined_Key_H,
    I = Defined_Key_I,
    J = Defined_Key_J,
    K = Defined_Key_K,
    L = Defined_Key_L,
    M = Defined_Key_M,
    N = Defined_Key_N,
    O = Defined_Key_O,
    P = Defined_Key_P,
    Q = Defined_Key_Q,
    R = Defined_Key_R,
    S = Defined_Key_S,
    T = Defined_Key_T,
    U = Defined_Key_U,
    V = Defined_Key_V,
    W = Defined_Key_W,
    X = Defined_Key_X,
    Y = Defined_Key_Y,
    Z = Defined_Key_Z,
    Num0 = Defined_Key_Num0,
    Num1 = Defined_Key_Num1,
    Num2 = Defined_Key_Num2,
    Num3 = Defined_Key_Num3,
    Num4 = Defined_Key_Num4,
    Num5 = Defined_Key_Num5,
    Num6 = Defined_Key_Num6,
    Num7 = Defined_Key_Num7,
    Num8 = Defined_Key_Num8,
    Num9 = Defined_Key_Num9,
  };
}

#endif
