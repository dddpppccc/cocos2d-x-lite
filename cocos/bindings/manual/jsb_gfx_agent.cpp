/****************************************************************************
 Copyright (c) 2021 Xiamen Yaji Software Co., Ltd.

 http://www.cocos.com

 Permission is hereby granted, free of charge, to any person obtaining a copy
 of this software and associated engine source code (the "Software"), a limited,
 worldwide, royalty-free, non-assignable, revocable and non-exclusive license
 to use Cocos Creator solely to develop games on your target platforms. You shall
 not use Cocos Creator software for developing other software or tools that's
 used for developing games. You are not granted to publish, distribute,
 sublicense, and/or sell copies of Cocos Creator.

 The software or tools in this License Agreement are licensed, not sold.
 Xiamen Yaji Software Co., Ltd. reserves all rights not expressly granted to you.

 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 THE SOFTWARE.
****************************************************************************/

#include "cocos/bindings/manual/jsb_gfx_agent.h"
#include "cocos/bindings/manual/jsb_conversions.h"
#include "cocos/bindings/manual/jsb_global.h"
#include "gfx-agent/GFXDeviceAgent.h"

#ifndef JSB_ALLOC
#define JSB_ALLOC(kls, ...) new (std::nothrow) kls(__VA_ARGS__)
#endif

#ifndef JSB_FREE
#define JSB_FREE(ptr) delete ptr
#endif
se::Object* __jsb_cc_gfx_DeviceAgent_proto = nullptr;
se::Class* __jsb_cc_gfx_DeviceAgent_class = nullptr;

SE_DECLARE_FINALIZE_FUNC(js_cc_gfx_DeviceAgent_finalize)

static bool js_gfx_agent_DeviceAgent_constructor(se::State& s) // constructor.c
{
    CC_UNUSED bool ok = true;
    const auto& args = s.args();
    cc::gfx::Device* arg0 = nullptr;
    ok &= sevalue_to_native(args[0], &arg0, s.thisObject());
    SE_PRECONDITION2(ok, false, "js_gfx_agent_DeviceAgent_constructor : Error processing arguments");
    cc::gfx::DeviceAgent* cobj = JSB_ALLOC(cc::gfx::DeviceAgent, arg0);
    s.thisObject()->setPrivateData(cobj);
    args[0].toObject()->clearPrivateData(true); // actor device is now under agent's full control (unique_ptr)
    se::NonRefNativePtrCreatedByCtorMap::emplace(cobj);
    return true;
}
SE_BIND_CTOR(js_gfx_agent_DeviceAgent_constructor, __jsb_cc_gfx_DeviceAgent_class, js_cc_gfx_DeviceAgent_finalize)



extern se::Object* __jsb_cc_gfx_Device_proto;

static bool js_cc_gfx_DeviceAgent_finalize(se::State& s)
{
    auto iter = se::NonRefNativePtrCreatedByCtorMap::find(SE_THIS_OBJECT<cc::gfx::DeviceAgent>(s));
    if (iter != se::NonRefNativePtrCreatedByCtorMap::end())
    {
        se::NonRefNativePtrCreatedByCtorMap::erase(iter);
        cc::gfx::DeviceAgent* cobj = SE_THIS_OBJECT<cc::gfx::DeviceAgent>(s);
        // device destruction should happen after all resources which we have no way to control through JS object lifecycles
        //JSB_FREE(cobj); // so just skip it here and do it manually
    }
    return true;
}
SE_BIND_FINALIZE_FUNC(js_cc_gfx_DeviceAgent_finalize)

bool js_register_gfx_agent_DeviceAgent(se::Object* obj)
{
    auto cls = se::Class::create("DeviceAgent", obj, __jsb_cc_gfx_Device_proto, _SE(js_gfx_agent_DeviceAgent_constructor));

    cls->defineFinalizeFunction(_SE(js_cc_gfx_DeviceAgent_finalize));
    cls->install();
    JSBClassType::registerClass<cc::gfx::DeviceAgent>(cls);

    __jsb_cc_gfx_DeviceAgent_proto = cls->getProto();
    __jsb_cc_gfx_DeviceAgent_class = cls;

    se::ScriptEngine::getInstance()->clearException();
    return true;
}

bool register_all_gfx_agent(se::Object* obj)
{
    // Get the ns
    se::Value nsVal;
    if (!obj->getProperty("gfx", &nsVal))
    {
        se::HandleObject jsobj(se::Object::createPlainObject());
        nsVal.setObject(jsobj);
        obj->setProperty("gfx", nsVal);
    }
    se::Object* ns = nsVal.toObject();

    js_register_gfx_agent_DeviceAgent(ns);
    return true;
}
