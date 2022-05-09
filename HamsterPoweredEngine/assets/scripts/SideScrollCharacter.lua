function OnCreate()
    print("Created a new lua script!")
    speed = 100
    yMovement = 0
    xMovement = 0
    gravity = 15
    jumpheight = 6
    jumped = false
    
end

function ValueInRange(value, min, max) 
    return (value >= min) and (value <= max)
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
    yMovement = yMovement + gravity * deltaTime;
    
    if IsMouseDown(0) then
    end
    if IsKeyDown("W") then
        if jumped == false then
            yMovement = jumpheight * -1
            jumped = true
        end
    end
    if IsKeyDown("A") then
        xMovement = xMovement - speed * deltaTime
    end
    if IsKeyDown("S") then
        yMovement = yMovement + (jumpheight * 10) * deltaTime
    end 
    if IsKeyDown("D") then
        xMovement = xMovement + speed * deltaTime
    end
    if not(IsKeyDown("D") and IsKeyDown("A")) then
        xMovement = xMovement * 0.8
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
                    if yMovement > 0 then
                        jumped = false
                    end
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