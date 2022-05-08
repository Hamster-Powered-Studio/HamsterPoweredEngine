function OnCreate()
    print("Created a new lua script!")
    speed = 500
end

function OnUpdate(deltaTime)
    --print("Updated Amogus!")
    --print(1 / deltaTime)
    if IsMouseDown(0) then
    end
    if IsKeyDown("W") then
        trans.GetTransform.Pos.y = trans.GetTransform.Pos.y - speed * deltaTime
    end
    if IsKeyDown("A") then
        trans.GetTransform.Pos.x = trans.GetTransform.Pos.x - speed * deltaTime
    end
    if IsKeyDown("S") then
        trans.GetTransform.Pos.y = trans.GetTransform.Pos.y + speed * deltaTime
    end 
    if IsKeyDown("D") then
        trans.GetTransform.Pos.x = trans.GetTransform.Pos.x + speed * deltaTime
    end
    --print(1 / deltaTime)
    
end

function OnDestroy()
    print ("Destroyed!")
end