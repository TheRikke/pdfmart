#ifndef PMSETTINGS_H
#define PMSETTINGS_H


class PMSettings
{
public:

   PMSettings(bool, bool);
   static bool IsDebugEnabled();
   static bool IsConversionDisabled();
private:
   static bool DebugEnabled;
   static bool NoConversion;
};

#endif // PMSETTINGS_H
