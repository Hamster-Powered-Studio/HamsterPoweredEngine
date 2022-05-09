function OnCreate()
    print("Created a new lua script!")
    speed = 500
    
end

function ValueInRange(value, min, max) 
    return (value >= min) and (value <= max)
end

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

function Intersects(RectA, RectB)
    RectALeft = RectA.Bounds.Left + RectA.Offset.x
    RectATop = RectA.Bounds.Top + RectA.Offset.y
    RectBLeft = RectB.Bounds.Left + RectB.Offset.x
    RectBTop = RectB.Bounds.Top + RectB.Offset.y
    
    xOverlap = ValueInRange(RectALeft, RectBLeft, RectBLeft + RectB.Bounds.Width) or ValueInRange(RectBLeft, RectALeft, RectALeft + RectA.Bounds.Width)
    yOverlap = ValueInRange(RectATop, RectBTop, RectBTop + RectB.Bounds.Height) or ValueInRange(RectBTop, RectATop, RectATop + RectA.Bounds.Height)
    
    return xOverlap and yOverlap
end


function OnUpdate(deltaTime)
    xMovement = 0;
    yMovement = 0;
    
    if IsMouseDown(0) then
    end
    if IsKeyDown("W") then
        yMovement = yMovement - speed * deltaTime
    end
    if IsKeyDown("A") then
        xMovement = xMovement - speed * deltaTime
    end
    if IsKeyDown("S") then
        yMovement = yMovement + speed * deltaTime
    end 
    if IsKeyDown("D") then
        xMovement = xMovement + speed * deltaTime
    end
    
    local FuturePositionX = Self.GetCollider(Self).CopyBox(Self.GetCollider(Self))
    local FuturePositionY = Self.GetCollider(Self).CopyBox(Self.GetCollider(Self))
    FuturePositionY.Bounds.Top = FuturePositionY.Bounds.Top + yMovement
    FuturePositionX.Bounds.Left = FuturePositionX.Bounds.Left + xMovement

    for i,v in ipairs(Scene.GetAllColliders(Scene)) do
        if v.Active then
            if (v ~= Self.GetCollider(Self)) then
                if (Intersects(Self.GetCollider(Self), v) and v.Type == "Overlap") then
                    print ("Overlap!")
                    break
                end
                if (Intersects(FuturePositionX, v) and v.Type == "Block") then
                    xMovement = 0
                end
                if (Intersects(FuturePositionY, v) and v.Type == "Block") then
                    yMovement = 0
                end
            end
        end
    end
    
    Self.GetTransform(Self).Transform.Pos.x = Self.GetTransform(Self).Transform.Pos.x + xMovement
    Self.GetTransform(Self).Transform.Pos.y = Self.GetTransform(Self).Transform.Pos.y + yMovement
    
end

function OnDestroy()
    print ("Destroyed!")
end