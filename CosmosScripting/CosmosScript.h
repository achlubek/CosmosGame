#pragma once
#include "chaiscript\chaiscript.hpp"
#include "chaiscript\chaiscript_stdlib.hpp"
class CosmosScript
{
public:
    CosmosScript(){
        script = new chaiscript::ChaiScript(chaiscript::Std_Lib::library());
    }
    ~CosmosScript()
    {
        delete script;
    }
    chaiscript::ChaiScript* script;
};

