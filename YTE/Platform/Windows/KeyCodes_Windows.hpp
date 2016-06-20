//////////////////////////////////////////////
// Author: Joshua T. Fisher
//////////////////////////////////////////////
#pragma once

#ifndef KeyCodes_Windows_hpp
#define KeyCodes_Windows_hpp

#include "YTE/Core/Types.hpp"

// These are from documentation from MSDN on Virtual Key Codes,
// we manually define them so we don't have to poison our normal 
// headers with Windows.h spam.
//
// TODO: These values will need to be changed to support other 
//       non US regions.
const int Defined_Key_Backspace = 0x08;
const int Defined_Key_Tab       = 0x09;
const int Defined_Key_Enter     = 0x0D;
const int Defined_Key_Shift     = 0x10;
const int Defined_Key_Control   = 0x11;
const int Defined_Key_Menu      = 0x12;
const int Defined_Key_CapsLock  = 0x14;
const int Defined_Key_Escape    = 0x1B;
const int Defined_Key_Space     = 0x20;
const int Defined_Key_PageUp    = 0x21;
const int Defined_Key_PageDown  = 0x22;
const int Defined_Key_End       = 0x23;
const int Defined_Key_Home      = 0x24;
const int Defined_Key_Left      = 0x25;
const int Defined_Key_Up        = 0x26;
const int Defined_Key_Right     = 0x27;
const int Defined_Key_Down      = 0x28;
const int Defined_Key_Insert    = 0x2D;
const int Defined_Key_Delete    = 0x2E;
const int Defined_Key_0         = 0x30;
const int Defined_Key_1         = 0x31;
const int Defined_Key_2         = 0x32;
const int Defined_Key_3         = 0x33;
const int Defined_Key_4         = 0x34;
const int Defined_Key_5         = 0x35;
const int Defined_Key_6         = 0x36;
const int Defined_Key_7         = 0x37;
const int Defined_Key_8         = 0x38;
const int Defined_Key_9         = 0x39;
const int Defined_Key_A         = 0x41;
const int Defined_Key_B         = 0x42;
const int Defined_Key_C         = 0x43;
const int Defined_Key_D         = 0x44;
const int Defined_Key_E         = 0x45;
const int Defined_Key_F         = 0x46;
const int Defined_Key_G         = 0x47;
const int Defined_Key_H         = 0x48;
const int Defined_Key_I         = 0x49;
const int Defined_Key_J         = 0x4A;
const int Defined_Key_K         = 0x4B;
const int Defined_Key_L         = 0x4C;
const int Defined_Key_M         = 0x4D;
const int Defined_Key_N         = 0x4E;
const int Defined_Key_O         = 0x4F;
const int Defined_Key_P         = 0x50;
const int Defined_Key_Q         = 0x51;
const int Defined_Key_R         = 0x52;
const int Defined_Key_S         = 0x53;
const int Defined_Key_T         = 0x54;
const int Defined_Key_U         = 0x55;
const int Defined_Key_V         = 0x56;
const int Defined_Key_W         = 0x57;
const int Defined_Key_X         = 0x58;
const int Defined_Key_Y         = 0x59;
const int Defined_Key_Z         = 0x5A;
const int Defined_Key_Num0      = 0x60;
const int Defined_Key_Num1      = 0x61;
const int Defined_Key_Num2      = 0x62;
const int Defined_Key_Num3      = 0x63;
const int Defined_Key_Num4      = 0x64;
const int Defined_Key_Num5      = 0x65;
const int Defined_Key_Num6      = 0x66;
const int Defined_Key_Num7      = 0x67;
const int Defined_Key_Num8      = 0x68;
const int Defined_Key_Num9      = 0x69;

#endif
