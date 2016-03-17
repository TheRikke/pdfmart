#ifndef PMSETTINGS_H
#define PMSETTINGS_H


class PMSettings
{
public:

   PMSettings(bool);
   static bool IsDebugEnabled();
private:
   static bool DebugEnabled;
};

#endif // PMSETTINGS_H
