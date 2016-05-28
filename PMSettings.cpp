#include "PMSettings.h"

bool PMSettings::DebugEnabled = false;
bool PMSettings::NoConversion = false;

bool PMSettings::IsDebugEnabled()
{
   return DebugEnabled;
}

bool PMSettings::IsConversionDisabled()
{
   return NoConversion;
}

PMSettings::PMSettings(bool debug, bool noConversion)
{
   DebugEnabled = debug;
   NoConversion = noConversion;
}
