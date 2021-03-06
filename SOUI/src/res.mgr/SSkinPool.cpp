#include "souistd.h"
#include "res.mgr/SSkinPool.h"
#include "core/Sskin.h"
#include "SApp.h"
#include "helper/mybuffer.h"

namespace SOUI
{

//////////////////////////////////////////////////////////////////////////
// SSkinPool

SSkinPool::SSkinPool()
{
    m_pFunOnKeyRemoved=OnKeyRemoved;
}

SSkinPool::~SSkinPool()
{
}

int SSkinPool::LoadSkins(pugi::xml_node xmlNode)
{
    if(!xmlNode) return 0;
    
    int nLoaded=0;
    SStringW strSkinName, strTypeName;

    pugi::xml_node xmlSkin=xmlNode.first_child();
    while(xmlSkin)
    {
        strTypeName = xmlSkin.name();
        strSkinName = xmlSkin.attribute(L"name").value();

        if (strSkinName.IsEmpty() || strTypeName.IsEmpty())
            continue;
            
        xmlSkin.attribute(L"name").set_userdata(1);    //标记该属性不需要再处理。
        
        SASSERT(!HasKey(strSkinName));
        ISkinObj *pSkin=SApplication::getSingleton().CreateSkinByName(strTypeName);
        if(pSkin)
        {
            pSkin->InitFromXml(xmlSkin);
            AddKeyObject(strSkinName,pSkin);
            nLoaded++;
        }
        else
        {
            SASSERT_FMTW(FALSE,L"load skin error,type=%s,name=%s",strTypeName,strSkinName);
        }
        xmlSkin.attribute(L"name").set_userdata(0);    //清除标记
        xmlSkin=xmlSkin.next_sibling();
    }

    return nLoaded;
}


ISkinObj* SSkinPool::GetSkin(LPCWSTR strSkinName)
{
    if(!HasKey(strSkinName))
    {
        return NULL;
    }
    return GetKeyObject(strSkinName);
}

void SSkinPool::OnKeyRemoved(const SSkinPtr & obj )
{
    obj->Release();
}

//////////////////////////////////////////////////////////////////////////
template<> SSkinPoolMgr * SSingleton<SSkinPoolMgr>::ms_Singleton=0;

SSkinPoolMgr::SSkinPoolMgr()
{
    m_bulitinSkinPool.Attach(new SSkinPool);
    PushSkinPool(m_bulitinSkinPool);
}

SSkinPoolMgr::~SSkinPoolMgr()
{
    SPOSITION pos=m_lstSkinPools.GetHeadPosition();
    while(pos)
    {
        SSkinPool *p = m_lstSkinPools.GetNext(pos);
        p->Release();
    }
    m_lstSkinPools.RemoveAll();

}

ISkinObj* SSkinPoolMgr::GetSkin( LPCWSTR strSkinName )
{
    SPOSITION pos=m_lstSkinPools.GetTailPosition();
    while(pos)
    {
        SSkinPool *pStylePool=m_lstSkinPools.GetPrev(pos);
        if(ISkinObj* pSkin=pStylePool->GetSkin(strSkinName))
        {
            return pSkin;
        }
    }
    SASSERT_FMTW(FALSE,L"GetSkin[%s] Failed!",strSkinName);
    return NULL;
}

const wchar_t * BUILDIN_SKIN_NAMES[]=
{
    L"_skin.sys.checkbox",
    L"_skin.sys.radio",
    L"_skin.sys.focuscheckbox",
    L"_skin.sys.focusradio",
    L"_skin.sys.btn.normal",
    L"_skin.sys.scrollbar",
    L"_skin.sys.border",
    L"_skin.sys.dropbtn",
    L"_skin.sys.tree.toggle",
    L"_skin.sys.tree.checkbox",
    L"_skin.sys.tab.page",
    L"_skin.sys.header",
    L"_skin.sys.split.vert",
    L"_skin.sys.split.horz",
    L"_skin.sys.prog.bkgnd",
    L"_skin.sys.prog.bar",
    L"_skin.sys.vert.prog.bkgnd",
    L"_skin.sys.vert.prog.bar",
    L"_skin.sys.slider.thumb",
    L"_skin.sys.btn.close",
    L"_skin.sys.btn.minimize",
    L"_skin.sys.btn.maxmize",
    L"_skin.sys.btn.restore",
    L"_skin.sys.menu.check",
    L"_skin.sys.menu.sep",
    L"_skin.sys.menu.border",
    L"_skin.sys.menu.skin",
    L"_skin.sys.icons",
    L"_skin.sys.wnd.bkgnd"
};


ISkinObj * SSkinPoolMgr::GetBuiltinSkin( SYS_SKIN uID )
{
    return GetBuiltinSkinPool()->GetSkin(BUILDIN_SKIN_NAMES[uID]);
}

void SSkinPoolMgr::PushSkinPool( SSkinPool *pSkinPool )
{
    m_lstSkinPools.AddTail(pSkinPool);
    pSkinPool->AddRef();
}

SSkinPool * SSkinPoolMgr::PopSkinPool(SSkinPool *pSkinPool)
{
    SSkinPool * pRet=NULL;
    if(pSkinPool)
    {
        if(pSkinPool == m_bulitinSkinPool) return NULL;

        SPOSITION pos=m_lstSkinPools.Find(pSkinPool);
        if(pos)
        {
            pRet=m_lstSkinPools.GetAt(pos);
            m_lstSkinPools.RemoveAt(pos);
        }
    }else
    {
        pRet = m_lstSkinPools.RemoveTail();
    }
    if(pRet) pRet->Release();
    return pRet;
}

}//namespace SOUI