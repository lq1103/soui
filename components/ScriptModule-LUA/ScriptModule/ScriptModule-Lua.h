#pragma  once

#include <interface/SScriptModule-i.h>
#include <unknown/obj-ref-impl.hpp>
extern "C"
{
#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>
};

namespace SOUI
{
    class SScriptModule_Lua : public SOUI::TObjRefImpl<IScriptModule>
    {
    public:
        SScriptModule_Lua(void);

        ~SScriptModule_Lua();

        virtual void * GetScriptEngine () {return d_state;}

        virtual	void	executeScriptFile(LPCSTR pszScriptFile);
        virtual	void	executeScriptBuffer(const char* buff, size_t sz);
        virtual void    executeString(LPCSTR str);

        virtual	bool	executeScriptedEventHandler(LPCSTR handler_name,SOUI::EventArgs *pEvt);

        virtual LPCSTR getIdentifierString() const;

        virtual bool subscribeEvent(SOUI::SWindow* target, UINT uEvent, LPCSTR subscriber_name);
        virtual bool unsubscribeEvent(SOUI::SWindow* target, UINT uEvent, LPCSTR subscriber_name );
    protected:
        lua_State * d_state;
    };

    namespace SCRIPT_LUA
    {
        SOUI_COM_C BOOL SOUI_COM_API SCreateInstance(IObjRef ** ppScript);
    }
}