function deepcopy(orig)
    local orig_type = type(orig)
    local copy
    if orig_type == 'table' then
        copy = {}
        for orig_key, orig_value in next, orig, nil do
            copy[deepcopy(orig_key)] = deepcopy(orig_value)
        end
        setmetatable(copy, deepcopy(getmetatable(orig)))
    else -- number, string, boolean, etc
        copy = orig
    end
    return copy
end

function OnCreate()
    print("Created a new lua script 3!")
end

function OnUpdate()
    --print(GetTransform().transform.pos.x)
    --GetTransform.transform.pos.x = GetTransform.transform.pos.x + 2
    --trans.pos.x = trans.pos.x + 1
    --print("test")
    --print(i)
    trans = deepcopy(GetTransform)
    trans.transform.pos.x = trans.transform.pos.x + 1
    
    --GetTransform().transform.pos.x = trans.transform.pos.x
end