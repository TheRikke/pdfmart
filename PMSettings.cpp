#include "PMSettings.h"

bool PMSettings::DebugEnabled = false;

bool PMSettings::IsDebugEnabled()
{
   return DebugEnabled;
}

PMSettings::PMSettings(bool debug)
{
   DebugEnabled = debug;
}
